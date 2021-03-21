#ifndef MASKPAINTERGRAPHICSVIEW_H
#define MASKPAINTERGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPixmap>
#include <QBitmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
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

    enum class Tool {Brush, Rect, Fill};

    //Sets the image
    void setImage(const QImage &t_image);

    //Sets the active tool
    void setTool(const Tool t_tool);
    //Returns brush radius
    int getBrushRadius();

    //Returns image
    QImage getImage();

public slots:
    //Sets brush radius
    void setBrushRadius(const int t_radius);

protected:
    //Mouse events used for painting
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QImage image;
    QGraphicsPixmapItem *imageItem;

    Tool activeTool;
    int brushRadius;
    QGraphicsEllipseItem *brushPreview;

    QPoint clickStart;
    QGraphicsRectItem *rectPreview;

    //Draws ellipse on image alpha
    void drawCircle(const QPoint &t_center, const bool active);
    //Draws rectangle on image alpha
    void drawRect(const QPoint &t_start, const QPoint &t_end, const bool active);
    //Fills the connected component at position on image alpha
    void fillConnectedComponent(const QPoint &t_pos, const bool active);

    //Updates image item with new image
    void updateImage();
};

#endif // MASKPAINTERGRAPHICSVIEW_H
