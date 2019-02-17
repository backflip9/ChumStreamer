#include "chumstreamer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    chumstreamer w;
    w.show();

    return a.exec();
}
