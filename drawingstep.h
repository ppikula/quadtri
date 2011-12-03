#ifndef SCENESTEP_H
#define SCENESTEP_H

#include <QtGui>

class DrawingStep: public QGraphicsItemGroup
{
public:
    DrawingStep(qreal value=0.1,QString description="");
    qreal value;
    QString description;

};

#endif // SCENESTEP_H
