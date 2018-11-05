#include "btntestwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BtnTestWindow w;
    w.show();

    return a.exec();
}
