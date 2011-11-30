#ifndef QUADTRI_H
#define QUADTRI_H

#include <QMainWindow>

namespace Ui {
    class QuadTri;
}

class QuadTri : public QMainWindow
{
    Q_OBJECT

public:
    explicit QuadTri(QWidget *parent = 0);
    ~QuadTri();

private:
    Ui::QuadTri *ui;
};

#endif // QUADTRI_H
