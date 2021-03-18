#include "maskpaintergraphicsview.h"

#include <iostream>

#include <QCoreApplication>

MaskPainterGraphicsView::MaskPainterGraphicsView(QWidget *parent) :
    QGraphicsView(parent), activeTool{Tool::Brush}, brushSize{100}
{
    setScene(new QGraphicsScene(this));

    //Create pixmap graphics item in scene
    imageItem = scene()->addPixmap(QPixmap());
}

MaskPainterGraphicsView::~MaskPainterGraphicsView() {}

//Sets the image
//Keep mask will keep the previous mask (as long as the size is the same)
void MaskPainterGraphicsView::setImage(const QImage &t_image, const bool keepMask)
{
    image = t_image;

    if (keepMask && mask.size() == image.size())
        updatePixmap();
    else
    {
        //Create new mask of image size
        mask = QImage(image.size(), QImage::Format::Format_Grayscale8);
        clearMask();
    }

    //Set brush size to 5% of image size
    brushSize = std::round(0.05 * std::min(image.width(), image.height()));

    //Display pixmap
    updatePixmap();
    setSceneRect(imageItem->pixmap().rect());
    fitInView(sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
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

//Clears the current mask
void MaskPainterGraphicsView::clearMask()
{
    //Set all mask pixels to active
    QPainter painter(&mask);
    painter.setBrush(Qt::GlobalColor::white);
    painter.drawRect(mask.rect());
}

//Returns mask
QImage MaskPainterGraphicsView::getMask()
{
    return mask;
}

//Mouse events used for painting mask
void MaskPainterGraphicsView::mousePressEvent(QMouseEvent *event)
{
    const QPoint clickPos = mapToScene(event->pos()).toPoint();

    //Maps left click to inactive, right click to active
    QPainter painter(&mask);
    painter.setPen(Qt::NoPen);
    if (event->button() == Qt::LeftButton)
        painter.setBrush(Qt::GlobalColor::black);
    else if (event->button() == Qt::RightButton)
        painter.setBrush(Qt::GlobalColor::white);
    else
        return;

    if (activeTool == Tool::Brush)
    {
        //Draw circle on mask
        painter.drawEllipse(clickPos, brushSize, brushSize);
    }
    else if (activeTool == Tool::Fill)
    {
        //Fill connected component
    }

    updatePixmap();
}

//Mouse events used for painting mask
void MaskPainterGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint clickPos = mapToScene(event->pos()).toPoint();

    //Maps left click to inactive, right click to active
    QPainter painter(&mask);
    painter.setPen(Qt::NoPen);
    if (event->buttons() == Qt::LeftButton)
        painter.setBrush(Qt::GlobalColor::black);
    else if (event->buttons() == Qt::RightButton)
        painter.setBrush(Qt::GlobalColor::white);
    else
        return;

    if (activeTool == Tool::Brush)
    {
        //Draw circle on mask
        painter.drawEllipse(clickPos, brushSize, brushSize);
        updatePixmap();
    }
}

//Updates displayed pixmap with new mask
void MaskPainterGraphicsView::updatePixmap()
{
    QImage newImage(image);
    newImage.setAlphaChannel(mask);
    imageItem->setPixmap(QPixmap::fromImage(newImage));
}
