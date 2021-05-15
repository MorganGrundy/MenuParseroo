#include "PreprocessStepWidget.h"

PreprocessStepWidget::PreprocessStepWidget(QWidget *parent)
	: QWidget(parent)
{
	layout = new QHBoxLayout(this);
	setLayout(layout);

	nameLabel = new QLabel("Name");
	button = new QPushButton("Button");

	layout->addWidget(nameLabel);
	layout->addWidget(button);
}

PreprocessStepWidget::~PreprocessStepWidget()
{
	delete button;
	delete nameLabel;
	delete layout;
}

void PreprocessStepWidget::setName(const QString &name)
{
	nameLabel->setText(name);
}

QString PreprocessStepWidget::getName() const
{
	return nameLabel->text();
}
