#include "polygon.h"

Polygon::Polygon(Polygon *basePolygon):QGraphicsItem(basePolygon),basePolygon(basePolygon),closed(true){
    if(basePolygon==NULL){
        holesPolies.append(new Polygon(this));
    }
}


Polygon::~Polygon(){
    QList<Polygon* >::iterator it2;
    for(it2=holes().begin();it2!=holes().end();it2++){
        delete *it2;
    }
    QList<PolyDot* >::iterator it;
    for(it=boundary.end();it!=boundary.end();it++){
        delete *it;
    }
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
    return rect;
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

bool Polygon::isInner(){
    return basePolygon != NULL;
}

QList<Polygon*> & Polygon::holes(){
    return holesPolies;
}

void Polygon::addBoundaryPoint(PolyDot *dot){
    dot->setParentItem(this);
    boundary.append(dot);
}

void Polygon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QList<PolyDot*>::iterator it;
    PolyDot *prev = *boundary.begin();
    QPolygonF polygon;

    for(it = boundary.begin();it!=boundary.end();++it){
        painter->drawLine(prev->center(),(*it)->center());
        polygon << (*it)->center();
    }

    if(!isInner()){
        painter->setPen(Qt::blue);
        painter->setBrush(Qt::green);
    }else{
        painter->setPen(Qt::blue);
        painter->setBrush(Qt::black);
    }

    if(isClosed() || isInner()){
        painter->drawPolygon(polygon);
    }else
    {
        painter->drawPolyline(polygon);
    }
}



