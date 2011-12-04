#include "DrawingScene.h"

DrawingScene::DrawingScene( qreal x, qreal y, qreal width, qreal height):
    QGraphicsScene(x,y,width,height)
{

}

void DrawingScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
    emit doubleClicked(event);
}


