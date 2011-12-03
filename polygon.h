#ifndef POLYGON_H
#define POLYGON_H

#include <QtGui>
#include "polydot.h"

class Polygon : public QGraphicsItem
{
public:
    explicit Polygon(Polygon *basePolygon = 0);
     ~Polygon();

    QRectF boundingRect() const;
    void addBoundaryPoint(PolyDot *dot);

    //open/close poly for edition
    void close();
    void open();

    /* is closed for edition*/
    bool isClosed();

    /* is this inner poly of some other poly? */
    bool isInner();

    /*list of inner polygons which create holes in main poly*/
    QList<Polygon*> & holes();
signals:

public slots:

protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
//    QPainterPath shape() const;
private:
    Polygon *basePolygon;
    bool closed;

    QList<PolyDot*> boundary;
    QList<Polygon*> holesPolies;

};

#endif // POLYGON_H
