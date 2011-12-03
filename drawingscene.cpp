#include "drawingscene.h"

DrawingScene::DrawingScene(Polygon *basePolygon, qreal x, qreal y, qreal width, qreal height):
    QGraphicsScene(x,y,width,height),basePolygon(basePolygon)
{
    addItem(basePolygon);
    update();
}

void DrawingScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){


    if(!basePolygon->isClosed()){
        PolyDot *dot =new PolyDot();
        dot->setPos(event->scenePos());
        basePolygon->addBoundaryPoint(dot);

        update();

        qDebug() << "double click on scene: adding new poly dot: " << event->scenePos().rx();
    }else {
        Polygon *poly = basePolygon->holes().last();
        if(!poly->isClosed()){
            PolyDot *dot =new PolyDot();
            dot->setPos(event->scenePos());
            poly->addBoundaryPoint(dot);
        }
    }
    update();
}
