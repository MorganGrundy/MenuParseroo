#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QGraphicsPixmapItem>

#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "imageutility.h"
#include "maskpainterdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Displays text from clicked text in OCRGraphicsView
    connect(ui->graphicsView, SIGNAL(textBoundClicked(const QString)),
            ui->textBrowser, SLOT(setText(const QString &)));

    //Otsu's binarisation toggle
    connect(ui->checkOtsu, &QCheckBox::toggled,
            this, [=](){setThreshold(ui->thresholdSlider->value());});

    //OCR
    connect(ui->pushOCR, SIGNAL(released()), ui->graphicsView, SLOT(OCR()));

    //Mask painter
    connect(ui->pushPaintMask, SIGNAL(released()), ui->graphicsView, SLOT(editMask()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Prompts user for image
void MainWindow::loadImage()
{
    const QString file = QFileDialog::getOpenFileName(this, tr("Open image"), QDir::currentPath(),
                                                      tr("Images (*.png *.jpg *.jpeg)"));

    if (!file.isEmpty())
    {
        cv::Mat image = cv::imread(file.toStdString(), cv::IMREAD_COLOR);
        if (image.empty())
        {
            //Failed to load image
            QMessageBox messageBox(this);
            messageBox.setText(tr("Failed to load image at: ") + file);
            messageBox.exec();
        }
        else
        {
            //Set image
            ui->graphicsView->setImage(image);
        }
    }
}

//Sets threshold for thresholding image
void MainWindow::setThreshold(const int threshold)
{
    ui->graphicsView->setThreshold(threshold, ui->checkOtsu->isChecked());
}

//Changes displayed level of OCR
void MainWindow::OCRLevelChanged(const QString t_level)
{
    //Select level to iterate
    tesseract::PageIteratorLevel level;
    if (t_level.contains("Word"))
        level = tesseract::RIL_WORD;
    else if (t_level.contains("Textline"))
        level = tesseract::RIL_TEXTLINE;
    else if (t_level.contains("Paragraph"))
        level = tesseract::RIL_PARA;
    else if (t_level.contains("Block"))
        level = tesseract::RIL_BLOCK;
    else
    {
        level = tesseract::RIL_WORD; //Suppress warning of uninitialised variable
        std::cerr << __FILE__":" << __LINE__ << " - OCR level not recognised\n";
        QCoreApplication::exit(-1);
    }

    ui->graphicsView->setOCRLevel(level);
}

//Displayed image choice changed
void MainWindow::imageChoiceChanged(QAbstractButton *button)
{
    if (button->text().contains("Original"))
        ui->graphicsView->showImage(OCRGraphicsView::Image::Original);
    else if (button->text().contains("Gray"))
        ui->graphicsView->showImage(OCRGraphicsView::Image::Gray);
    else if (button->text().contains("Threshold"))
        ui->graphicsView->showImage(OCRGraphicsView::Image::Threshold);
}

//Modifies text from OCR
void MainWindow::textChanged()
{

}
