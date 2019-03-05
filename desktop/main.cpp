#include "chumstreamer_desktop.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    chumstreamer_desktop w;
    w.show();

    return a.exec();
}
