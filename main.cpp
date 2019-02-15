#include "ChumStreamer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChumStreamer w;
    w.show();

    return a.exec();
}
