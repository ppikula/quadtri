#ifndef SCENESTEP_H
#define SCENESTEP_H

#include <QtGui>

class DrawingStep: public QObject,public QGraphicsItemGroup
{
    Q_INTERFACES(QGraphicsItemGroup)
public:
    enum StepType { QUADRANT, TRIANGLE };

    explicit DrawingStep(StepType type);
    StepType type;
    QString description;
};

#endif // SCENESTEP_H
