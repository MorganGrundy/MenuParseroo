#ifndef ZOOMABLEGRAPHICSVIEW_H
#define ZOOMABLEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class ZoomableGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ZoomableGraphicsView(QWidget *parent = nullptr);
    ~ZoomableGraphicsView();

private:
    //Modifies zoom by factor
    void zoom(const double factor);

    //Handles scrolling and zooming
    //Ctrl-scroll = zoom
    //Shift-scroll = horizontal scroll
    //Scroll = vertical scroll
    void wheelEvent(QWheelEvent *event) override;
};

#endif // ZOOMABLEGRAPHICSVIEW_H
