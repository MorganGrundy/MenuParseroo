#include "PreprocessStepWidget.h"

PreprocessStepWidget::PreprocessStepWidget(QWidget *parent)
	: QWidget(parent)
{
	//Create layout
	m_layout = new QHBoxLayout(this);

	//Create label and button
	m_label = new QLabel("Test Label", this);
	m_button = new QPushButton("Test Button", this);

	//Add to layout
	m_layout->addWidget(m_label);
	m_layout->addWidget(m_button);
}

PreprocessStepWidget::~PreprocessStepWidget()
{
	delete m_label;
	delete m_layout;
}
