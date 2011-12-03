#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QtGui>
#include "polygon.h"

class DrawingArea : public QWidget
{
    Q_OBJECT

public:    
    struct GraphicsStep {
        float time;
        QQueue<QGraphicsItem*> items;
        GraphicsStep(float time):time(time){}
    };

    explicit DrawingArea(QWidget *parent = 0);

    void startStep(float time);
    void stopStep();
    void addToQueue(QGraphicsItem *item);
    void show_all();

    QQueue<GraphicsStep*> renderQueue;
    GraphicsStep *currentStep;

    QGraphicsView *view;
    QGraphicsScene *scene;
    QPushButton *startStopBut,*startStopInner,*triangulateBut;

    QSlider *slider;

private slots:
    void startStopPoly();
    void startStopInnerPloly();
    void triangulate();

signals:

public slots:

private:
    Polygon *basePolygon;
};

#endif // DRAWINGAREA_H
