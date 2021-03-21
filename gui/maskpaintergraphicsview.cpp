#include "maskpaintergraphicsview.h"

#include <iostream>
#include <queue>

#include <QCoreApplication>

MaskPainterGraphicsView::MaskPainterGraphicsView(QWidget *parent) :
    ZoomableGraphicsView(parent), activeTool{Tool::Brush}, brushRadius{50}
{
    setScene(new QGraphicsScene(this));

    //Create pixmap graphics item in scene
    imageItem = scene()->addPixmap(QPixmap());

    //Create tool preview items in scene
    QPen selectionPen(QColor(0, 120, 215, 150));
    QBrush selectionBrush(QColor(0, 120, 215, 100));
    brushPreview = scene()->addEllipse(QRect(), selectionPen, selectionBrush);
    rectPreview = scene()->addRect(QRect(), selectionPen, selectionBrush);
    rectPreview->setVisible(false);
}

MaskPainterGraphicsView::~MaskPainterGraphicsView() {}

//Sets the image
void MaskPainterGraphicsView::setImage(const QImage &t_image)
{
    if (t_image.format() == QImage::Format_RGBA8888)
        image = t_image;
    else
        image = t_image.convertToFormat(QImage::Format_RGBA8888);

    //Set brush radius to 5% of image size
    setBrushRadius(std::round(0.05 * std::min(image.width(), image.height())));

    //Display image
    updateImage();
    setSceneRect(image.rect());
}

//Sets the active tool
void MaskPainterGraphicsView::setTool(const MaskPainterGraphicsView::Tool t_tool)
{
    if (t_tool != activeTool)
    {
        if (activeTool == Tool::Brush)
            brushPreview->setVisible(false);

        if (t_tool == Tool::Brush)
            brushPreview->setVisible(true);
    }

    activeTool = t_tool;
}

//Returns brush radius
int MaskPainterGraphicsView::getBrushRadius()
{
    return brushRadius;
}

//Sets brush radius
void MaskPainterGraphicsView::setBrushRadius(const int t_radius)
{
    brushRadius = t_radius;

    brushPreview->setRect(-brushRadius, -brushRadius, brushRadius * 2 + 1, brushRadius * 2 + 1);
}

//Returns image
QImage MaskPainterGraphicsView::getImage()
{
    return image;
}

//Mouse events used for painting
void MaskPainterGraphicsView::mousePressEvent(QMouseEvent *event)
{
    //Maps left click to inactive, right click to active
    bool active;
    if (event->button() == Qt::LeftButton)
        active = false;
    else if (event->button() == Qt::RightButton)
        active = true;
    else
        return;

    const QPoint clickPos = mapToScene(event->pos()).toPoint();

    if (activeTool == Tool::Brush)
    {
        drawCircle(clickPos, active);
    }
    else if (activeTool == Tool::Rect)
    {
        clickStart = clickPos;
    }
    else if (activeTool == Tool::Fill)
    {
        fillConnectedComponent(clickPos, active);
    }

    updateImage();
}

//Mouse events used for painting mask
void MaskPainterGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    //Maps left click to inactive, right click to active
    bool isClick = true;
    bool active;
    if (event->buttons() == Qt::LeftButton)
        active = false;
    else if (event->buttons() == Qt::RightButton)
        active = true;
    else
        isClick = false;

    const QPoint mousePos = mapToScene(event->pos()).toPoint();

    if (activeTool == Tool::Brush)
    {
        brushPreview->setPos(mousePos);
        if (isClick)
            drawCircle(mousePos, active);
    }

    updateImage();
}

//Mouse events used for painting mask
void MaskPainterGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    //Maps left click to inactive, right click to active
    bool active;
    if (event->button() == Qt::LeftButton)
        active = false;
    else if (event->button() == Qt::RightButton)
        active = true;
    else
        return;

    const QPoint clickPos = mapToScene(event->pos()).toPoint();

    if (activeTool == Tool::Rect)
    {
        drawRect(clickStart, clickPos, active);
    }

    updateImage();
}

//Draws ellipse on image alpha
void MaskPainterGraphicsView::drawCircle(const QPoint &t_center, const bool active)
{
    const int hh = brushRadius * brushRadius;
    const int ww = brushRadius * brushRadius;
    const int hhww = hh * ww;
    int x0 = brushRadius;
    int dx = 0;

    //Draw the horizontal diameter
    if (t_center.y() >= 0 && t_center.y() < image.height())
    {
        for (int x = -brushRadius; x <= brushRadius; ++x)
        {
            if (t_center.x() + x >= 0 && t_center.x() + x < image.width())
            {
                QColor colour = image.pixelColor(t_center.x() + x, t_center.y());
                colour.setAlpha(active ? 255 : 0);
                image.setPixelColor(t_center.x() + x, t_center.y(), colour);
            }
        }
    }

    //Draw both halves at the same time, away from the horizontal diameter
    for (int y = 1; y <= brushRadius; ++y)
    {
        int x1 = x0 - (dx - 1);  //Try slopes of dx - 1 or more
        for ( ; x1 > 0; x1--)
            if (x1*x1*hh + y*y*ww <= hhww)
                break;
        dx = x0 - x1;  //Current approximation of the slope
        x0 = x1;

        if (t_center.y() + y >= 0 && t_center.y() + y < image.height())
        {
            //Draw horizontal line in each half
            for (int x = -x0; x <= x0; ++x)
            {
                if (t_center.x() + x >= 0 && t_center.x() + x < image.width())
                {
                    if (t_center.y() - y >= 0 && t_center.y() - y < image.height())
                    {
                        QColor colour = image.pixelColor(t_center.x() + x, t_center.y() - y);
                        colour.setAlpha(active ? 255 : 0);
                        image.setPixelColor(t_center.x() + x, t_center.y() - y, colour);
                    }

                    if (t_center.y() + y >= 0 && t_center.y() + y < image.height())
                    {
                        QColor colour = image.pixelColor(t_center.x() + x, t_center.y() + y);
                        colour.setAlpha(active ? 255 : 0);
                        image.setPixelColor(t_center.x() + x, t_center.y() + y, colour);
                    }
                }
            }
        }
    }
}

//Draws rectangle on image alpha
void MaskPainterGraphicsView::drawRect(const QPoint &t_start, const QPoint &t_end, const bool active)
{
    const int minX = std::clamp(std::min(t_start.x(), t_end.x()), 0, image.width()-1);
    const int maxX = std::clamp(std::max(t_start.x(), t_end.x()), 0, image.width()-1);
    const int minY = std::clamp(std::min(t_start.y(), t_end.y()), 0, image.height()-1);
    const int maxY = std::clamp(std::max(t_start.y(), t_end.y()), 0, image.height()-1);

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            QColor colour = image.pixelColor(x, y);
            colour.setAlpha(active ? 255 : 0);
            image.setPixelColor(x, y, colour);
        }
    }
}

//Fills the connected component at position on image alpha
void MaskPainterGraphicsView::fillConnectedComponent(const QPoint &t_pos, const bool active)
{
    //Position is in image bounds
    if (t_pos.x() >= 0 && t_pos.x() < image.width() &&
        t_pos.y() >= 0 && t_pos.y() < image.height())
    {
        const QColor targetColour = image.pixelColor(t_pos);
        //Pixel must not have the same active state
        if ((targetColour.alpha() != 0) != active)
        {
            //Iterate over pixels setting new active state and adding adjacent pixels to queue
            std::queue<QPoint> pixels;
            pixels.emplace(t_pos);

            do
            {
                //Get pixel from queue
                const QPoint currentPixel = pixels.front();
                QColor currentColour = image.pixelColor(currentPixel);
                pixels.pop();

                if (currentColour == targetColour)
                {
                    //Set pixel alpha
                    currentColour.setAlpha(active ? 255 : 0);
                    image.setPixelColor(currentPixel, currentColour);

                    //Add adjacent pixels to queue
                    if (currentPixel.x()-1 >= 0 && currentPixel.x()-1 < image.width() &&
                        currentPixel.y() >= 0 && currentPixel.y() < image.height())
                        pixels.emplace(currentPixel.x()-1, currentPixel.y());

                    if (currentPixel.x()+1 >= 0 && currentPixel.x()+1 < image.width() &&
                        currentPixel.y() >= 0 && currentPixel.y() < image.height())
                        pixels.emplace(currentPixel.x()+1, currentPixel.y());

                    if (currentPixel.x() >= 0 && currentPixel.x() < image.width() &&
                        currentPixel.y()-1 >= 0 && currentPixel.y()-1 < image.height())
                        pixels.emplace(currentPixel.x(), currentPixel.y()-1);

                    if (currentPixel.x() >= 0 && currentPixel.x() < image.width() &&
                        currentPixel.y()+1 >= 0 && currentPixel.y()+1 < image.height())
                        pixels.emplace(currentPixel.x(), currentPixel.y()+1);
                }
            }
            while (!pixels.empty());
        }
    }
}

//Updates image item with new image
void MaskPainterGraphicsView::updateImage()
{
    imageItem->setPixmap(QPixmap::fromImage(image));
}
