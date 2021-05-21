#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "ImageUtility.h"
#include "BinaryThresholdPreprocessStepWidget.h"
#include "GrayscalePreprocessStepWidget.h"
#include "MaskPreprocessStepWidget.h"
#include "asmOpenCV.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	//Update graphics view when preprocessed image changes
	connect(ui->preprocessStepList, &PreprocessStepListWidget::imageUpdated,
		this, [=](const cv::Mat &t_image) { ui->graphicsView->setImage(ASM::cvMatToQPixmap(t_image)); });

	//Add menu to add step button
	menu = new QMenu(this);
	menu->addAction("Convert to Grayscale", this, [=]() { ui->preprocessStepList->addStep(new GrayscalePreprocessStepWidget()); });
	menu->addAction("Binary Threshold", this, [=]() { ui->preprocessStepList->addStep(new BinaryThresholdPreprocessStepWidget()); });
	menu->addAction("Apply Mask", this, [=]() { ui->preprocessStepList->addStep(new MaskPreprocessStepWidget()); });
	ui->toolAddStep->setMenu(menu);
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
			ui->preprocessStepList->setImage(image);
		}
	}
}