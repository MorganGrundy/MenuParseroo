#include "PreprocessStepWidget.h"

int PreprocessStepWidget::id = 0;

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
	m_layout->setMargin(0);

	//Create Up/Down button and label
	m_upDownButton = new UpDownButton(this);
	m_label = new QLabel("Test Label " + QString::number(id++), this);

	//Add to layout
	m_layout->addWidget(m_upDownButton);
	m_layout->addWidget(m_label);

	//Emit signals for moving item
	connect(m_upDownButton, SIGNAL(up()),
		this, SIGNAL(moveUp()));
	connect(m_upDownButton, SIGNAL(down()),
		this, SIGNAL(moveDown()));
}

PreprocessStepWidget::~PreprocessStepWidget()
{
	delete m_upDownButton;
	delete m_label;
	delete m_layout;
}
