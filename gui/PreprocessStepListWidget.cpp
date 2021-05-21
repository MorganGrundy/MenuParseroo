#include "PreprocessStepListWidget.h"

PreprocessStepListWidget::PreprocessStepListWidget(QWidget *parent)
	: QWidget(parent), m_steps{}
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setMargin(0);
}

PreprocessStepListWidget::~PreprocessStepListWidget()
{}

//Adds a preprocessing step to the list
void PreprocessStepListWidget::addStep(PreprocessStepWidget *step)
{
	if (step == nullptr)
		return;

	//No duplicates
	if (m_steps.contains(step))
		return;

	step->setParent(this);
	m_steps.append(step);
	m_layout->addWidget(step);

	//Connect signals for changing step order
	connect(step, &PreprocessStepWidget::moveUp,
		this, [=]() { moveStep(step, Direction::Up); });
	connect(step, &PreprocessStepWidget::moveDown,
		this, [=]() { moveStep(step, Direction::Down); });

	//Connect signal for deleting step
	connect(step, &PreprocessStepWidget::deleteReleased,
		this, [=]() { deleteStep(step); });

	//Connect signal for when step is modified
	connect(step, &PreprocessStepWidget::stepChanged,
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
	if (m_image.empty())
		return;

	cv::Mat tmp = m_image.clone();
	cv::Mat tmp2;
	for (auto step : m_steps)
	{
		step->preprocess(tmp, tmp2);
		tmp = tmp2;
	}

	emit imageUpdated(tmp);
}
