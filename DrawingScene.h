#ifndef DRAWINGSCENE_H
#define DRAWINGSCENE_H

#include <QtGui>
#include "Polygon.h"

class DrawingScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit DrawingScene(qreal x,qreal y,qreal width,qreal height);

signals:
    void doubleClicked(QGraphicsSceneMouseEvent *event);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

};

#endif // DRAWINGSCENE_H
