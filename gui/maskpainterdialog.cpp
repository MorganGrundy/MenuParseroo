#include "MaskPainterDialog.h"
#include "ui_maskpainterdialog.h"

#include <iostream>

MaskPainterDialog::MaskPainterDialog(const QImage &t_image, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MaskPainterDialog)
{
	ui->setupUi(this);

	ui->graphicsView->setImage(t_image);

	//Get initial brush size
	ui->spinBrushSize->setValue(ui->graphicsView->getBrushRadius() * 2 + 1);
	//Connect brush size spin box
	connect(ui->spinBrushSize, qOverload<int>(&QSpinBox::valueChanged),
		this, [=](const int size) {
			ui->graphicsView->setBrushRadius((size - 1) / 2);
		});

	//Connect painter tool buttons
	connect(ui->buttonGroup, qOverload<QAbstractButton *>(&QButtonGroup::buttonReleased),
		this, [=](QAbstractButton *button) {
			if (button->text().contains("Brush"))
				ui->graphicsView->setTool(MaskPainterGraphicsView::Tool::Brush);
			else if (button->text().contains("Rect"))
				ui->graphicsView->setTool(MaskPainterGraphicsView::Tool::Rect);
			else if (button->text().contains("Fill"))
				ui->graphicsView->setTool(MaskPainterGraphicsView::Tool::Fill);
			else
			{
				std::cerr << __FILE__":" << __LINE__ << " - Painter tool not recognised\n";
				QCoreApplication::exit(-1);
			}
		});
}

MaskPainterDialog::~MaskPainterDialog()
{
	delete ui;
}

//Returns resulting image with mask
QImage MaskPainterDialog::getImage()
{
	return ui->graphicsView->getImage();
}

//Resizes graphics scene to fit in view
void MaskPainterDialog::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);
	ui->graphicsView->fitInView(ui->graphicsView->sceneRect(), Qt::KeepAspectRatio);
}
