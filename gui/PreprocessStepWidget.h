#pragma once
#include "UpDownButton.h"
#include "PreprocessStep.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class PreprocessStepWidget : public QFrame
{
	Q_OBJECT

public:
	PreprocessStepWidget(QWidget *parent);
	~PreprocessStepWidget();

	void loadStepData();

signals:
	void moveUp();
	void moveDown();

	void deleteReleased();

private:
	QHBoxLayout *m_layout;
	UpDownButton *m_upDownButton;
	QLabel *m_label;
	QPushButton *m_delButton;

	static int id;

	PreprocessStep *m_step;
};
