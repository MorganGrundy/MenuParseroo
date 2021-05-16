#include "PreprocessStepWidget.h"

PreprocessStepWidget::PreprocessStepWidget(QWidget *parent)
	: QFrame(parent)
{
	//Set style
	setFrameStyle(QFrame::Panel | QFrame::Raised);
	setStyleSheet("PreprocessStepWidget {"
		"background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0, stop:0 rgba(200, 200, 200, 100), "
		"stop:1 rgba(255, 255, 255, 100)); }");

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
