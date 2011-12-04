#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QtGui>
#include "Polygon.h"
#include "DrawingScene.h"
#include "DrawingStep.h"
class DrawingArea : public QWidget
{
    Q_OBJECT
public:    
    explicit DrawingArea(QWidget *parent = 0);

    void startStep(float time);
    void stopStep();
    void addToQueue(QGraphicsItem *item);

    void clearAll();
    void clear();

    Polygon *polygon() {return basePolygon;}
    QList<Polygon*> holesInPoly() {return holes;}
    QString polyString();

private slots:
    void startStopPoly();
    void startStopInnerPloly();
    void viewSliderChanged(int v);

signals:
    void polyChanged(Polygon *poly,QList<Polygon*> holes);

public slots:
    void polyUpdate();
    void sceneDoubleCliked(QGraphicsSceneMouseEvent *event);

private:

    /* Controls */
    QGraphicsView *view;
    DrawingScene *scene;
    QPushButton *startStopBut,*startStopInner;
    QSlider *slider;

    /* Logic items */
    Polygon *basePolygon;
    QList<Polygon*> holes;
    QList<DrawingStep*> renderQueue;
};

#endif // DRAWINGAREA_H
