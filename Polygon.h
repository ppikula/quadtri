#ifndef POLYGON_H
#define POLYGON_H

#include <QtGui>
#include "PolyDot.h"

class Polygon : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
     Polygon(bool inner=false);
     ~Polygon();

    QRectF boundingRect() const;
    void addBoundaryPoint(QPointF pos);

    //open/close poly for edition
    void close() { closed = true; }
    void open() { closed = false;}

    /* is closed for edition*/
    bool isClosed() { return closed;}

    /* is this inner poly of some other poly? */
    bool isInner() {return inner;}

    QString toString();
    QList<PolyDot*> boundary;
signals:
    void polyChanged();

public slots:
    void dotUpdate();

protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    Polygon *basePolygon;
    bool closed;
    bool inner;



};

#endif // POLYGON_H
