#include "drawingscene.h"

DrawingScene::DrawingScene(Polygon *basePolygon, qreal x, qreal y, qreal width, qreal height):
    QGraphicsScene(x,y,width,height),basePolygon(basePolygon)
{
    addItem(basePolygon);
}

void DrawingScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    if(itemAt(event->pos())==NULL && !basePolygon->isClosed()){
        qDebug() << "double click on scene: adding new poly dot";
        PolyDot *dot =new PolyDot();
        dot->setPos(event->pos());
        basePolygon->addBoundaryPoint(dot);
        basePolygon->update();
    }else
    {
        qDebug() << "double click on scene: cannot add new poly dot beacse sth is this place: "<<itemAt(event->pos());
    }
}
