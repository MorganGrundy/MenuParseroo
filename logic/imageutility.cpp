#include "imageutility.h"

#include <iostream>

#include <QCoreApplication>

//Converts an OpenCV Mat to a QPixmap
QPixmap ImageUtility::matToQPixmap(const cv::Mat &t_mat)
{
    //Single channel is grayscale
    if (t_mat.channels() == 1)
    {
        return QPixmap::fromImage(
            QImage(t_mat.data, t_mat.cols, t_mat.rows, static_cast<int>(t_mat.step),
                   QImage::Format_Grayscale8));
    }
    //3-channel is BGR
    else if (t_mat.channels() == 3)
    {
        return QPixmap::fromImage(
            QImage(t_mat.data, t_mat.cols, t_mat.rows, static_cast<int>(t_mat.step),
                   QImage::Format_BGR888));
    }

    std::cerr << __FILE__":" << __LINE__ << " - Image type not supported\n";
    QCoreApplication::exit(-1);
    return QPixmap();
}

//Converts an OpenCV Mat to a QImage
QImage ImageUtility::matToQImage(const cv::Mat &t_mat)
{
    //Single channel is grayscale
    if (t_mat.channels() == 1)
    {
        return QImage(t_mat.data, t_mat.cols, t_mat.rows, static_cast<int>(t_mat.step),
                   QImage::Format_Grayscale8);
    }
    //3-channel is BGR
    else if (t_mat.channels() == 3)
    {
        return QImage(t_mat.data, t_mat.cols, t_mat.rows, static_cast<int>(t_mat.step),
                   QImage::Format_BGR888);
    }

    std::cerr << __FILE__":" << __LINE__ << " - Image type not supported\n";
    QCoreApplication::exit(-1);
    return QImage();
}

//Converts a QImage to an OpenCV Mat
cv::Mat ImageUtility::qImageToMat(const QImage &t_image)
{
    if (t_image.format() == QImage::Format_RGBA8888)
    {
        cv::Mat mat(t_image.height(), t_image.width(), CV_8UC3);
        uchar *rowPtr;
        for (int row = 0; row < t_image.height(); ++row)
        {
            rowPtr = mat.ptr(row);
            for (int col = 0; col < t_image.width(); ++col)
            {
                const QColor pixel = t_image.pixelColor(col, row);
                if (pixel.alpha() != 0)
                {
                    rowPtr[col*3] = pixel.blue();
                    rowPtr[col*3+1] = pixel.green();
                    rowPtr[col*3+2] = pixel.red();
                }
                else
                {
                    rowPtr[col*3] = 255;
                    rowPtr[col*3+1] = 255;
                    rowPtr[col*3+2] = 255;
                }
            }
        }

        return mat;
    }

    std::cerr << __FILE__":" << __LINE__ << " - Image type not supported\n";
    QCoreApplication::exit(-1);
    return cv::Mat();
}
