#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class PreprocessStepWidget : public QWidget
{
	Q_OBJECT

public:
	PreprocessStepWidget(QWidget *parent);
	~PreprocessStepWidget();

private:
	QHBoxLayout *m_layout;
	QLabel *m_label;
	QPushButton *m_button;
};
