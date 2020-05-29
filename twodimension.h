#ifndef TWODIMENSION_H
#define TWODIMENSION_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

class HeatMapper;
class GradientPalette;

class TwoDimension: public QWidget
{
public:
    enum {
        CANVAS_WIDTH = 1000,
        CANVAS_HEIGHT = 700,
        DEFAULT_RADIUS = 60,
        DEFAULT_OPACITY = 128,
        DEFAULT_WIDTH = 255
    };

    explicit TwoDimension(QWidget *parent = 0);
    virtual ~TwoDimension();

protected:
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    // 绘图对象指针
    HeatMapper *mapper_;
    // 用于显示输出的图像
    QImage *canvas_;
    // 调色板
    GradientPalette *palette_;

    QWidget *dockTwoView;
};

#endif // TWODIMENSION_H
