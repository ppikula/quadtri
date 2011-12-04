#include "Triangulator.h"

Triangulator::Triangulator(DrawingArea *area,QObject *parent) :
    QObject(parent),area(area)
{
    connect(area,SIGNAL(polyChanged(Polygon*,QList<Polygon*>)),this,SLOT(polyChanged(Polygon*,QList<Polygon*>)));
}

void Triangulator::polyChanged(Polygon *poly, QList<Polygon *> holes){
    Q_UNUSED(poly)
    Q_UNUSED(holes)
    triangulate();//Remove this if you don't want to triangulate on every poly update
}

void Triangulator::triangulate(){

    Polygon *poly = area->polygon(); //main polygon
    QList<Polygon*> holes = area->holesInPoly(); //holes in it

    area->clear(); //remove previous drawings

    for(int i=0;i<50;i++){
        area->startStep(0.1); //begins drawing step
        for(int j=1;j<qrand()%10;j++){
            qreal ax = 100+qrand()%300;
            qreal ay = 100+qrand()%300;
            qreal bx = 100+qrand()%300;
            qreal by = 100+qrand()%300;
            QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(ax,ay,bx,by));
            QPen pen(Qt::yellow);
            line->setPen(pen);
            area->addToQueue(line); //adds figure to the step
        }
        area->stopStep(); //ends drawing step
    }
}
