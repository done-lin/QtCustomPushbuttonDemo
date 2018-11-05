#include "btntestwindow.h"
#include "ui_btntestwindow.h"

BtnTestWindow::BtnTestWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BtnTestWindow)
{
    ui->setupUi(this);
    this->setMouseTracking(true);

    m_pCustomPushButton = new CustomPushButton(ui->centralWidget);
    m_pCustomPushButton->setGeometry(100, 50, 200, 50);
    m_pCustomPushButton->setParent(ui->centralWidget);
    m_pCustomPushButton->show();
}

BtnTestWindow::~BtnTestWindow()
{
    delete ui;
}
