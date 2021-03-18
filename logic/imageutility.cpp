#include "imageutility.h"

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

    return QImage();
}
