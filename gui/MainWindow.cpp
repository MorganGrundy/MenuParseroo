#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "ImageUtility.h"
#include "MaskPainterDialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QGraphicsPixmapItem>
#include <QStringListModel>
#include <QHeaderView>

#include <iostream>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	//Create model
	model = new PreprocessStepsTableModel();
	ui->tableView->setModel(model);

	//Insert data into model
	for (int i = 0; i < 5; ++i)
		model->addStep(i, "Test data " + QString::number(i));

	//Resize table view width to fit model
	int width = 0;
	for (int col = 0; col < model->columnCount(); ++col)
		width += ui->tableView->columnWidth(col) + 2;
	ui->tableView->setMinimumWidth(width);
}

MainWindow::~MainWindow()
{
	delete model;
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
			ui->graphicsView->setImage(ImageUtility::matToQPixmap(image));
		}
	}
}