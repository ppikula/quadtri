#include "drawingarea.h"
#include "drawingscene.h"

DrawingArea::DrawingArea(QWidget *parent) :
    QWidget(parent),currentStep(NULL)
{
    basePolygon = new Polygon();
    scene = new DrawingScene(basePolygon,0, 0, 600, 600);

    PolyDot *firstdot = new PolyDot();
    PolyDot *secondDot = new PolyDot();
    PolyDot *thirdDot = new PolyDot();

    firstdot->setPos(50,50);
    secondDot->setPos(44,30);
    thirdDot->setPos(77,33);

    basePolygon->addBoundaryPoint(firstdot);
    basePolygon->addBoundaryPoint(secondDot);
    basePolygon->addBoundaryPoint(thirdDot);

    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setBackgroundBrush(QColor(0,0,0));

    startStopBut =new QPushButton();
    startStopBut->setText("Open");

    startStopInner = new QPushButton();
    startStopInner->setText("Open Inner");
    //startStopInner->setDisabled(true);

    triangulateBut = new QPushButton();
    triangulateBut->setText("Triangualte");

    slider = new QSlider();
    slider->setOrientation(Qt::Horizontal);

    QHBoxLayout *buttonsLay = new QHBoxLayout();
    buttonsLay->addWidget(startStopBut);
    buttonsLay->addWidget(startStopInner);
    buttonsLay->addWidget(triangulateBut);

    QVBoxLayout *bottomLay = new QVBoxLayout();
    bottomLay->addWidget(slider);
    bottomLay->addLayout(buttonsLay);

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(view);
    lay->addLayout(bottomLay);
    setLayout(lay);

    connect(startStopBut,SIGNAL(clicked()),this,SLOT(startStopPoly()));
    connect(triangulateBut,SIGNAL(clicked()),this,SLOT(triangulate()));
    connect(startStopInner,SIGNAL(clicked()),this,SLOT(startStopInnerPloly()));
}
void DrawingArea::startStep(float time){
    if (currentStep != NULL){
        throw "step didn't finished!";
    }else
    {
        currentStep = new GraphicsStep(time);
    }
}

void DrawingArea::stopStep(){
    qDebug() << "stop Step\n";
    renderQueue.enqueue(currentStep);
    currentStep = NULL;
}

void DrawingArea::addToQueue(QGraphicsItem *item){
    qDebug() << "adding Graphics object\n";
    currentStep->items.enqueue(item);
}

void DrawingArea::show_all(){
    for(QQueue<GraphicsStep*>::iterator it= renderQueue.begin();it!=renderQueue.end();++it){
        for(QQueue<QGraphicsItem*>::iterator it2 = (*it)->items.begin();it2!=(*it)->items.end();++it2){
            scene->addItem(*it2);
        }
    }
}

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
}

void DrawingArea::startStopInnerPloly(){
    if(basePolygon->holes().last()->isClosed()){
        basePolygon->holes().last()->open();
        startStopInner->setText("Close");
        startStopBut->setEnabled(false);
    }else
    { basePolygon->holes().last()->close();
        Polygon *newPoly = new Polygon(basePolygon);
        basePolygon->holes().append(newPoly);
        startStopInner->setText("Open");
        startStopBut->setEnabled(true);
    }
}

void DrawingArea::triangulate(){
    qDebug() << "Triangualte clicked";
}
