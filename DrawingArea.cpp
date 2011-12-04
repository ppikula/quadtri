#include "DrawingArea.h"
#include "DrawingScene.h"
#include "DrawingStep.h"

DrawingArea::DrawingArea(QWidget *parent) :
    QWidget(parent)
{
    basePolygon = new Polygon();
    scene = new DrawingScene(0, 0, 600, 600);
    scene->addItem(basePolygon);

    basePolygon->addBoundaryPoint(QPointF(100,100));
    basePolygon->addBoundaryPoint(QPointF(200,200));
    basePolygon->addBoundaryPoint(QPointF(100,200));

    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setBackgroundBrush(QColor(0,0,0));


    startStopBut =new QPushButton();
    startStopBut->setText("Open");

    startStopInner = new QPushButton();
    startStopInner->setText("Add hole");

    slider = new QSlider();
    slider->setOrientation(Qt::Horizontal);

    slider->setMinimum(0);
    slider->setMaximum(0);


    QHBoxLayout *buttonsLay = new QHBoxLayout();
    buttonsLay->addWidget(startStopBut);
    buttonsLay->addWidget(startStopInner);

    QVBoxLayout *bottomLay = new QVBoxLayout();
    bottomLay->addWidget(slider);
    bottomLay->addLayout(buttonsLay);

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(view);
    lay->addLayout(bottomLay);
    setLayout(lay);

    connect(basePolygon,SIGNAL(polyChanged()),this,SLOT(polyUpdate()));
    connect(scene,SIGNAL(doubleClicked(QGraphicsSceneMouseEvent*)),this,SLOT(sceneDoubleCliked(QGraphicsSceneMouseEvent*)));
    connect(startStopBut,SIGNAL(clicked()),this,SLOT(startStopPoly()));
    connect(startStopInner,SIGNAL(clicked()),this,SLOT(startStopInnerPloly()));
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(viewSliderChanged(int)));

}

void DrawingArea::startStep(float time){
    renderQueue.append(new DrawingStep(time));
}

void DrawingArea::stopStep(){
    scene->addItem(renderQueue.last());
    slider->setMaximum(slider->maximum()+1);
    slider->setValue(slider->maximum());
    scene->update();
}

void DrawingArea::addToQueue(QGraphicsItem *item){
    renderQueue.last()->addToGroup(item);
}

/* Slots */
void DrawingArea::polyUpdate(){
    emit polyChanged(basePolygon,holes);
}


/* Callbacks */
void DrawingArea::startStopPoly(){
    if(basePolygon->isClosed()){
        basePolygon->open();
        startStopBut->setText("Close");
        startStopInner->setEnabled(false);
    }else{
        basePolygon->close();
        startStopBut->setText("Open");
        startStopInner->setEnabled(true);
    }
    scene->update();
}

void DrawingArea::startStopInnerPloly(){
    if(holes.size()==0 || holes.last()->isClosed()){
        Polygon *newPoly = new Polygon(true);
        newPoly->open();
        connect(newPoly,SIGNAL(polyChanged()),this,SLOT(polyUpdate()));
        scene->addItem(newPoly);
        holes.append(newPoly);

        startStopInner->setText("Close hole");
        startStopBut->setEnabled(false);
    }else{
        holes.last()->close();

        startStopInner->setText("Add hole");
        startStopBut->setEnabled(true);
    }
    scene->update();
}

void DrawingArea::viewSliderChanged(int v){
    for(int i=0;i<renderQueue.size();++i){
        renderQueue[i]->setVisible(false);
    }
    for(int i=0;i<v;++i){
        renderQueue[i]->setVisible(true);
    }
    scene->update();
}

void DrawingArea::sceneDoubleCliked(QGraphicsSceneMouseEvent *event){
    if(!basePolygon->isClosed()){
        basePolygon->addBoundaryPoint(event->scenePos());
    }else {
        Polygon *poly = holes.last();
        if(!poly->isClosed()){
            poly->addBoundaryPoint(event->scenePos());
        }
    }
    scene->update();
}

QString DrawingArea::polyString(){
    QString out = basePolygon->toString();

    out +="\n\n";
    foreach(Polygon *hole,holes){
        out +=hole->toString();
        out +="\n\n";
    }
    out.chop(2);
    return out;
}

void DrawingArea::clear(){
    foreach(DrawingStep *step,renderQueue){
        scene->removeItem(step);
        delete step;
    }
    renderQueue.clear();
    slider->setMaximum(0);
}

void DrawingArea::clearAll(){
    startStopBut->setText("Open");
    startStopInner->setText("Add hole");
    scene->clear();

    basePolygon = new Polygon();
    holes.clear();
    renderQueue.clear();
    slider->setMaximum(0);
}
