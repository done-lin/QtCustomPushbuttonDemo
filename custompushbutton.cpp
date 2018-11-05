#include "custompushbutton.h"
#include <QPushButton>
#include <QMouseEvent>


CustomPushButton::CustomPushButton(QWidget *parent)
{
    qDebug("%s[%d],  custom push button construct!", __FUNCTION__, __LINE__);
}

void CustomPushButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        qDebug("%s[%d], get mouse left btn released!", __FUNCTION__, __LINE__);
    }else{
        qDebug("%s[%d], ", __FUNCTION__, __LINE__);
        QPushButton::mouseReleaseEvent(e);
    }
}

void CustomPushButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        qDebug("%s[%d], get mouse left btn press!", __FUNCTION__, __LINE__);
    }else{
        qDebug("%s[%d], ", __FUNCTION__, __LINE__);
        QPushButton::mousePressEvent(e);
    }
}

void CustomPushButton::mouseMoveEvent(QMouseEvent *e)
{
    int x=e->x();
    int y=e->y();
    qDebug("%s[%d], X:%d,   Y:%d ", __FUNCTION__, __LINE__, x, y);
}
