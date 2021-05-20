#pragma once

#include "PreprocessStepWidget.h"

#include <QSlider>

class BinaryThresholdPreprocessStepWidget : public PreprocessStepWidget
{
	Q_OBJECT

public:
	BinaryThresholdPreprocessStepWidget(QWidget *parent = nullptr);
	~BinaryThresholdPreprocessStepWidget();

private:
	QLabel *m_sliderText;
	QSlider *m_slider;
};
