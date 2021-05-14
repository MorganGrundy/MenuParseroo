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

	void setName(const QString &name);
	QString getName() const;

private:
	QHBoxLayout *layout;
	QLabel *nameLabel;
	QPushButton *button;
};
