#pragma once
#include "UpDownButton.h"

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
};
