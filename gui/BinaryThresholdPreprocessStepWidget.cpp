#include "BinaryThresholdPreprocessStepWidget.h"
#include "BinaryThresholdPreprocessStep.h"

BinaryThresholdPreprocessStepWidget::BinaryThresholdPreprocessStepWidget(QWidget *parent)
	: PreprocessStepWidget(parent)
{
	//Create step
	m_step = new BinaryThresholdPreprocessStep();

	//Set name label
	m_nameLabel->setText(QString::fromStdString(m_step->getName()));

	//Create slider
	m_slider = new QSlider(Qt::Orientation::Horizontal, this);
	m_slider->setRange(0, 255);
	m_slider->setValue(static_cast<BinaryThresholdPreprocessStep *>(m_step)->getThreshold());
	m_layout->insertWidget(3, m_slider);

	//Create slider text
	m_sliderText = new QLabel(this);
	m_sliderText->setText(QString("%1/%2").arg(m_slider->value()).arg(255));
	m_layout->insertWidget(3, m_sliderText);

	//When slider changes update the label and emit signal that the step changed
	connect(m_slider, &QSlider::valueChanged,
		this, [=](const int t_value) {
			static_cast<BinaryThresholdPreprocessStep *>(m_step)->setThreshold(t_value);
			m_sliderText->setText(QString("%1/%2").arg(m_slider->value()).arg(255));
			emit stepChanged();
		});
}

BinaryThresholdPreprocessStepWidget::~BinaryThresholdPreprocessStepWidget()
{}