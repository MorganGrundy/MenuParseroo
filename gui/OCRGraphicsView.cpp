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
	//Delete old font metric items
	for (auto item : fontMetricItems)
		delete item;
	fontMetricItems.clear();

	for (const auto &result : t_data)
	{
		//Add font metric item to scene
		fontMetricItems.push_back(new GraphicsFontMetricItem(result));
		scene()->addItem(fontMetricItems.back());
	}
}

//Modifies the selected text
void OCRGraphicsView::modifySelectedText(const bool t_valid)
{
	if (selectedText != nullptr)
	{
		selectedText->setValid(t_valid);
	}
}

//Modifies the selected text
void OCRGraphicsView::modifySelectedText(const std::string &t_text)
{
	if (selectedText != nullptr)
	{
		selectedText->setText(t_text);
	}
}

//Emits OCR data of clicked text
void OCRGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		//Get all items at click
		auto clickedItems = items(event->pos());
		//Remove any non font metric items
		clickedItems.erase(std::remove_if(clickedItems.begin(), clickedItems.end(), [](QGraphicsItem *item) -> bool {
			return dynamic_cast<GraphicsFontMetricItem *>(item) == nullptr;
		}), clickedItems.end());

		if (!clickedItems.isEmpty())
		{
			//If one of the items is already selected then select the next one
			const int currentIndex = clickedItems.indexOf(selectedText);
			const int nextIndex = (currentIndex + 1) % clickedItems.size();

			//Get clicked item
			GraphicsFontMetricItem *clickedItem = dynamic_cast<GraphicsFontMetricItem *>(clickedItems.at(nextIndex));

			//Deselect previous item
			if (selectedText != nullptr)
				selectedText->setGraphicsEffect(nullptr);
			selectedText = clickedItem;

			//Change color of item to green
			QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect();
			effect->setColor(QColor(0, 255, 0));
			effect->setStrength(1);
			selectedText->setGraphicsEffect(effect);

			emit textClicked(clickedItem->getFontMetric().isValid(), clickedItem->getFontMetric().getText());
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
