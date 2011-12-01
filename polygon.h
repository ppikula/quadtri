#ifndef POLYGON_H
#define POLYGON_H

#include <QtGui>
#include "polydot.h"

class Polygon : public QGraphicsItem
{
public:
    explicit Polygon(QObject *parent = 0);
    QRectF boundingRect() const;
    void addBoundaryPoint(PolyDot *dot);
    void close();
    void open();
    bool isClosed();
signals:

public slots:

protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;
private:
    bool closed;

    QList<PolyDot*> boundary;
    QList<QList<PolyDot*> > innerBoundaries;

};

#endif // POLYGON_H
