#include "multiscaleocr.h"

#include <iostream>

#include <QDir>

#include <QCoreApplication>
#include <tesseract/genericvector.h>

#include <opencv2/imgproc.hpp>

MultiscaleOCR::MultiscaleOCR()
    : targetScales{0.5, 1.0, 2.0}
{
    actualScales.resize(targetScales.size());

    //Create new tesseract api
    const size_t oldSize = tess_api.size();
    tess_api.resize(targetScales.size());

    //Initialise new tesseract api
    for (size_t i = oldSize; i < tess_api.size(); ++i)
    {
        //Get path to tesseract config file
        const QDir appDir(qApp->applicationDirPath());
        const QString configFile(appDir.absoluteFilePath("tesseract.config"));
        QByteArray configByteArray = configFile.toLocal8Bit();
        char *configs[] = {configByteArray.data()};

        //Create vars that contain a relative filepath
        GenericVector<STRING> vars, varValues;
        vars.push_back("user_words_file");
        varValues.push_back(appDir.absoluteFilePath("eng.user-words").toLocal8Bit().constData());
        vars.push_back("user_patterns_file");
        varValues.push_back(appDir.absoluteFilePath("eng.user-patterns").toLocal8Bit().constData());

        //Initialize tesseract with English
        if (tess_api.at(i).Init(NULL, "eng-fast", tesseract::OcrEngineMode::OEM_LSTM_ONLY, configs, 1,
                                &vars, &varValues, true))
        {
            std::cerr << __FILE__":" << __LINE__ << " - Could not initialize tesseract\n";
            QCoreApplication::exit(-1);
        }
        tess_api.at(i).SetPageSegMode(tesseract::PageSegMode::PSM_AUTO);

        //Saves alternate symbol choices from OCR
        //tess_api.at(i).SetVariable("save_blob_choices", "T");

        //Output tesseract variables to a file
        //tess_api.at(i).PrintVariables(fopenWriteStream("E:/Desktop/MenuParseroo/variables.txt", "w"));
    }

    //Create new result pointers
    results.resize(targetScales.size());
}

MultiscaleOCR::~MultiscaleOCR()
{
    for (auto api: tess_api)
        api.End();
}

//Sets image that is OCRed
void MultiscaleOCR::setImage(const cv::Mat &t_image)
{
    image = t_image;
}

//Performs OCR on all scales
void MultiscaleOCR::OCR()
{
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
        tess_api.at(i).SetImage(scaledIm.data, scaledIm.cols, scaledIm.rows,
                          scaledIm.channels(), static_cast<int>(scaledIm.step));
        tess_api.at(i).Recognize(NULL);
        results.at(i) = std::shared_ptr<tesseract::ResultIterator>(tess_api.at(i).GetIterator());
    }
}

//Return number of scales
size_t MultiscaleOCR::size()
{
    return targetScales.size();
}

//Returns result iterator
std::shared_ptr<tesseract::ResultIterator> MultiscaleOCR::getResult(const size_t i)
{
    return results.at(i);
}

//Returns scale
double MultiscaleOCR::getScale(const size_t i)
{
    return actualScales.at(i);
}

//Clears results
void MultiscaleOCR::clear()
{
    for (auto result: results)
        result.reset();
}
