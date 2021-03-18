#ifndef MASKPAINTER_H
#define MASKPAINTER_H

#include <QWidget>
#include <QPixmap>
#include <QBitmap>
#include <QGraphicsPixmapItem>

namespace Ui {
class MaskPainter;
}

class MaskPainter : public QWidget
{
    Q_OBJECT

public:
    explicit MaskPainter(QWidget *parent = nullptr);
    ~MaskPainter();

    //Sets the image
    void setImage(const QImage &t_image, const bool keepMask = false);

    //Returns mask
    QImage getMask();

private:
    Ui::MaskPainter *ui;
};

#endif // MASKPAINTER_H
