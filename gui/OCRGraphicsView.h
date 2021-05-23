#ifndef OCRGRAPHICSVIEW_H
#define OCRGRAPHICSVIEW_H

#include "ZoomableImageView.h"
#include "GraphicsFontMetricItem.h"

#include <QGraphicsView>
#include <QWheelEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsColorizeEffect>

class OCRGraphicsView : public ZoomableImageView
{
	Q_OBJECT
public:
	explicit OCRGraphicsView(QWidget *parent = nullptr);
	~OCRGraphicsView();

	//Sets the data to display
	void setData(const std::vector<FontMetric> &t_data);

signals:
	//Shares the text that was clicked
	void textBoundClicked(const QString t_text);

protected:
	//Emits OCR data of clicked text
	void mouseReleaseEvent(QMouseEvent *event) override;

private:
	//Font metric items for text (also has text in data 0)
	std::vector<GraphicsFontMetricItem *> fontMetricItems;
	QGraphicsItem *selectedText;

	//-----------------------------------------------------
	//Clears font metric items
	void clearFontMetricItems();
};

#endif // OCRGRAPHICSVIEW_H
