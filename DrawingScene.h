#ifndef DRAWINGSCENE_H
#define DRAWINGSCENE_H

#include <QtGui>
#include "Polygon.h"

class DrawingScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit DrawingScene(Polygon *basePolygon,qreal x,qreal y,qreal width,qreal height);

signals:
    void polyChanged();
    void doubleClicked(QGraphicsSceneMouseEvent *event);

public slots:
    void polyUpdate();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    Polygon *basePolygon;



};

#endif // DRAWINGSCENE_H
