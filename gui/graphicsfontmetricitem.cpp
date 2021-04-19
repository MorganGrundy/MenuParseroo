#include "graphicsfontmetricitem.h"

GraphicsFontMetricItem::GraphicsFontMetricItem(
	const qreal x, const qreal y, const qreal width, const qreal height,
	const int t_baseline, QGraphicsItem *parent)
	: QGraphicsItem{ parent }, rect{ x, y, width, height }, baseline{ t_baseline }
{}

GraphicsFontMetricItem::GraphicsFontMetricItem(const FontMetric &fontMetric, QGraphicsItem *parent)
	: QGraphicsItem{ parent }, rect(fontMetric.getBounds().x, fontMetric.getBounds().y,
		fontMetric.getBounds().width, fontMetric.getBounds().height),
	baseline{ fontMetric.getBaseline() }
{}

QRectF GraphicsFontMetricItem::boundingRect() const
{
	return rect;
}

void GraphicsFontMetricItem::paint(QPainter *painter,
	[[maybe_unused]] const QStyleOptionGraphicsItem *option,
	[[maybe_unused]] QWidget *widget)
{
	painter->drawRect(rect);
	painter->drawLine(rect.left(), rect.top() + baseline, rect.right(), rect.top() + baseline);
}
