#include "OCRGraphicsView.h"
#include "ImageUtility.h"
#include "MaskPainterDialog.h"
#include "asmOpenCV.h"

#include <iostream>

#include <QCoreApplication>
#include <QDir>

#include <opencv2/imgproc.hpp>

#include <tesseract/genericvector.h>

OCRGraphicsView::OCRGraphicsView(QWidget *parent)
	: ZoomableImageView(parent), selectedText{ nullptr }
{
}

OCRGraphicsView::~OCRGraphicsView() {}

//Sets the data to display
void OCRGraphicsView::setData(const std::vector<FontMetric> &t_data)
{
	for (const auto &result : t_data)
	{
		//Add font metric item to scene
		fontMetricItems.push_back(new GraphicsFontMetricItem(result));
		scene()->addItem(fontMetricItems.back());
		//Add OCR text to item data, also add ascender and descender of font metrics
		fontMetricItems.back()->setData(0, QVariant(QString::fromStdString(result.getText())));
	}
}

//Emits OCR data of clicked text
void OCRGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		//Font metric items have text stored in data so can filter by that
		auto clickedItem = itemAt(event->pos());
		if (clickedItem != nullptr && !clickedItem->data(0).isNull())
		{
			//Deselect previous item
			if (selectedText != nullptr)
				selectedText->setGraphicsEffect(nullptr);

			//Change color of item to red
			selectedText = clickedItem;
			QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect();
			effect->setColor(QColor(255, 0, 0));
			effect->setStrength(1);
			selectedText->setGraphicsEffect(effect);

			emit textBoundClicked(clickedItem->data(0).toString());
		}
	}
	QGraphicsView::mouseReleaseEvent(event);
}

//Clears font metric items
void OCRGraphicsView::clearFontMetricItems()
{
	selectedText = nullptr;

	for (auto fontMetricItem : fontMetricItems)
	{
		scene()->removeItem(fontMetricItem);
		delete fontMetricItem;
	}
	fontMetricItems.clear();
}
