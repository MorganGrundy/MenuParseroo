#pragma once

#include <QGraphicsPixmapItem>

#include "zoomablegraphicsview.h"

class ZoomableImageView : public ZoomableGraphicsView
{
	Q_OBJECT
public:
	explicit ZoomableImageView(QWidget *parent = nullptr);
	~ZoomableImageView();

	//Sets image for use in graphics view
	void setImage(const QPixmap &t_pixmap);

private:
	QGraphicsPixmapItem *imageItem;
};

