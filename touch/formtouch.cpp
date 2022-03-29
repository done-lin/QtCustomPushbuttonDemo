#include "formtouch.h"
#include "ui_formtouch.h"

FormTouch::FormTouch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTouch)
{
    ui->setupUi(this);
}

FormTouch::~FormTouch()
{
    delete ui;
}

void FormTouch::mousePressEvent(QMouseEvent *event)
{
    std::vector<QPoint> line;
    line.push_back(event->pos());
    _lines.push_back(line);
}

void FormTouch::mouseMoveEvent(QMouseEvent *event)
{
    auto &line = _lines[_lines.size() - 1];
    line.push_back(event->pos());

    update(); //更新，重新绘制窗口，自动调用paintEvent
}

void FormTouch::mouseReleaseEvent(QMouseEvent *event)
{
    auto &line = _lines[_lines.size() - 1];
    line.push_back(event->pos());
}

void FormTouch::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::red, 3));
    for (const auto &line : _lines)
    {
        for (int i=0;i<line.size()-1;i++)
        {
            painter.drawLine(line.at(i), line.at(i + 1));
        }
    }
}

void FormTouch::on_pushButton_touch_clicked()
{
    this->close();
    this->deleteLater();
}
