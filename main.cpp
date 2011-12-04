#include <QtGui>
#include "DrawingArea.h"
#include "PolyDot.h"
#include "Polygon.h"
#include "Triangulator.h"



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

    Triangulator *t = new Triangulator(area);

    QObject::connect(triangualteBut,SIGNAL(clicked()),t,SLOT(triangulate()));


    return a.exec();
}
