#include "PreprocessStepWidget.h"
#include "BinaryThresholdPreprocessStep.h"

PreprocessStepWidget::PreprocessStepWidget(QWidget *parent)
	: QFrame(parent), m_step{ nullptr }
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
	m_nameLabel = new QLabel(this);
	m_nameLabel->setText("N/A");
	m_layout->addWidget(m_nameLabel);

	//Create spacer item
	m_layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Policy::Expanding));

	//Create delete button
	m_delButton = new QPushButton(this);
	m_delButton->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
	QIcon deleteIcon(":/gui/DeleteButton.png");
	deleteIcon.addFile(":/gui/DeleteButtonActive.png", QSize(), QIcon::Active, QIcon::State::Off);
	m_delButton->setIcon(deleteIcon);
	m_delButton->setIconSize(QSize(24, 24));
	m_delButton->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
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
{}

//Applies the preprocess step to an image
void PreprocessStepWidget::preprocess(const cv::Mat &t_in, cv::Mat &t_out)
{
	if (m_step == nullptr)
		t_out = t_in.clone();
	else
		m_step->preprocess(t_in, t_out);
}