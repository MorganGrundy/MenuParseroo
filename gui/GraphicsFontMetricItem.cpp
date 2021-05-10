#include "GraphicsFontMetricItem.h"

GraphicsFontMetricItem::GraphicsFontMetricItem(const FontMetric &fontMetric, QGraphicsItem *parent)
	: QGraphicsItem{ parent }, rect(fontMetric.getBounds().x, fontMetric.getBounds().y,
		fontMetric.getBounds().width, fontMetric.getBounds().height),
	metrics{ fontMetric }
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

	const int capitalLine = rect.top() + metrics.getBaseline() - metrics.getCapHeight();
	painter->drawLine(rect.left(), capitalLine, rect.right(), capitalLine);

	const int medianLine = rect.top() + metrics.getBaseline() - metrics.getXHeight();
	painter->drawLine(rect.left(), medianLine, rect.right(), medianLine);

	painter->drawLine(rect.left(), rect.top() + metrics.getBaseline(), rect.right(), rect.top() + metrics.getBaseline());
}
