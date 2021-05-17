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

	//Create Up/Down button
	m_upDownButton = new UpDownButton(this);
	m_layout->addWidget(m_upDownButton);

	//Create label
	m_label = new QLabel("Test Label " + QString::number(id++), this);
	m_layout->addWidget(m_label);

	//Create delete button
	m_delButton = new QPushButton(this);
	m_delButton->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
	QIcon deleteIcon(":/gui/DeleteButton.png");
	deleteIcon.addFile(":/gui/DeleteButtonActive.png", QSize(), QIcon::Active, QIcon::State::Off);
	m_delButton->setIcon(deleteIcon);
	m_delButton->setIconSize(QSize(24, 24));
	m_layout->addWidget(m_delButton);

	//Connect signals for moving item
	connect(m_upDownButton, SIGNAL(up()),
		this, SIGNAL(moveUp()));
	connect(m_upDownButton, SIGNAL(down()),
		this, SIGNAL(moveDown()));

	//Connect signals for deleting item
	connect(m_delButton, SIGNAL(released()),
		this, SIGNAL(deleteReleased()));
}

PreprocessStepWidget::~PreprocessStepWidget()
{
	delete m_upDownButton;
	delete m_label;
	delete m_delButton;
	delete m_layout;
}
