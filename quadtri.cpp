#include "quadtri.h"
#include "ui_quadtri.h"

QuadTri::QuadTri(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QuadTri)
{
    ui->setupUi(this);
}

QuadTri::~QuadTri()
{
    delete ui;
}
