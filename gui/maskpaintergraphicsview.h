#ifndef MASKPAINTERGRAPHICSVIEW_H
#define MASKPAINTERGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPixmap>
#include <QBitmap>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>

#include "zoomablegraphicsview.h"

/*
 * Displays image and allows editing of mask
 * Alpha channel used as mask (only with 0 or 255)
*/
class MaskPainterGraphicsView : public ZoomableGraphicsView
{
public:
    explicit MaskPainterGraphicsView(QWidget *parent = nullptr);
    ~MaskPainterGraphicsView();

    enum class Tool {Brush, Fill};

    //Sets the image
    void setImage(const QImage &t_image, const bool keepMask = false);

    //Sets the active tool
    void setTool(const Tool t_tool);
    //Returns brush size
    int getBrushSize();

    //Clears the current mask
    void clearMask();
    //Returns mask
    QImage getMask();

public slots:
    //Sets brush size
    void setBrushSize(const int t_size);

private:
    QImage image, mask;
    QGraphicsPixmapItem *imageItem;

    Tool activeTool;
    int brushSize;

    //Mouse events used for painting mask
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    //Updates displayed pixmap with new mask
    void updatePixmap();
};

#endif // MASKPAINTERGRAPHICSVIEW_H
