#include "polygon.h"

Polygon::Polygon(QObject *parent):closed(false)
{
    PolyDot *firstdot = new PolyDot();
    PolyDot *secondDot = new PolyDot();
    PolyDot *thirdDot = new PolyDot();
    firstdot->setPos(50,50);
    secondDot->setPos(44,30);
    thirdDot->setPos(77,33);
    addBoundaryPoint(firstdot);
    addBoundaryPoint(secondDot);
    addBoundaryPoint(thirdDot);
}

void Polygon::open(){
    if(closed){
        closed = false;
    }else
    {
        qDebug()<<"it is already opened!";
    }
}

void Polygon::close(){
    if(!closed){
        closed = true;
    }else
    {
        qDebug()<<"it is already closed!";
    }
}

QRectF Polygon::boundingRect() const
{
    qreal min_x=100000,min_y=100000,max_x=-1000000,max_y=-1000000;
    QList<PolyDot* >::const_iterator it;

    for(it=boundary.constBegin();it!=boundary.constEnd();it++){
        if((*it)->pos().x()<min_x)
            min_x = (*it)->pos().x();
        if((*it)->pos().y()<min_y)
            min_y = (*it)->pos().y();
        if((*it)->pos().x()>max_x)
            max_x = (*it)->pos().x();
        if((*it)->pos().y()>max_y)
            max_y = (*it)->pos().y();
    }
    QRectF rect(min_x,max_y,max_x-min_x,max_y-min_y);
}

//QPainterPath Polygon::shape()const{
//    QPainterPath path;
//    QPolygonF polyF(boundary.size());
//    QList<PolyDot*>::const_iterator it;
//    for(it=boundary.begin();it!=boundary.end();it++){
//        polyF.append((*it)->pos());
//    }
//    path.addPolygon(polyF);
//    return path;
//}

bool Polygon::isClosed(){
    return closed;
}


void Polygon::addBoundaryPoint(PolyDot *dot){
    dot->setParentItem(this);
    boundary.append(dot);
}

void Polygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QList<PolyDot*>::iterator it;
    PolyDot *prev = *boundary.begin();
    for(it = boundary.begin()+1;it!=boundary.end();++it){
        painter->setPen(Qt::blue);
        painter->drawLine(prev->center(),(*it)->center());
        prev = *it;
    }
    if(closed){
        painter->setPen(Qt::blue);
        painter->drawLine(boundary.last()->center(),boundary.first()->center());
    }
}

