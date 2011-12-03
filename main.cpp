#include <QtGui/QApplication>
#include "quadtri.h"
#include "quadtree.h"
#include <cstdio>

void run_test()
{
    QuadTree tr(Point(0,0),800);

    printf("%u \n",tr.depth());

    printf("%u \n",tr.depth());

}


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QuadTri w;
    w.show();
    run_test();

    return a.exec();
}
