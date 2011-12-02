#include "drawingscene.h"

DrawingScene::DrawingScene(Polygon *basePolygon, qreal x, qreal y, qreal width, qreal height):
    QGraphicsScene(x,y,width,height),basePolygon(basePolygon)
{
    addItem(basePolygon);
    update();
}

void DrawingScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    qDebug() << "pos "<< event->pos()<<", scenePos: " << event->scenePos();
    if(itemAt(event->pos())==NULL && !basePolygon->isClosed()){
        qDebug() << "double click on scene: adding new poly dot: " << event->scenePos().rx();
        PolyDot *dot =new PolyDot();

        dot->setPos(event->scenePos());
        basePolygon->addBoundaryPoint(dot);

        update();
    }else
    {
        qDebug() << "double click on scene: cannot add new poly dot beacse sth is this place: "<<itemAt(event->pos());
    }
}
