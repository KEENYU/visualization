#include "twodimension.h"
#include <QPainter>
#include <QRadialGradient>
#include <QImage>
#include <QMouseEvent>
#include <QDebug>
#include "heatmapper.h"
#include "gradientpalette.h"
#include <QDockWidget>
#include <QTreeWidget>
#include <QTextEdit>


TwoDimension::TwoDimension(QWidget *parent)
    : QWidget(parent)
{
    palette_ = new GradientPalette(DEFAULT_WIDTH);  //渐变色板宽度

    palette_->setColorAt(0.45, Qt::blue);  //用指定颜色在指定位置创建一个停止点
    palette_->setColorAt(0.55, Qt::cyan);
    palette_->setColorAt(0.65, Qt::green);
    palette_->setColorAt(0.85, Qt::yellow);
    palette_->setColorAt(1.0, Qt::red);

    canvas_ = new QImage(CANVAS_WIDTH, CANVAS_HEIGHT, QImage::Format_ARGB32);  //画布
    canvas_->fill(QColor(0, 0, 0, 0));  //图像填充
    mapper_ = new HeatMapper(canvas_, palette_, DEFAULT_RADIUS, DEFAULT_OPACITY);

    //this->setFixedSize(CANVAS_WIDTH, CANVAS_HEIGHT);  //设置控件宽高

}
TwoDimension::~TwoDimension()
{
    delete mapper_;
    delete palette_;
}

/*
 * 绘图方法，当窗口刷新时该方法会被调用
 */
void TwoDimension::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, *canvas_);
}

/*
 * 该方法用于向绘图对象添加一个数据点
 */
void TwoDimension::mouseReleaseEvent(QMouseEvent *e)
{
    if (NULL == mapper_)
        return;

    mapper_->addPoint(e->x(), e->y());
    this->update();  //调度paintEvent更新
}
