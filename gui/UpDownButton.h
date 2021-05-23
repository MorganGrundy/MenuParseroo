#pragma once

#include <QWidget>
#include "ui_UpDownButton.h"

class UpDownButton : public QWidget
{
	Q_OBJECT

public:
	UpDownButton(QWidget *parent = Q_NULLPTR);
	~UpDownButton();

signals:
	void up();
	void down();

private:
	Ui::UpDownButton ui;
};
