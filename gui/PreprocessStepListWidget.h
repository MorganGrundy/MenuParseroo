#pragma once
#include "PreprocessStepWidget.h"

#include <QWidget>
#include <QVBoxLayout>

class PreprocessStepListWidget : public QWidget
{
	Q_OBJECT

public:
	enum class Direction { Up = -1, Down = 1 };

	PreprocessStepListWidget(QWidget *parent);
	~PreprocessStepListWidget();

public slots:
	//Creates a preprocessing step and adds it to list
	void addStep();

	//Moves step in direction
	void moveStep(PreprocessStepWidget *step, const Direction dir);

private:
	QVBoxLayout *m_layout;
	QList<PreprocessStepWidget *> m_steps;
};