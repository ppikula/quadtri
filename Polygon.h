#ifndef POLYGON_H
#define POLYGON_H

#include <QtGui>
#include "PolyDot.h"

class Polygon : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
     Polygon(bool inner=false);
     ~Polygon();

    QRectF boundingRect() const;
    void addBoundaryPoint(QPointF pos);

    //open/close poly for edition
    void close();
    void open();

    /* is closed for edition*/
    bool isClosed();

    /* is this inner poly of some other poly? */
    bool isInner();

    QString toString();

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

    QList<PolyDot*> boundary;

};

#endif // POLYGON_H
