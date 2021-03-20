#include "maskpaintergraphicsview.h"

#include <iostream>

#include <QCoreApplication>

MaskPainterGraphicsView::MaskPainterGraphicsView(QWidget *parent) :
    ZoomableGraphicsView(parent), activeTool{Tool::Brush}, brushSize{100}
{
    setScene(new QGraphicsScene(this));

    //Create pixmap graphics item in scene
    imageItem = scene()->addPixmap(QPixmap());
}

MaskPainterGraphicsView::~MaskPainterGraphicsView() {}

//Sets the image
void MaskPainterGraphicsView::setImage(const QImage &t_image)
{
    //Add alpha channel if image has none
    if (t_image.hasAlphaChannel())
        image = t_image;
    else
        image = t_image.convertToFormat(QImage::Format_RGBA8888);

    //Set brush size to 5% of image size
    brushSize = std::round(0.05 * std::min(image.width(), image.height()));

    //Display image
    updateImage();
    setSceneRect(image.rect());
}

//Sets the active tool
void MaskPainterGraphicsView::setTool(const MaskPainterGraphicsView::Tool t_tool)
{
    activeTool = t_tool;
}

//Returns brush size
int MaskPainterGraphicsView::getBrushSize()
{
    return brushSize;
}

//Sets brush size
void MaskPainterGraphicsView::setBrushSize(const int t_size)
{
    brushSize = t_size;
}

//Returns image
QImage MaskPainterGraphicsView::getImage()
{
    return image;
}

//Mouse events used for painting
void MaskPainterGraphicsView::mousePressEvent(QMouseEvent *event)
{
    const QPoint clickPos = mapToScene(event->pos()).toPoint();

    //Maps left click to inactive, right click to active
    bool active;
    if (event->button() == Qt::LeftButton)
        active = false;
    else if (event->button() == Qt::RightButton)
        active = true;
    else
        return;

    if (activeTool == Tool::Brush)
    {
        drawCircle(clickPos, active);
    }
    else if (activeTool == Tool::Fill)
    {
        //Fill connected component
    }

    updateImage();
}

//Mouse events used for painting mask
void MaskPainterGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint clickPos = mapToScene(event->pos()).toPoint();

    //Maps left click to inactive, right click to active
    bool active;
    if (event->buttons() == Qt::LeftButton)
        active = false;
    else if (event->buttons() == Qt::RightButton)
        active = true;
    else
        return;

    if (activeTool == Tool::Brush)
    {
        drawCircle(clickPos, active);
    }

    updateImage();
}

//Draws ellipse on image alpha
void MaskPainterGraphicsView::drawCircle(const QPoint &t_center, const bool active)
{
    const int diameter = brushSize * 2 + 1;
    const int hh = diameter * diameter;
    const int ww = diameter * diameter;
    const int hhww = hh * ww;
    int x0 = diameter;
    int dx = 0;

    //Draw the horizontal diameter
    if (t_center.y() >= 0 && t_center.y() < image.height())
    {
        for (int x = -diameter; x <= diameter; ++x)
        {
            if (t_center.x() + x >= 0 && t_center.x() + x < image.width())
            {
                QColor colour = image.pixelColor(t_center.x() + x, t_center.y());
                colour.setAlpha(active ? 255 : 0);
                image.setPixelColor(t_center.x() + x, t_center.y(), colour);
            }
        }
    }

    //Draw both halves at the same time, away from the diameter
    for (int y = 1; y <= diameter; ++y)
    {
        int x1 = x0 - (dx - 1);  //Try slopes of dx - 1 or more
        for ( ; x1 > 0; x1--)
            if (x1*x1*hh + y*y*ww <= hhww)
                break;
        dx = x0 - x1;  //Current approximation of the slope
        x0 = x1;

        //Draw horizontal line in each half
        for (int x = -x0; x <= x0; ++x)
        {
            if (t_center.x() + x >= 0 && t_center.x() + x < image.width() &&
                t_center.y() - y >= 0 && t_center.y() - y < image.height())
            {
                QColor colour = image.pixelColor(t_center.x() + x, t_center.y() - y);
                colour.setAlpha(active ? 255 : 0);
                image.setPixelColor(t_center.x() + x, t_center.y() - y, colour);
            }

            if (t_center.x() + x >= 0 && t_center.x() + x < image.width() &&
                t_center.y() + y >= 0 && t_center.y() + y < image.height())
            {
                QColor colour = image.pixelColor(t_center.x() + x, t_center.y() + y);
                colour.setAlpha(active ? 255 : 0);
                image.setPixelColor(t_center.x() + x, t_center.y() + y, colour);
            }
        }
    }
}

//Updates image item with new image
void MaskPainterGraphicsView::updateImage()
{
    imageItem->setPixmap(QPixmap::fromImage(image));
}
