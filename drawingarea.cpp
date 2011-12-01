#include "drawingarea.h"
#include "drawingscene.h"

DrawingArea::DrawingArea(QWidget *parent) :
    QWidget(parent),currentStep(NULL)
{
    basePolygon = new Polygon();
    scene = new DrawingScene(basePolygon,0, 0, 800, 600);

    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setBackgroundBrush(QColor(0,0,0));

    startStopBut =new QPushButton();
    startStopBut->setText("Stop");

    triangulateBut = new QPushButton();
    triangulateBut->setText("Triangualte");

    slider = new QSlider();
    slider->setOrientation(Qt::Horizontal);

    QHBoxLayout *buttonsLay = new QHBoxLayout();
    buttonsLay->addWidget(startStopBut);
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
    qDebug() << "Start/Stop clicked";
}

void DrawingArea::triangulate(){
    qDebug() << "Triangualte clicked";
}
