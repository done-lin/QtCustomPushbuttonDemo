#ifndef FORMTOUCH_H
#define FORMTOUCH_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QMouseEvent>
#include <QPaintEvent>

namespace Ui {
class FormTouch;
}

class FormTouch : public QWidget
{
    Q_OBJECT

public:
    explicit FormTouch(QWidget *parent = 0);
    ~FormTouch();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private slots:
    void on_pushButton_touch_clicked();

private:
    std::vector<std::vector<QPoint>> _lines;
    Ui::FormTouch *ui;
};

#endif // FORMTOUCH_H
