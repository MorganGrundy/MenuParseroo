#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

	//Sets threshold for thresholding image
	void setThreshold(const int threshold);

	//Displayed image choice changed
	void imageChoiceChanged(QAbstractButton *button);

	//Modifies text from OCR
	void textChanged();

private:
	Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
