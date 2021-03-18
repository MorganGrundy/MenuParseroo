#include "maskpainter.h"
#include "ui_maskpainter.h"

#include <iostream>

MaskPainter::MaskPainter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaskPainter)
{
    ui->setupUi(this);

    //Get initial brush size
    ui->spinBrushSize->setValue(ui->graphicsView->getBrushSize());
    //Connect brush size spin box
    connect(ui->spinBrushSize, qOverload<int>(&QSpinBox::valueChanged),
            ui->graphicsView, &MaskPainterGraphicsView::setBrushSize);

    //Connect painter tool buttons
    connect(ui->buttonGroup, qOverload<QAbstractButton *>(&QButtonGroup::buttonReleased),
            this, [=](QAbstractButton *button){
                if (button->text().contains("Brush"))
                    ui->graphicsView->setTool(MaskPainterGraphicsView::Tool::Brush);
                else if (button->text().contains("Fill"))
                    ui->graphicsView->setTool(MaskPainterGraphicsView::Tool::Fill);
                else
                {
                    std::cerr << __FILE__":" << __LINE__ << " - Painter tool not recognised\n";
                    QCoreApplication::exit(-1);
                }
            });
}

MaskPainter::~MaskPainter()
{
    delete ui;
}

//Sets the image
void MaskPainter::setImage(const QImage &t_image, const bool keepMask)
{
    ui->graphicsView->setImage(t_image, keepMask);
}

//Returns mask
QImage MaskPainter::getMask()
{
    return ui->graphicsView->getMask();
}
