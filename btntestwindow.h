#ifndef BTNTESTWINDOW_H
#define BTNTESTWINDOW_H

#include <QMainWindow>
#include "custompushbutton.h"

namespace Ui {
class BtnTestWindow;
}

class BtnTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BtnTestWindow(QWidget *parent = 0);
    ~BtnTestWindow();

private:
    CustomPushButton *m_pCustomPushButton;
    Ui::BtnTestWindow *ui;
};

#endif // BTNTESTWINDOW_H
