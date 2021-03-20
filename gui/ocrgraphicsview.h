#ifndef OCRGRAPHICSVIEW_H
#define OCRGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsPixmapItem>
#include "graphicsfontmetricitem.h"
#include <QGraphicsColorizeEffect>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <opencv2/core.hpp>

#include "zoomablegraphicsview.h"

class OCRGraphicsView : public ZoomableGraphicsView
{
    Q_OBJECT
public:
    explicit OCRGraphicsView(QWidget *parent = nullptr);
    ~OCRGraphicsView();

    //Sets image of graphics view
    void setImage(const cv::Mat &t_image);
    //Sets threshold for thresholding image
    void setThreshold(const int t_threshold, const bool t_otsu = false, const bool force = false);

    //Enum for images
    enum class Image {Original, Gray, Threshold};
    //Selects which image to display
    void showImage(const Image t_type);

    //Sets the level of OCR results that are shown
    void setOCRLevel(const tesseract::PageIteratorLevel t_level);

public slots:
    //Allows user to edit the mask of the current image
    void editMask();

    //OCRs threshold image
    void OCR();

signals:
    //Shares the text that was clicked
    void textBoundClicked(const QString t_text);

protected:
    //Emits OCR data of clicked text
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    //Stores images
    cv::Mat originalImage, grayImage, thresholdImage;
    QImage imageAndMask;
    QGraphicsPixmapItem *originalImageItem, *grayImageItem, *thresholdImageItem;

    //Threshold
    int threshold;
    bool otsu;

    //Current image
    Image currentImage;

    //Tesseract
    tesseract::TessBaseAPI tess_api;
    std::shared_ptr<tesseract::ResultIterator> tess_ri;
    tesseract::PageIteratorLevel tess_level;

    //Font metric items for text (also has text in data 0)
    std::vector<GraphicsFontMetricItem *> fontMetricItems;
    QGraphicsItem *selectedText;

    //-----------------------------------------------------
    //Creates and displays images
    void updateImages();

    //Returns font metric item that matches type
    QGraphicsPixmapItem *getFontMetricItem(const Image t_type);

    //Clears tesseract result
    void clearTesseract();
    //Clears font metric items
    void clearFontMetricItems();
};

#endif // OCRGRAPHICSVIEW_H
