#include "ZoomableImageView.h"

ZoomableImageView::ZoomableImageView(QWidget *parent)
	: ZoomableGraphicsView{ parent }
{
	//Create scene
	setScene(new QGraphicsScene());

	imageItem = scene()->addPixmap(QPixmap());
}

ZoomableImageView::~ZoomableImageView()
{
	scene()->removeItem(imageItem);
	delete imageItem;
}

//Sets image for use in graphics view
void ZoomableImageView::setImage(const QPixmap &t_pixmap)
{
	imageItem->setPixmap(t_pixmap);

	//Update scene
	setSceneRect(t_pixmap.rect());
	fitInView(sceneRect(), Qt::KeepAspectRatio);
}
