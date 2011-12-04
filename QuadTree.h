#ifndef QUADTREE_H
#define QUADTREE_H
#include <QScopedPointer>
#include <vector>
#include <utility>

// forward declarations
class DrawingArea;
class Polygon;
struct Edge;

struct Point
{
    Point():x(0),y(0),next(0),prev(0){}
    Point(double x,double y):x(x),y(y),next(0),prev(0){}

    Edge *next,*prev;
    double x,y;
};

//TODO: indices or pointers?

struct Edge
{
    Edge():b(0),e(0){}
    Edge(Point* b,Point* e):b(b),e(e){}
    void setPointsPtr(){b->next=this;e->prev=this;}
    Point *b,*e;
    bool intersects(Edge* e);
    Point intersectionPoint(Edge* e);
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

    void upadteNeighbours();

private:
    void extractNeighbours();
    EQuadrant whichQuadrant(const Point& p) const;
    bool contains(Edge* e) const;
    Point itersectionP(Edge* e);

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
};

#endif // QUADTREE_H
