#include "PreprocessStepListWidget.h"

PreprocessStepListWidget::PreprocessStepListWidget(QWidget *parent)
	: QWidget(parent), m_steps{}
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setMargin(0);
}

PreprocessStepListWidget::~PreprocessStepListWidget()
{
	for (auto step : m_steps)
		delete step;
	m_steps.clear();

	delete m_layout;
}

//Creates a preprocessing step and adds it to list
void PreprocessStepListWidget::addStep()
{
	//Create step and add to layout
	PreprocessStepWidget *newStep = new PreprocessStepWidget(this);
	m_steps.append(newStep);
	m_layout->addWidget(newStep);
}