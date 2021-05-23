#include "MultiscaleOCR.h"

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
	tess_api.SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_LINE);
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

//Performs OCR on all potential text
void MultiscaleOCR::OCR()
{
	results.clear();

	//Perform horizontal gaussian blur
	//Characters in the same text line should blur together
	cv::Mat invertedIm;
	cv::bitwise_not(image, invertedIm);
	cv::Mat blurredIm;
	cv::GaussianBlur(invertedIm, blurredIm, cv::Size(17, 1), 7, 7);

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
		const cv::Mat roi = image(cv::Rect(left, top, width, height));

		//Scale text to optimal character height
		const double scale = 30.0 / height;
		cv::Mat scaledROI;
		cv::resize(roi, scaledROI, cv::Size(), scale, scale);
		const double inverseScale = height / static_cast<double>(scaledROI.rows);

		//Tesseract struggles when no whitespace border around text, so add a border
		//The horizontal blurring has already provided us with a left/right border
		const int borderTop = 10;
		const int borderBottom = 10;
		const int borderLeft = 0;
		const int borderRight = 0;
		cv::copyMakeBorder(scaledROI, scaledROI, borderTop, borderBottom, borderLeft, borderRight,
			cv::BORDER_CONSTANT, cv::Scalar(255));

		//Set image, perform OCR, and get result iterator
		tess_api.SetImage(scaledROI.data, scaledROI.cols, scaledROI.rows, scaledROI.channels(),
			static_cast<int>(scaledROI.step));
		tess_api.Recognize(NULL);
		auto tess_ri = std::shared_ptr<tesseract::ResultIterator>(tess_api.GetIterator());

		//Filter results
		if (tess_ri != nullptr)
		{
			tess_ri->Begin();
			do
			{
				//Ignore images
				if (tess_ri->BlockType() != PolyBlockType::PT_FLOWING_IMAGE &&
					tess_ri->BlockType() != PolyBlockType::PT_HEADING_IMAGE &&
					tess_ri->BlockType() != PolyBlockType::PT_PULLOUT_IMAGE)
				{
					//Get text bounding box and baseline
					int x1, y1, x2, y2;
					if (!tess_ri->BoundingBox(tesseract::RIL_TEXTLINE, &x1, &y1, &x2, &y2))
						continue;

					int base_x1, base_y1, base_x2, base_y2;
					if (!tess_ri->Baseline(tesseract::RIL_TEXTLINE, &base_x1, &base_y1, &base_x2, &base_y2))
						continue;

					//Convert bounding box and baseline to global space
					//Apply inverse scale to bounding box and baseline
					x1 = left + std::floor((x1 - borderLeft) * inverseScale);
					y1 = top + std::floor((y1 - borderTop) * inverseScale);
					x2 = left + std::ceil((x2 - borderLeft) * inverseScale);
					y2 = top + std::ceil((y2 - borderTop) * inverseScale);

					base_x1 = left + std::floor((base_x1 - borderLeft) * inverseScale);
					base_y1 = top + std::floor((base_y1 - borderTop) * inverseScale);
					base_x2 = left + std::ceil((base_x2 - borderLeft) * inverseScale);
					base_y2 = top + std::ceil((base_y2 - borderTop) * inverseScale);

					//Calculate font metrics of text and add to results
					char *text = tess_ri->GetUTF8Text(tesseract::RIL_TEXTLINE);
					try
					{
						FontMetric metric(blurredROI, cv::Rect(x1, top, x2 - x1, height),
							std::string(text), base_y1 - top);

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

//Clears results
void MultiscaleOCR::clear()
{
	results.clear();
}

//Returns results
const std::vector<FontMetric> &MultiscaleOCR::getResults() const
{
	return results;
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
