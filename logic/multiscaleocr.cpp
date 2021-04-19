#include "multiscaleocr.h"

#include <iostream>

#include <QDir>

#include <QCoreApplication>
#include <tesseract/genericvector.h>

#include <opencv2/imgproc.hpp>

MultiscaleOCR::MultiscaleOCR()
	: targetScales{ 1.0, 1.75 }
{
	actualScales.resize(targetScales.size());


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

//Performs OCR on all scales
void MultiscaleOCR::OCR()
{
	results.clear();

	//Perform OCR for each scale
	for (size_t i = 0; i < targetScales.size(); ++i)
	{
		//Scale the image
		cv::Mat scaledIm;
		if (targetScales.at(i) > 1)
			cv::resize(image, scaledIm, cv::Size(), targetScales.at(i), targetScales.at(i),
				cv::InterpolationFlags::INTER_CUBIC);
		else if (targetScales.at(i) < 1)
			cv::resize(image, scaledIm, cv::Size(), targetScales.at(i), targetScales.at(i),
				cv::InterpolationFlags::INTER_AREA);
		else
			scaledIm = image;

		//Get exact scale of image
		if (scaledIm.rows > scaledIm.cols)
			actualScales.at(i) = static_cast<double>(image.rows) / scaledIm.rows;
		else
			actualScales.at(i) = static_cast<double>(image.cols) / scaledIm.cols;


		//Set image, perform OCR, and get result iterator
		tess_api.SetImage(scaledIm.data, scaledIm.cols, scaledIm.rows,
			scaledIm.channels(), static_cast<int>(scaledIm.step));
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
					tess_ri->BoundingBox(tesseract::RIL_WORD, &x1, &y1, &x2, &y2);

					int base_x1, base_y1, base_x2, base_y2;
					tess_ri->Baseline(tesseract::RIL_WORD, &base_x1, &base_y1, &base_x2, &base_y2);

					if (y2 - y1 < 25 || x2 - x1 < 10)
						continue;

					try
					{
						FontMetric metric(scaledIm, cv::Rect(x1, y1, x2 - x1, y2 - y1),
							std::string(tess_ri->GetUTF8Text(tesseract::RIL_WORD)),
							base_y1 - y1);

						//if (metric.getAscender() > 25 && metric.getAscender() < 40)
						{
							//std::cerr << "Median before = " << metric.getMedian() << "\n";
							metric.scale(actualScales.at(i));
							//std::cerr << "Median after = " << metric.getMedian() << "\n";
							results.push_back(metric);
						}
					}
					catch (const std::exception &e)
					{
						std::cerr << e.what() << "\n";
					}
				}
			} while ((tess_ri->Next(tesseract::RIL_WORD)));
		}
	}
}

//Return number of scales
size_t MultiscaleOCR::size() const
{
	return targetScales.size();
}

//Returns scale
double MultiscaleOCR::getScale(const size_t i) const
{
	return actualScales.at(i);
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
