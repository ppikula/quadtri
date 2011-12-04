#ifndef TRIANGULATOR_H
#define TRIANGULATOR_H

#include <QtGui>
#include "Polygon.h"
#include "DrawingArea.h"

class Triangulator : public QObject
{
    Q_OBJECT
public:
    explicit Triangulator(DrawingArea *area,QObject *parent = 0);

signals:

public slots:
    void polyChanged(Polygon *poly, QList<Polygon*> holes);
    void triangulate();

private:
    DrawingArea *area;

};

#endif // TRIANGULATOR_H
