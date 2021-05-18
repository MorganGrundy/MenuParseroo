#pragma once
#include "UpDownButton.h"
#include "PreprocessStep.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class PreprocessStepWidget : public QFrame
{
	Q_OBJECT

public:
	PreprocessStepWidget(QWidget *parent);
	~PreprocessStepWidget();

	void loadStepData();

	//Applies the preprocess step to an image
	void preprocess(const cv::Mat &t_in, cv::Mat &t_out);


signals:
	void moveUp();
	void moveDown();

	void deleteReleased();

private:
	QHBoxLayout *m_layout;
	UpDownButton *m_upDownButton;
	QLabel *m_label;
	QPushButton *m_delButton;

	static int id;

	PreprocessStep *m_step;
};
