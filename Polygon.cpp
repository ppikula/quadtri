#include "Polygon.h"

Polygon::Polygon(bool inner):QGraphicsItem(),inner(inner),closed(true){
}

Polygon::~Polygon(){
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
    return inner;
}

void Polygon::addBoundaryPoint(QPointF pos){
    PolyDot *dot =new PolyDot();
    dot->setPos(pos);
    dot->setParentItem(this);

    connect(dot,SIGNAL(dotMoved()),this,SLOT(dotUpdate()));

    boundary.append(dot);    
    emit polyChanged();
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

void Polygon::dotUpdate(){
    qDebug() << "polyChanged from polygon!";
    emit polyChanged();
}

QString Polygon::toString(){
    QString out;
    foreach (PolyDot *dot, boundary) {
        out += QString::number(dot->center().x());
        out += ";";
        out += QString::number(dot->center().y());
        out += "\n";
    }
    out.chop(1);
    return out;
}



