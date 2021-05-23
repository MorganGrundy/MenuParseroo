#pragma once

#include "PreprocessStepWidget.h"

class MaskPreprocessStepWidget : public PreprocessStepWidget
{
	Q_OBJECT

public:
	MaskPreprocessStepWidget(QWidget *parent = nullptr);
	~MaskPreprocessStepWidget();

	//Applies the preprocess step to an image
	void preprocess(const cv::Mat &t_in, cv::Mat &t_out) override;

private:
	QPushButton *m_button;

	cv::Mat m_inputImageCache;
};
