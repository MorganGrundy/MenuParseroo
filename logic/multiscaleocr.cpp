#include "multiscaleocr.h"

#include <iostream>

#include <QDir>

#include <QCoreApplication>
#include <tesseract/genericvector.h>

#include <opencv2/imgproc.hpp>

MultiscaleOCR::MultiscaleOCR()
{
	//Get path to tesseract config file
	const QDir appDir(qApp->applicationDirPath());
	const QString configFile(appDir.absoluteFilePath("tesseract.config"));
	QByteArray configByteArray = configFile.toLocal8Bit();
	char *configs[] = { configByteArray.data() };

	//Create vars that contain a relative filepath
	GenericVector<STRING> vars, varValues;
	vars.push_back("user_words_file");
	varValues.push_back(appDir.absoluteFilePath("eng.user-words").toLocal8Bit().constData());
	vars.push_back("user_patterns_file");
	varValues.push_back(appDir.absoluteFilePath("eng.user-patterns").toLocal8Bit().constData());

	//Initialize tesseract with English
	if (tess_api.Init(NULL, "eng-fast", tesseract::OcrEngineMode::OEM_LSTM_ONLY, configs, 1,
		&vars, &varValues, true))
	{
		std::cerr << __FILE__":" << __LINE__ << " - Could not initialize tesseract\n";
		QCoreApplication::exit(-1);
	}
	tess_api.SetPageSegMode(tesseract::PageSegMode::PSM_AUTO);
}

MultiscaleOCR::~MultiscaleOCR()
{
	tess_api.End();
}

//Sets image that is OCRed
void MultiscaleOCR::setImage(const cv::Mat &t_image)
{
	image = t_image;
}

#include <opencv2/highgui.hpp>
//Performs OCR on all potential text
void MultiscaleOCR::OCR()
{
	results.clear();

	//Perform horizontal gaussian blur
	//Characters in the same text line should blur together
	cv::Mat invertedIm;
	cv::bitwise_not(image, invertedIm);
	cv::Mat blurredIm;
	cv::GaussianBlur(invertedIm, blurredIm, cv::Size(29, 1), 7, 7);

	//Get components + stats of blurred image (groups text)
	cv::Mat components, stats, centroids;
	const size_t componentCount = cv::connectedComponentsWithStats(blurredIm, components,
		stats, centroids, 8, CV_16U);

	//For each text group perform OCR
	for (int component = 1; component < componentCount; ++component)
	{
		//Get component roi
		const int top = stats.at<int>(component, cv::CC_STAT_TOP);
		const int left = stats.at<int>(component, cv::CC_STAT_LEFT);
		const int width = stats.at<int>(component, cv::CC_STAT_WIDTH);
		const int height = stats.at<int>(component, cv::CC_STAT_HEIGHT);
		//Ignore small components
		if (height < 7)
			continue;
		const cv::Mat blurredROI = blurredIm(cv::Rect(left, top, width, height));
		//Tesseract struggles when no whitespace border around text
		//So need to add a border
		//The horizontal blurring has already provided us with a left/right border
		//Need to clone ROI otherwise border expands ROI instead of creating border
		cv::Mat roi = image(cv::Rect(left, top, width, height)).clone();
		const int borderTop = 10;
		const int borderBottom = 10;
		const int borderLeft = 0;
		const int borderRight = 0;
		cv::copyMakeBorder(roi, roi, borderTop, borderBottom, borderLeft, borderRight,
			cv::BORDER_CONSTANT, cv::Scalar(255));

		//Scale text to optimal character height
		const double scale = 30.0 / height;
		const double inverseScale = height / 30.0;
		cv::Mat scaledROI;
		cv::resize(roi, scaledROI, cv::Size(), scale, scale);

		std::cerr << "Size = " << roi.size() << "\n";
		std::cerr << "Scaled Size = " << scaledROI.size() << "\n";

		cv::namedWindow("ROI", cv::WINDOW_NORMAL);
		cv::imshow("ROI", roi);
		cv::namedWindow("Scaled ROI", cv::WINDOW_NORMAL);
		cv::imshow("Scaled ROI", scaledROI);
		cv::waitKey();
		cv::destroyWindow("ROI");
		cv::destroyWindow("Scaled ROI");

		//Set image, perform OCR, and get result iterator
		tess_api.SetImage(scaledROI.data, scaledROI.cols, scaledROI.rows, scaledROI.channels(),
			static_cast<int>(scaledROI.step));
		tess_api.Recognize(NULL);
		auto tess_ri = std::shared_ptr<tesseract::ResultIterator>(tess_api.GetIterator());

		//Calculate xHeight of text
		const auto [baseline, median] = getBaselineAndMedianRows(blurredROI);
		if (baseline == -1 || median == -1)
			continue;
		const int xHeight = baseline - median;

		//Filter results
		if (tess_ri != nullptr)
		{
			std::cerr << "OCR Results\n";
			tess_ri->Begin();
			do
			{
				std::cerr << "Result Found\n";
				//Ignore images
				if (tess_ri->BlockType() != PolyBlockType::PT_FLOWING_IMAGE &&
					tess_ri->BlockType() != PolyBlockType::PT_HEADING_IMAGE &&
					tess_ri->BlockType() != PolyBlockType::PT_PULLOUT_IMAGE)
				{
					std::cerr << "Results is text\n";
					//Get text bounding box and baseline
					int x1, y1, x2, y2;
					if (!tess_ri->BoundingBox(tesseract::RIL_TEXTLINE, &x1, &y1, &x2, &y2))
						continue;
					std::cerr << "Bounding box\n";

					int base_x1, base_y1, base_x2, base_y2;
					if (!tess_ri->Baseline(tesseract::RIL_TEXTLINE, &base_x1, &base_y1, &base_x2, &base_y2))
						continue;
					std::cerr << "Baseline\n";

					if (y2 - y1 < 25 || x2 - x1 < 10)
						continue;
					std::cerr << "Size\n";

					//Apply inverse scale to bounding box and baseline
					x1 = left + x1 * inverseScale;
					y1 = top + y1 * inverseScale;
					x2 = left + x2 * inverseScale;
					y2 = top + y2 * inverseScale;

					base_x1 = left + base_x1 * inverseScale;
					base_y1 = top + base_y1 * inverseScale;
					base_x2 = left + base_x2 * inverseScale;
					base_y2 = top + base_y2 * inverseScale;

					char *text = tess_ri->GetUTF8Text(tesseract::RIL_TEXTLINE);
					std::cerr << text << "\n";
					try
					{
						FontMetric metric(blurredROI, cv::Rect(x1, y1, x2 - x1, y2 - y1),
							std::string(text), base_y1 - y1);

						results.push_back(metric);
					}
					catch (const std::exception &e)
					{
						std::cerr << e.what() << "\n";
					}
					delete[] text;
				}
			} while ((tess_ri->Next(tesseract::RIL_TEXTLINE)));
		}
	}
}

//Returns the row of the baseline and median
//Assumes roi to be a horizontal gaussian blur of a single line of text
std::pair<int, int> MultiscaleOCR::getBaselineAndMedianRows(const cv::Mat &roi) const
{
	//Calculate row mass (sum of blurred rows)
	cv::Mat rowMasses;
	cv::reduce(roi, rowMasses, 1, cv::ReduceTypes::REDUCE_SUM, CV_32S);
	//Calculate average row mass
	const double averageRowMass = cv::sum(rowMasses)[0] / roi.rows;

	//Find row corresponding to the text baseline
	//The last row with a mass exceeding the average is assumed to be the baseline
	int baselineRow = -1;
	for (int row = roi.rows - 1; row >= 0; --row)
	{
		const int rowMass = rowMasses.at<int>(row, 0);
		if (rowMass > averageRowMass)
		{
			baselineRow = row;
			break;
		}
	}

	//Find row corresponding to the text median
	//The first row with a mass exceeding the average is assumed to be the median
	int medianRow = -1;
	for (int row = 0; row < roi.rows; ++row)
	{
		const int rowMass = rowMasses.at<int>(row, 0);
		if (rowMass > averageRowMass)
		{
			medianRow = row;
			break;
		}
	}

	return { baselineRow, medianRow };
}

//Clears results
void MultiscaleOCR::clear()
{
	results.clear();
}

//Const iterator for beginning of OCR results
std::vector<FontMetric>::const_iterator MultiscaleOCR::begin() const
{
	return results.cbegin();
}

//Const iterator for end of OCR results
std::vector<FontMetric>::const_iterator MultiscaleOCR::end() const
{
	return results.cend();
}
