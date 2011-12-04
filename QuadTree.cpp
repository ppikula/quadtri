#include "QuadTree.h"
#include <vector>
#include <list>
#include <queue>
#include <algorithm>
#include "Triangulator.h"

uint QuadTreeNode::MAX_LEVEL = 20;

static int direction(const Point& a, const Point& b,const Point& c){
    double res = (a.x-c.x)*(b.y-c.y) -(b.x-c.x)*(a.y-c.y);
    if( res< 0 ) return -1;
    else if (res > 0 ) return 1;
    return 0;
}

bool Edge::intersects(Edge* e)
{
    if( direction(*b,*(this->e),*(e->b)) == direction(*b,*(this->e),*(e->e) ))return false;
    if( direction(*(e->b),*(e->e),*(this->b)) == direction(*(e->b),*(e->e),*(this->e))) return false;
    return true;
}

QuadTreeNode::QuadTreeNode(const Point& p, double size):lu_corner(p),level(0),size(size),parent(0)
{
    insertedPoint=0;
    insertedEdge=0;
    N=0;
    S=0;
    W=0;
    E=0;
}

QuadTreeNode::QuadTreeNode(const Point& p, double size,QuadTreeNode* parent,EQuadrant type):lu_corner(p),size(size),parent(parent)
{
    Q_ASSERT(parent);
    level=parent->level+1;
    insertedPoint=NULL;
    insertedEdge=NULL;
    //extractNeighbours();
    N=0;
    S=0;
    W=0;
    E=0;
    this->type=type;
}

void QuadTreeNode::subdivide()
{
    if(isLeaf() && level<MAX_LEVEL)
    {
        double s2=size/2;
        NW.reset(new QuadTreeNode(lu_corner,s2,this,EQ_NW));
        NE.reset(new QuadTreeNode(Point(lu_corner.x+s2,lu_corner.y),s2,this,EQ_NE));
        SE.reset(new QuadTreeNode(Point(lu_corner.x+s2,lu_corner.y+s2),s2,this,EQ_SE));
        SW.reset(new QuadTreeNode(Point(lu_corner.x,lu_corner.y+s2),s2,this,EQ_SW));

        if(N && N->depth()<level)N->subdivide();
        if(E && E->depth()<level)E->subdivide();
        if(S && S->depth()<level)S->subdivide();
        if(W && W->depth()<level)W->subdivide();

        //non root
        if(parent)
        {
            if( (type==EQ_SE || type==EQ_SW) && parent->S && parent->S->depth()<level )
            {
                parent->S->subdivide();
                if(type==EQ_SE)S=parent->S->NE.data();
                if(type==EQ_SW)S=parent->S->NW.data();
            }
            if( (type==EQ_NE || type==EQ_NW) && parent->N && parent->N->depth()<level )
            {
                parent->N->subdivide();
                if(type==EQ_NE)N=parent->N->SE.data();
                if(type==EQ_NW)N=parent->N->SW.data();
            }
            if( (type==EQ_SE || type==EQ_NE) && parent->E && parent->E->depth()<level )
            {
                parent->E->subdivide();
                if(type==EQ_NE)E=parent->E->NW.data();
                if(type==EQ_SE)E=parent->E->SW.data();
            }
            if( (type==EQ_NW || type==EQ_SW) && parent->W && parent->W->depth()<level )
            {
                parent->W->subdivide();
                if(type==EQ_NW)W=parent->W->NE.data();
                if(type==EQ_SW)W=parent->W->SE.data();
            }

        }

        //filling obvious neighbours
        NW->E=NE.data();
        NW->S=SW.data();

        NE->W=NW.data();
        NE->S=SE.data();

        SE->W=SW.data();
        SE->N=NE.data();

        SW->N=NW.data();
        SW->E=SE.data();

        //other neighbours
        if(parent)
        {
            /*if( parent->N  && parent->N->depth()==level)
            {
                if(type==EQ_NW)N=parent->SW.data();
                if(type==EQ_NE)N=parent->SE.data();
            }*/
        }


        if(N && N->depth()>=level+1)
        {
            //NW->N = N->SW.data();
            //NE->N = N->SE.data();
        }

        /*if(S && W->depth()>=level+1){ NW->N=N->SW.data();NE->N=N->SE.data(); }
        if(E && E->depth()>=level+1){ NW->N=N->SW.data();NE->N=N->SE.data(); }
        if(W && W->depth()>=level+1){ NW->N=N->SW.data();NE->N=N->SE.data(); }*/



        if( insertedPoint )
        {
            switch(whichQuadrant(*insertedPoint))
            {
                case EQ_NW:
                    NW->insert(insertedPoint);
                    break;
                case EQ_NE:
                    NE->insert(insertedPoint);
                    break;
                case EQ_SE:
                    SE->insert(insertedPoint);
                    break;
                case EQ_SW:
                    SW->insert(insertedPoint);
                    break;
            }
            insertedPoint=NULL;
        }
    }
}

bool QuadTreeNode::isLeaf()
{
    return NW.isNull() && NE.isNull() && SE.isNull() && SW.isNull();
}

uint QuadTreeNode::depth()
{
    if(isLeaf())return level;

    std::vector<uint> depths;
    depths.push_back(NW->depth());
    depths.push_back(NE->depth());
    depths.push_back(SE->depth());
    depths.push_back(SW->depth());

    return *(std::max_element(depths.begin(),depths.end()));
}

QuadTreeNode::EQuadrant QuadTreeNode::whichQuadrant(const Point& p) const
{
    bool x2 = p.x >= lu_corner.x+size/2;
    bool y2 = p.y >= lu_corner.y+size/2;
    if(!x2 && !y2)return EQ_NW;
    else if(x2 && y2) return EQ_SE;
    else if(x2 && !y2) return EQ_NE;
    else if(y2 && !x2) return EQ_SW;
}

void QuadTreeNode::insert(Point* p)
{
    if( isLeaf() && !insertedPoint ){
        insertedPoint=p;
    }else {
        subdivide();//wont subdivide quadrant already divided
        switch(whichQuadrant(*p))
        {
            case EQ_NW:
                NW->insert(p);
                break;
            case EQ_NE:
                NE->insert(p);
                break;
            case EQ_SE:
                SE->insert(p);
                break;
            case EQ_SW:
                SW->insert(p);
                break;
        }
    }
}

void QuadTreeNode::insert(Edge* e)
{
    if( isLeaf() && !insertedEdge && !insertedPoint ){
        insertedEdge=e;
    }else{
        subdivide();//wont subdivide quadrant already divided
        if(NW->contains(e))NW->insert(e);
        if(NE->contains(e))NE->insert(e);
        if(SE->contains(e))SE->insert(e);
        if(SW->contains(e))SW->insert(e);
    }
}

bool QuadTreeNode::contains(Edge* e) const
{
    Point pts[4];
    Edge  edg[4];

    for(int i=0;i<4;++i){
        edg[i].b=&pts[i];
        edg[i].e=&pts[(i+1)%4];
        if (edg[i].intersects(e)) return true;
    }

    if( (e->b->x >= lu_corner.x &&  e->b->x <= lu_corner.x+size &&
         e->b->y >= lu_corner.y &&  e->b->y <= lu_corner.y+size)&&
        (e->e->x >= lu_corner.x &&  e->e->x <= lu_corner.x+size &&
         e->e->y >= lu_corner.y &&  e->e->y <= lu_corner.y+size))return true;

    return false;
}

void QuadTreeNode::extractNeighbours()
{
    //TODO: neighbours
}

void QuadTreeNode::draw(DrawingArea *area)
{
    if (!isLeaf())
    {
        QGraphicsLineItem* vert=new QGraphicsLineItem(QLineF(lu_corner.x+size/2,
                                                      lu_corner.y,
                                                      lu_corner.x+size/2,
                                                      lu_corner.y+size));

        QGraphicsLineItem* hor =new QGraphicsLineItem(QLineF(lu_corner.x,
                                                             lu_corner.y+size/2,
                                                             lu_corner.x+size,
                                                             lu_corner.y+size/2));

        QPen pen(Qt::yellow);
        vert->setPen(pen);
        hor->setPen(pen);
        area->addToQueue(vert);
        area->addToQueue(hor);

        NW->draw(area);
        NE->draw(area);
        SE->draw(area);
        SW->draw(area);
    }
}

/////////////////////////////////////////////////////////////
// QuadTree
/////////////////////////////////////////////////////////////

QuadTree::QuadTree(const Point &p, uint size)
{
    root.reset(new QuadTreeNode(p,size));
}

uint QuadTree::depth()
{
    return root->depth();
}

void QuadTree::Triangulate()
{
    std::list<QuadTreeNode*> leaves;
    std::queue<QuadTreeNode*> nodes_q;
    nodes_q.push(root.data());
    while(!nodes_q.empty())
    {
        if(nodes_q.front()->isLeaf()){
            leaves.push_back(nodes_q.front());
            nodes_q.pop();
        }else {
            QuadTreeNode* n=nodes_q.front();
            nodes_q.pop();
            nodes_q.push(n->NW.data());
            nodes_q.push(n->NE.data());
            nodes_q.push(n->SE.data());
            nodes_q.push(n->SW.data());
        }
    }


    // iterate over leaves and create triangles
    // analyse neighbours and create triangles
    /*std::list<QuadTreeNode*> it=leaves.begin();
    for(;it!=leaves.end();it++)
    {

    }*/

    //TODO: filtering and mesh opitimizations

}

void QuadTree::draw(DrawingArea *area)
{
    area->startStep(10);

    QGraphicsRectItem* quad=new QGraphicsRectItem(QRectF(root->lu_corner.x,root->lu_corner.y,
                                                         root->size,root->size));
    QPen pen(Qt::yellow);
    quad->setPen(pen);
    area->addToQueue(quad);
    root->draw(area);


    area->stopStep();
}

void QuadTree::insertPolygon(Polygon* poly)
{
    QRectF bound=poly->boundingRect();
    //PP: WTF? why 15 is offset? ?
    root->lu_corner.x=bound.topLeft().x()-15;
    root->lu_corner.y=bound.topLeft().y()-15;

    root->size=std::max(bound.height()+10,bound.width()+10);

    //iterate over poly and add points
    QList<PolyDot* >::const_iterator it;

    points.clear();
    edges.clear();

    //blah FIMXE!!!
    edges.reserve(100000);
    points.reserve(1000000);
    //qDebug()<<"===========";

    for(it=poly->boundary.constBegin();it!=poly->boundary.constEnd();it++){
        insert(Point((*it)->center().x(),(*it)->center().y()));
    }
}

void QuadTree::insert(const Point& p)
{
    points.push_back(p);
    root->insert(&points.back());
}
