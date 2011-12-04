#ifndef QUADTREE_H
#define QUADTREE_H
#include <QScopedPointer>
#include <vector>

struct Point
{
    Point():x(0),y(0){}
    Point(double x,double y):x(x),y(y){}
    double x,y;
};

//TODO: indices or pointers?

struct Edge
{
    Edge():b(0),e(0){}
    Point *b,*e;
    bool intersects(Edge* e);
};


struct Triangle
{
    Triangle(){}
    Point *a,*b,*c;
};

// forward declarations
class DrawingArea;
class Polygon;

struct QuadTreeNode
{
    enum EQuadrant{
        EQ_NW,
        EQ_NE,
        EQ_SE,
        EQ_SW
    };

    static uint MAX_LEVEL;//maximum tree level

    EQuadrant type;
    Point* insertedPoint;
    Edge* insertedEdge;

    Point lu_corner;
    uint level;
    double size;

    QuadTreeNode* parent;
    QScopedPointer<QuadTreeNode> NW,NE,SE,SW;//subnodes
    QuadTreeNode* N,*E,*S,*W;//neighbours

    QuadTreeNode(const Point&p,double size);
    QuadTreeNode(const Point&p,double size,QuadTreeNode* parent,EQuadrant type);

    void insert(Point* p);
    //FIXME: criterion for subdivision apply when there is no point in quadrant
    void insert(Edge* e);
    void subdivide();
    bool isLeaf();

    uint depth();

    void draw(DrawingArea *area);

private:
    void extractNeighbours();
    EQuadrant whichQuadrant(const Point& p) const;
    bool contains(Edge* e) const;

};



class QuadTree
{
    QScopedPointer<QuadTreeNode> root;

    std::vector<Point> points;
    std::vector<Edge> edges;

public:
    QuadTree(const Point& p, uint size);

    void insert(const Point& p);
    // points with constraints
    void insertPolygon(Polygon* poly);

    //maximum level
    uint depth();
    //should return  triangulation result
    void Triangulate();
    //std::list<QuadTreeNode> leaves;


    void draw(DrawingArea *area);
};

#endif // QUADTREE_H
