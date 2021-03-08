#include "ocrgraphicsview.h"

#include "imageutility.h"

#include <iostream>

#include <QCoreApplication>

#include <opencv2/imgproc.hpp>

OCRGraphicsView::OCRGraphicsView(QWidget *parent)
    : QGraphicsView(parent),
    threshold{127}, otsu{false},
    currentImage{Image::Original},
    tess_ri{}, tess_level{tesseract::PageIteratorLevel::RIL_WORD},
    selectedText{nullptr}
{
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(AnchorUnderMouse);

    //Centres image
    Qt::Alignment alignment;
    alignment.setFlag(Qt::AlignmentFlag::AlignHCenter);
    alignment.setFlag(Qt::AlignmentFlag::AlignVCenter);
    setAlignment(alignment);

    //Create scene
    setScene(new QGraphicsScene());
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);

    //Create pixmap items
    originalImageItem = scene()->addPixmap(QPixmap());
    grayImageItem = scene()->addPixmap(QPixmap());
    thresholdImageItem = scene()->addPixmap(QPixmap());
    grayImageItem->hide();
    thresholdImageItem->hide();

    //Initialize tesseract-ocr with English, without specifying tessdata path
    if (tess_api.Init(NULL, "eng"))
    {
        std::cerr << __FILE__":" << __LINE__ << " - Could not initialize tesseract\n";
        QCoreApplication::exit(-1);
    }
    //Set character whitelist
    tess_api.SetVariable("tessedit_char_whitelist",
                         " abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ,.£$&-");
    tess_api.SetVariable("save_blob_choices", "T");
    tess_api.SetPageSegMode(tesseract::PageSegMode::PSM_AUTO);
}

OCRGraphicsView::~OCRGraphicsView()
{
    tess_api.End();
}

//Sets image of graphics view at given index
void OCRGraphicsView::setImage(const cv::Mat &t_image)
{
    //Convert cv::Mat to QPixmap
    const QPixmap pixmap = ImageUtility::matToQPixmap(t_image);

    //Set new image
    originalImage = t_image;
    originalImageItem->setPixmap(pixmap);

    //Grayscale image
    cv::cvtColor(originalImage, grayImage, cv::COLOR_BGR2GRAY);
    //Set new gray image
    grayImageItem->setPixmap(ImageUtility::matToQPixmap(grayImage));

    //Threshold image
    setThreshold(threshold, otsu, true);

    //Update scene
    setSceneRect(pixmap.rect());
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}

//Sets threshold for thresholding image
void OCRGraphicsView::setThreshold(const int t_threshold, const bool t_otsu, const bool force)
{
    //Only threshold image if gray image not empty and:
    //Force is true, or
    //Otsu is different, or
    //Otsu is false and threshold is different
    if (!grayImage.empty() && (force || (otsu != t_otsu) || (!t_otsu && t_threshold != threshold)))
    {
        //Otsu's binarisation
        if (t_otsu)
        {
            cv::Mat blurredImage;
            cv::GaussianBlur(grayImage, blurredImage, cv::Size(5, 5), 0);
            cv::threshold(blurredImage, thresholdImage, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
        }
        //Binary threshold
        else
            cv::threshold(grayImage, thresholdImage, t_threshold, 255, cv::THRESH_BINARY);

        //Set new threshold image
        thresholdImageItem->setPixmap(ImageUtility::matToQPixmap(thresholdImage));

        //Clear old OCR data
        clearTesseract();
    }

    threshold = t_threshold;
    otsu = t_otsu;
}

//Shows image of type
void OCRGraphicsView::showImage(const Image t_type)
{
    if (t_type != currentImage)
    {
        getFontMetricItem(currentImage)->hide();
        getFontMetricItem(t_type)->show();
        currentImage = t_type;
    }
}

//OCRs threshold image
void OCRGraphicsView::OCR()
{
    if (!thresholdImage.empty())
    {
        tess_api.SetImage(thresholdImage.data, thresholdImage.cols, thresholdImage.rows,
                          thresholdImage.channels(), static_cast<int>(thresholdImage.step));
        tess_api.Recognize(NULL);
        tess_ri = std::shared_ptr<tesseract::ResultIterator>(tess_api.GetIterator());

        setOCRLevel(tess_level);
    }
}

//Sets the level of OCR results that are shown
void OCRGraphicsView::setOCRLevel(const tesseract::PageIteratorLevel t_level)
{
    clearFontMetricItems();

    //Iterate level, adding font metric items
    if(tess_ri != nullptr)
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
                tess_ri->BoundingBox(t_level, &x1, &y1, &x2, &y2);
                int base_x1, base_y1, base_x2, base_y2;
                tess_ri->Baseline(t_level, &base_x1, &base_y1, &base_x2, &base_y2);

                //Add font metric item to scene
                fontMetricItems.push_back(new GraphicsFontMetricItem(x1, y1, x2 - x1,
                                                                     y2 - y1, base_y1 - y1));
                scene()->addItem(fontMetricItems.back());
                //Add OCR text to item data
                fontMetricItems.back()->setData(0, QVariant(tess_ri->GetUTF8Text(t_level)));
            }
        } while((tess_ri->Next(t_level)));
    }
}

//Returns font metric item that matches type
QGraphicsPixmapItem *OCRGraphicsView::getFontMetricItem(const OCRGraphicsView::Image t_type)
{
    switch(t_type)
    {
    case Image::Original: return originalImageItem;
    case Image::Gray: return grayImageItem;
    case Image::Threshold: return thresholdImageItem;
    default:
    {
        std::cerr << __FILE__":" << __LINE__ << " - Image type not recognised\n";
        QCoreApplication::exit(-1);
        return nullptr;
    }
    }
}

//Clears tesseract result
void OCRGraphicsView::clearTesseract()
{
    tess_ri.reset();
    clearFontMetricItems();
}

//Clears font metric items
void OCRGraphicsView::clearFontMetricItems()
{
    selectedText = nullptr;

    for (auto fontMetricItem: fontMetricItems)
    {
        scene()->removeItem(fontMetricItem);
        delete fontMetricItem;
    }
    fontMetricItems.clear();
}

//Modifies zoom by factor
void OCRGraphicsView::zoom(const double factor)
{
    if (sceneRect().isEmpty())
        return;

    scale(factor, factor);
}

//Handles scrolling and zooming
//Ctrl-scroll = zoom
//Shift-scroll = horizontal scroll
//Scroll = vertical scroll
void OCRGraphicsView::wheelEvent(QWheelEvent *event)
{
    const double zoomScale = 1.1;
    //Ctrl-scrollwheel modifies image zoom
    if (event->modifiers() == Qt::ControlModifier)
    {
        zoom((event->angleDelta().y() > 0) ? zoomScale : 1.0 / zoomScale);
    }
    //Shift-scrollwheel scrolls horizontally
    else if (event->modifiers() == Qt::ShiftModifier)
    {
        //Swap vertical and horizontal delta
        QPoint pixelDelta(event->pixelDelta().y(), event->pixelDelta().x());
        QPoint angleDelta(event->angleDelta().y(), event->angleDelta().x());
        QWheelEvent horizontalScrollEvent(event->position(), event->globalPosition(),
                                          pixelDelta, angleDelta, event->buttons(),
                                          Qt::NoModifier, event->phase(), event->inverted());
        QGraphicsView::wheelEvent(&horizontalScrollEvent);
    }
    //Vertical scrolling
    else if (event->modifiers() == Qt::NoModifier)
        QGraphicsView::wheelEvent(event);
}

//Emits OCR data of clicked text
void OCRGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton)
    {
        //Font metric items have text stored in data so can filter by that
        auto clickedItem = itemAt(event->pos());
        if (clickedItem != nullptr && !clickedItem->data(0).isNull())
        {
            //Deselect previous item
            if (selectedText != nullptr)
                selectedText->setGraphicsEffect(nullptr);

            //Change color of item to red
            selectedText = clickedItem;
            QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect();
            effect->setColor(QColor(255, 0, 0));
            effect->setStrength(1);
            selectedText->setGraphicsEffect(effect);

            emit textBoundClicked(clickedItem->data(0).toString());
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}
