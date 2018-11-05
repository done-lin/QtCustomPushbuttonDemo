#include "btntestwindow.h"
#include "ui_btntestwindow.h"

BtnTestWindow::BtnTestWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BtnTestWindow)
{
    ui->setupUi(this);
}

BtnTestWindow::~BtnTestWindow()
{
    delete ui;
}
