#pragma once

#include "PreprocessStepWidget.h"

//Assumes image is either BGRA, BGR, or GRAY
class GrayscalePreprocessStepWidget : public PreprocessStepWidget
{
	Q_OBJECT

public:
	GrayscalePreprocessStepWidget(QWidget *parent = nullptr);
	~GrayscalePreprocessStepWidget();
};
