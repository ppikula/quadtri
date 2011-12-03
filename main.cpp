#include <QtGui>
#include "drawingarea.h"
#include "polydot.h"
#include "polygon.h"



void clicked(){
    qDebug() << "triangualte clicked!";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DrawingArea *area = new DrawingArea();

    QPushButton *triangualteBut = new QPushButton();
    triangualteBut->setText("Triangulate");

    QVBoxLayout *lay = new QVBoxLayout();
    lay->addWidget(area);
    lay->addWidget(triangualteBut);


    QWidget window;
    window.setLayout(lay);
    window.show();

        for(int i=0;i<50;i++){
            area->startStep(0.1);
            for(int j=1;j<qrand()%10;j++){
                qreal ax = 100+qrand()%300;
                qreal ay = 100+qrand()%300;
                qreal bx = 100+qrand()%300;
                qreal by = 100+qrand()%300;
                QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(ax,ay,bx,by));
                QPen pen(Qt::yellow);
                line->setPen(pen);
                area->addToQueue(line);
            }
            area->stopStep();
        }

    return a.exec();
}

//void DrawingArea::triangulate(){
//    for(int i=0;i<50;i++){
//        SceneStep *step = new SceneStep();
//        for(int j=1;j<qrand()%10;j++){
//            qreal ax = 100+qrand()%300;
//            qreal ay = 100+qrand()%300;
//            qreal bx = 100+qrand()%300;
//            qreal by = 100+qrand()%300;
//            QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(ax,ay,bx,by));
//            step->items.append(line);
//        }
//        scene->addSceneStep(step);
//    }
//    slider->setMaximum(50);
//}
