#include "DrawingArea.h"
#include "DrawingScene.h"
#include "DrawingStep.h"

DrawingArea::DrawingArea(QWidget *parent) :
    QWidget(parent)
{
    basePolygon = new Polygon();
    scene = new DrawingScene(0, 0, 600, 600);
    scene->addItem(basePolygon);

    basePolygon->addBoundaryPoint(QPointF(100,200));
    basePolygon->addBoundaryPoint(QPointF(200,300));
    basePolygon->addBoundaryPoint(QPointF(300,300));

    basePolygon->addBoundaryPoint(QPointF(50,100));



    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setBackgroundBrush(QColor(0,0,0));

    startStopBut =new QPushButton();
    startStopBut->setText("Open");

    startStopInner = new QPushButton();
    startStopInner->setText("Add hole");


    groupBox = new QGroupBox();
    groupBox->setTitle("View");

    noneRadio= new QRadioButton();
    noneRadio->setText("Poly");

    quadrantRadio = new QRadioButton();
    quadrantRadio->setText("QuadTree");

    trianglesRadio = new QRadioButton();
    trianglesRadio->setText("Triangles");
    trianglesRadio->setChecked(true);



    QHBoxLayout *buttonsLay = new QHBoxLayout();
    //buttonsLay->addWidget(startStopBut);
   // buttonsLay->addWidget(startStopInner);

    QHBoxLayout *viewLay = new QHBoxLayout();
    viewLay->addWidget(noneRadio);
    viewLay->addWidget(quadrantRadio);
    viewLay->addWidget(trianglesRadio);
    groupBox->setLayout(viewLay);



    QVBoxLayout *bottomLay = new QVBoxLayout();
    bottomLay->addLayout(buttonsLay);

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(view);
    lay->addWidget(groupBox);
    lay->addWidget(startStopBut);
    setLayout(lay);

    connect(basePolygon,SIGNAL(polyChanged()),this,SLOT(polyUpdate()));
    connect(scene,SIGNAL(doubleClicked(QGraphicsSceneMouseEvent*)),this,SLOT(sceneDoubleCliked(QGraphicsSceneMouseEvent*)));
    connect(startStopBut,SIGNAL(clicked()),this,SLOT(startStopPoly()));
    connect(startStopInner,SIGNAL(clicked()),this,SLOT(startStopInnerPloly()));


    connect(noneRadio,SIGNAL(clicked()),this,SLOT(selectedViewsChanged()));
    connect(quadrantRadio,SIGNAL(clicked()),this,SLOT(selectedViewsChanged()));
    connect(trianglesRadio,SIGNAL(clicked()),this,SLOT(selectedViewsChanged()));

}

void DrawingArea::startStep(DrawingStep::StepType type){
    DrawingStep *step = new DrawingStep(type);
    step->setVisible(false);
    renderQueue.append(step);

}

void DrawingArea::stopStep(){
    scene->addItem(renderQueue.last());
    this->selectedViewsChanged();
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

void DrawingArea::selectedViewsChanged(){
    foreach(DrawingStep *step,renderQueue){
        switch(step->type){
        case DrawingStep::QUADRANT:
            step->setVisible(quadrantRadio->isChecked());
            break;
        case DrawingStep::TRIANGLE:
                step->setVisible(trianglesRadio->isChecked());
            break;
        }
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
}

void DrawingArea::clearAll(){
    startStopBut->setText("Open");
    startStopInner->setText("Add hole");
    scene->clear();

    basePolygon = new Polygon();
    holes.clear();
    renderQueue.clear();
}
