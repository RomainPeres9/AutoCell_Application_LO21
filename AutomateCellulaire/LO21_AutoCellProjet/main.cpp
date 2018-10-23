#include "autocell.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutoCell w;
    w.show();

    return a.exec();
}
