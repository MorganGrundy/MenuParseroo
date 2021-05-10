#ifndef GRAPHICSFONTMETRICITEM_H
#define GRAPHICSFONTMETRICITEM_H

#include "FontMetric.h"

#include <QGraphicsItem>
#include <QPainter>

class GraphicsFontMetricItem : public QGraphicsItem
{
public:
	GraphicsFontMetricItem(const FontMetric &fontMetric, QGraphicsItem *parent = nullptr);

	QRectF boundingRect() const override;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget) override;

private:
	QRectF rect;
	FontMetric metrics;
};

#endif // GRAPHICSFONTMETRICITEM_H
