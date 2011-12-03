#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QtGui>
#include "polygon.h"
#include "drawingscene.h"
#include "drawingstep.h"
class DrawingArea : public QWidget
{
    Q_OBJECT
public:    
    explicit DrawingArea(QWidget *parent = 0);

    void startStep(float time);
    void stopStep();
    void addToQueue(QGraphicsItem *item);
    Polygon * polygon();

signals:
    void polyChanged(Polygon &poly);



private slots:
    void startStopPoly();
    void startStopInnerPloly();
    void viewSliderChanged(int v);

signals:

public slots:

private:

    /* Controls */
    QGraphicsView *view;
    DrawingScene *scene;
    QPushButton *startStopBut,*startStopInner;
    QSlider *slider;

    /* Logic items */
    Polygon *basePolygon;
    QList<DrawingStep*> renderQueue;
};

#endif // DRAWINGAREA_H
