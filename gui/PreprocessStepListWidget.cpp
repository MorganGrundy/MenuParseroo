#include "PreprocessStepListWidget.h"
#include "BinaryThresholdPreprocessStepWidget.h"

PreprocessStepListWidget::PreprocessStepListWidget(QWidget *parent)
	: QWidget(parent), m_steps{}
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setMargin(0);
}

PreprocessStepListWidget::~PreprocessStepListWidget()
{}

//Creates a preprocessing step and adds it to list
void PreprocessStepListWidget::addStep()
{
	//Create step and add to layout
	PreprocessStepWidget *newStep = new BinaryThresholdPreprocessStepWidget(this);
	m_steps.append(newStep);
	m_layout->addWidget(newStep);

	//Connect signals for changing step order
	connect(newStep, &PreprocessStepWidget::moveUp,
		this, [=]() { moveStep(newStep, Direction::Up); });
	connect(newStep, &PreprocessStepWidget::moveDown,
		this, [=]() { moveStep(newStep, Direction::Down); });

	//Connect signal for deleting step
	connect(newStep, &PreprocessStepWidget::deleteReleased,
		this, [=]() { deleteStep(newStep); });

	//Connect signal for when step is modified
	connect(newStep, &PreprocessStepWidget::stepChanged,
		this, [=]() { preprocess(); });

	preprocess();
}

//Moves step in direction
void PreprocessStepListWidget::moveStep(PreprocessStepWidget *step, const Direction dir)
{
	//Get current and new index of step
	int index = m_steps.indexOf(step);
	int newIndex = std::clamp(index + static_cast<int>(dir), 0, m_steps.size() - 1);

	//If index is unchanged then skip
	if (newIndex == index)
		return;

	m_steps.move(index, newIndex);

	//Move step in layout
	m_layout->removeWidget(step);
	m_layout->insertWidget(newIndex, step);

	preprocess();
}

//Deletes step
void PreprocessStepListWidget::deleteStep(PreprocessStepWidget *step)
{
	m_steps.removeOne(step);
	m_layout->removeWidget(step);
	delete step;

	preprocess();
}

void PreprocessStepListWidget::setImage(const cv::Mat &t_in)
{
	m_image = t_in;
	preprocess();
}

//Performs all preprocess steps on current image
void PreprocessStepListWidget::preprocess()
{
	cv::Mat tmp = m_image.clone();
	for (auto step : m_steps)
		step->preprocess(tmp, tmp);

	emit imageUpdated(tmp);
}
