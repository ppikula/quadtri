#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QQueue>
#include <QDebug>
#include "drawingarea.h"
#include "polydot.h"
#include "polygon.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DrawingArea *area = new DrawingArea();
    area->show();

//    Polygon *poly = new Polygon();
//    PolyDot *dot = new PolyDot();
//    PolyDot *dot2 = new PolyDot();
//    PolyDot *dot3 = new PolyDot();

//    dot->setPos(50,50);
//    dot2->setPos(100,140);
//    dot3->setPos(30,78);

//    poly->setPos(10,10);
//    poly->addBoundaryPoint(dot);
//    poly->addBoundaryPoint(dot2);
//    poly->addBoundaryPoint(dot3);
//    poly->close();

//    area->scene->addItem(poly);
//    area->startStep(0.1);
//    area->addToQueue(dot);
//    area->stopStep();


    return a.exec();
}
