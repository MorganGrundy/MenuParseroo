#ifndef GRAPHICSFONTMETRICITEM_H
#define GRAPHICSFONTMETRICITEM_H

#include <QGraphicsItem>
#include <QPainter>

class GraphicsFontMetricItem : public QGraphicsItem
{
public:
    GraphicsFontMetricItem(const qreal x, const qreal y, const qreal width, const qreal height,
                             const int t_baseline, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    QRectF rect;
    int baseline;
};

#endif // GRAPHICSFONTMETRICITEM_H
