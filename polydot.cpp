#include <QtGui>
#include "polydot.h"

PolyDot::PolyDot():dispRect(false),dispLabel(true)
{
    setCursor(Qt::OpenHandCursor);
    setAcceptedMouseButtons(Qt::LeftButton);
    acceptHoverEvents();
    setAcceptHoverEvents(true);
}

QPointF PolyDot::center(){
    return QPointF(pos().x() - boundingRect().width()/2,pos().y() -boundingRect().height()/2);
}

QRectF PolyDot::boundingRect() const
{
    return QRectF(-20, -20, 20, 20);
}

QRectF PolyDot::markerRect() const
{
    return QRectF(-20, -20, 20, 20);
}

void PolyDot::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::white);
    painter->drawEllipse(-12,-12,5,5);
    if(dispRect){
        painter->setPen(Qt::white);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(markerRect());
    }

    if(dispLabel){
        QString label;
        QString x,y;
        label += scenePos().x();
        label += ",";
        label += scenePos().y();
        //label += new QString()->setNum(this->scenePos().toPoint().x());
        label += ", ";
        label.setNum(this->scenePos().toPoint().y());

        painter->drawText(this->center(),label);
    }
}

void PolyDot::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    setCursor(Qt::ClosedHandCursor);
}

void PolyDot::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
            .length() < QApplication::startDragDistance()) {
        return;
    }

    setPos(this->pos() +event->pos());

    setCursor(Qt::OpenHandCursor);
    this->scene()->update();
}

void PolyDot::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    setCursor(Qt::OpenHandCursor);
    this->scene()->update();
}

void PolyDot::hoverEnterEvent(QGraphicsSceneHoverEvent *event){
    dispRect=true;

}

void PolyDot::hoverLeaveEvent(QGraphicsSceneHoverEvent *event){
    dispRect=false;
    this->scene()->update();
}

void PolyDot::hoverMoveEvent(QGraphicsSceneHoverEvent *event){

}
