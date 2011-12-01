#ifndef POLYDOT_H
#define POLYDOT_H

#include <QtGui>

class PolyDot : public QGraphicsItem
{
public:
    explicit PolyDot();
    QRectF boundingRect() const;
    QRectF markerRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPointF center();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

private:
    bool dispRect;
    QGraphicsItem *parentPolygon;

};

#endif // POLYDOT_H
