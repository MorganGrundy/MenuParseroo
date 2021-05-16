#pragma once
#include "PreprocessStepWidget.h"

#include <QWidget>
#include <QVBoxLayout>

class PreprocessStepListWidget : public QWidget
{
	Q_OBJECT

public:
	PreprocessStepListWidget(QWidget *parent);
	~PreprocessStepListWidget();

public slots:
	//Creates a preprocessing step and adds it to list
	void addStep();

private:
	QVBoxLayout *m_layout;
	QList<PreprocessStepWidget *> m_steps;
};
