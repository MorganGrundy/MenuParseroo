#ifndef MASKPAINTERDIALOG_H
#define MASKPAINTERDIALOG_H

#include <QDialog>
#include <QImage>
#include <QShowEvent>

namespace Ui
{
	class MaskPainterDialog;
}

class MaskPainterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MaskPainterDialog(const QImage &t_image, QWidget *parent = nullptr);
	~MaskPainterDialog();

	//Returns resulting image with mask
	QImage getImage();

protected:
	//Resizes graphics scene to fit in view
	void showEvent(QShowEvent *event) override;

private:
	Ui::MaskPainterDialog *ui;
};

#endif // MASKPAINTERDIALOG_H
