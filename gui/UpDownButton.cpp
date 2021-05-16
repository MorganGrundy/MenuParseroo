#include "UpDownButton.h"

UpDownButton::UpDownButton(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//Set icon for up button
	QIcon upArrow(":/gui/UpArrow.png");
	upArrow.addFile(":/gui/UpArrowActive.png", QSize(), QIcon::Active, QIcon::State::Off);
	ui.upButton->setIcon(upArrow);

	//Set icon for down button
	QIcon downArrow(":/gui/DownArrow.png");
	downArrow.addFile(":/gui/DownArrowActive.png", QSize(), QIcon::Active, QIcon::State::Off);
	ui.downButton->setIcon(downArrow);

	//Emit signals on button press
	connect(ui.upButton, &QPushButton::released,
		this, [=]() { emit up(); });
	connect(ui.downButton, &QPushButton::released,
		this, [=]() { emit down(); });
}

UpDownButton::~UpDownButton()
{
}
