#include "DrawingScene.h"

DrawingScene::DrawingScene(Polygon *basePolygon, qreal x, qreal y, qreal width, qreal height):
    QGraphicsScene(x,y,width,height),basePolygon(basePolygon)
{
    addItem(basePolygon);
    connect(basePolygon,SIGNAL(polyChanged()),this,SLOT(polyUpdate()));
    update();
}

void DrawingScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    emit doubleClicked(event);
}

void DrawingScene::polyUpdate(){
    qDebug()<<"polyUpdate from scene";
    emit polyChanged();
}


