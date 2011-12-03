#include "drawingarea.h"
#include "drawingscene.h"
#include "drawingstep.h"

DrawingArea::DrawingArea(QWidget *parent) :
    QWidget(parent)//,currentStep(NULL)
{
    basePolygon = new Polygon();
    scene = new DrawingScene(basePolygon,0, 0, 600, 600);

//    PolyDot *firstdot = new PolyDot();
//    PolyDot *secondDot = new PolyDot();
//    PolyDot *thirdDot = new PolyDot();

//    firstdot->setPos(100,250);
//    secondDot->setPos(200,60);
//    thirdDot->setPos(434,344);

//    basePolygon->addBoundaryPoint(firstdot);
//    basePolygon->addBoundaryPoint(secondDot);
//    basePolygon->addBoundaryPoint(thirdDot);

    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setBackgroundBrush(QColor(0,0,0));

    startStopBut =new QPushButton();
    startStopBut->setText("Open");

    startStopInner = new QPushButton();
    startStopInner->setText("Open Inner");

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
    qDebug() << "adding Graphics object\n";
    renderQueue.last()->addToGroup(item);
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
    if(basePolygon->holes().last()->isClosed()){
        basePolygon->holes().last()->open();
        startStopInner->setText("Close hole");
        startStopBut->setEnabled(false);
    }else
    { basePolygon->holes().last()->close();
        Polygon *newPoly = new Polygon(basePolygon);
        basePolygon->holes().append(newPoly);
        startStopInner->setText("Open hole");
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
}

Polygon * DrawingArea::polygon(){
    return basePolygon;
}
