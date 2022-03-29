#include "mainwindow.h"

#include <QApplication>
#include <QFont>
#include "useKeyBoard_Demo/virtualkeyboard_hasQSQLITE/virtualkeyboard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font = a.font();

    auto keyboard = VirtualKeyBoard::getKeyboard();
    keyboard->setGeometry(300, 200, 432, 185);
    keyboard->init("");

    font.setPointSize(8);
    a.setFont(font);
    MainWindow w;
    w.show();
    keyboard->show();

    return a.exec();
}
