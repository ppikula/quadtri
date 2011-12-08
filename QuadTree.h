#ifndef QUADTREE_H
#define QUADTREE_H
#include <QtGui>
#include <vector>
#include <utility>

// forward declarations
class DrawingArea;
class Polygon;
struct Edge;

class Point
{
public:
    Point():next(0),prev(0),x(0),y(0){}
    Point(double x,double y):next(0),prev(0),x(x),y(y){}

    Edge *next,*prev;
    double x,y;
};

class CornerPoint:public Point {
public:
    CornerPoint():Point(),left(0),right(0),opposite(0){}
    CornerPoint(double x,double y):Point(x,y),left(0),right(0),opposite(0){}
    CornerPoint *left,*right,*opposite;
};

class BorderPoint:public Point {
public:
    BorderPoint():Point(),left(0),right(0),oppositeLeft(0),oppositeRight(0){}
    BorderPoint(double x,double y):Point(x,y),left(0),right(0),oppositeRight(0){}
    Point *left,*right,*oppositeLeft,*oppositeRight;
};

//TODO: indices or pointers?

struct Edge
{
    Edge():b(0),e(0){}
    Edge(Point* b,Point* e):b(b),e(e){}
    void setPointsPtr(){b->next=this;e->prev=this;}
    Point *b,*e;
    bool intersects(Edge* e);
    bool intersects(Point *p,Point *p2);
    Point intersectionPoint(Edge* e);
    int getSide(Point *p);
};


struct Triangle
{
    Triangle(){}
    Triangle(Point *a,Point* b,Point*c):a(a),b(b),c(c){}
    Point *a,*b,*c;
    void draw(DrawingArea* area);
};



struct QuadTreeNode
{
    enum EQuadrant{
        EQ_NW,
        EQ_NE,
        EQ_SE,
        EQ_SW
    };

    enum Corner {
        P_NW,
        P_NE,
        P_SE,
        P_SW
    };

    enum Directions{
        P_N,
        P_E,
        P_S,
        P_W
    };

    static uint MAX_LEVEL;//maximum tree level

    EQuadrant type;
    Point* insertedPoint;
    Edge* insertedEdge;

    CornerPoint corners[4];
    BorderPoint borderPoints[4];
    Edge borders[4];


    Edge **crossEdges;
    QList<Triangle> triangles;

    Point lu_corner;
    uint level;
    double size;

    QuadTreeNode* parent;
    QScopedPointer<QuadTreeNode> NW,NE,SE,SW;//subnodes
    QuadTreeNode* N,*E,*S,*W;//neighbours

    static void initialize(const Point &p,double size,CornerPoint *corners,BorderPoint *borderPoints,Edge *borders,Edge ***crossEdges);
    QuadTreeNode(const Point&p,double size);
    QuadTreeNode(const Point&p,double size,QuadTreeNode* parent,EQuadrant type);

    void insert(Point* p);
    void insert(Edge* e);
    void subdivide();
    bool isLeaf();

    uint depth();
    void draw(DrawingArea *area);

    void upadteNeighbours();

private:
    void extractNeighbours();
    EQuadrant whichQuadrant(const Point& p) const;
    bool contains(Edge* e) const;
    Point* findItersectionPoints(Edge* e);

};

class QuadTree
{
    QScopedPointer<QuadTreeNode> root;

    std::vector<Point> points;
    std::vector<Edge> edges;
    std::vector<Triangle> tris;

public:
    QuadTree(const Point& p, uint size);

    void insert(const Point& p);
    // points with constraints
    void insertPolygon(Polygon* poly);

    //maximum level
    uint depth();
    //should return  triangulation result
    void Triangulate();
    void applyTemplateTriangulation(QuadTreeNode* n);
    void applyPointTriangulation(QuadTreeNode* n);
    void applyEdgeTriangulation(QuadTreeNode *n);
    //std::list<QuadTreeNode> leaves;


    void generatePointTree(Point** p,QuadTreeNode*n ,int type,int start);
    void draw(DrawingArea *area);
private:
    void calculateTrianglesForEdgeCase(Edge *e,Point *p,Point *sp,Point *op,BorderPoint **otherBP,int othersSize);
};

#endif // QUADTREE_H
