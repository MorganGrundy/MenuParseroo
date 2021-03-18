#include "zoomablegraphicsview.h"

ZoomableGraphicsView::ZoomableGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::RenderHint::Antialiasing);
    setDragMode(NoDrag);
    setTransformationAnchor(ViewportAnchor::AnchorUnderMouse);

    //Centres scene
    Qt::Alignment alignment;
    alignment.setFlag(Qt::AlignmentFlag::AlignHCenter);
    alignment.setFlag(Qt::AlignmentFlag::AlignVCenter);
    setAlignment(alignment);
}

ZoomableGraphicsView::~ZoomableGraphicsView() {}

//Modifies zoom by factor
void ZoomableGraphicsView::zoom(const double factor)
{
    if (sceneRect().isEmpty())
        return;

    scale(factor, factor);
}

//Handles scrolling and zooming
//Ctrl-scroll = zoom
//Shift-scroll = horizontal scroll
//Scroll = vertical scroll
void ZoomableGraphicsView::wheelEvent(QWheelEvent *event)
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
