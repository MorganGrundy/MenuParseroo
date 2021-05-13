#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "PreprocessStepsListModel.h"
#include "PreprocessStepsDelegate.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QAbstractButton>

#include <opencv2/core.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

public slots:
	//Prompts user for image
	void loadImage();

private:
	Ui::MainWindow *ui;
	PreprocessStepsListModel *model;
	PreprocessStepsDelegate *delegate;
};
#endif // MAINWINDOW_H
