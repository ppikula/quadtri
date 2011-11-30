#include <QtGui/QApplication>
#include "quadtri.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QuadTri w;
    w.show();

    return a.exec();
}
