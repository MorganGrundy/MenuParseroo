#ifndef MASKPAINTERGRAPHICSVIEW_H
#define MASKPAINTERGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPixmap>
#include <QBitmap>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>

#include "zoomablegraphicsview.h"

/*
 * Displays image and allows editing of alpha (as binary)
*/
class MaskPainterGraphicsView : public ZoomableGraphicsView
{
public:
    explicit MaskPainterGraphicsView(QWidget *parent = nullptr);
    ~MaskPainterGraphicsView();

    enum class Tool {Brush, Fill};

    //Sets the image
    void setImage(const QImage &t_image);

    //Sets the active tool
    void setTool(const Tool t_tool);
    //Returns brush size
    int getBrushSize();

    //Returns image
    QImage getImage();

public slots:
    //Sets brush size
    void setBrushSize(const int t_size);

protected:
    //Mouse events used for painting
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QImage image;
    QGraphicsPixmapItem *imageItem;

    Tool activeTool;
    int brushSize;

    //Draws ellipse on image alpha
    void drawCircle(const QPoint &t_center, const bool active);

    //Updates image item with new image
    void updateImage();
};

#endif // MASKPAINTERGRAPHICSVIEW_H
