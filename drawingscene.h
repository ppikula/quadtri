#ifndef DRAWINGSCENE_H
#define DRAWINGSCENE_H

#include <QtGui>
#include "polygon.h"

class DrawingScene : public QGraphicsScene
{
public:
    explicit DrawingScene(Polygon *basePolygon,qreal x,qreal y,qreal width,qreal height);

signals:

public slots:
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    Polygon *basePolygon;

};

#endif // DRAWINGSCENE_H
