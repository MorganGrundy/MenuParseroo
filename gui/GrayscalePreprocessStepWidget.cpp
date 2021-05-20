#include "GrayscalePreprocessStepWidget.h"
#include "GrayscalePreprocessStep.h"

GrayscalePreprocessStepWidget::GrayscalePreprocessStepWidget(QWidget *parent)
	: PreprocessStepWidget(parent)
{
	//Create step
	m_step = new GrayscalePreprocessStep();

	//Set name label
	m_nameLabel->setText(QString::fromStdString(m_step->getName()));
}

GrayscalePreprocessStepWidget::~GrayscalePreprocessStepWidget()
{
}
