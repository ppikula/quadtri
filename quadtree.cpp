#include "quadtree.h"
#include <vector>
#include <list>
#include <queue>
#include <algorithm>

uint QuadTreeNode::MAX_LEVEL = 20;

static int direction(const Point& a, const Point& b,const Point& c){
    double res = (a.x-c.x)*(b.y-c.y) -(b.x-c.x)*(a.y-c.y);
    if( res< 0 ) return -1;
    else if (res > 0 ) return 1;
    return 0;
}

bool Edge::intersects(Edge* e)
{
    if( direction(l1,l2.start) == direction(l1,l2.end) )return false;
    if( direction(l2,l1.start) == direction(l2,l1.end) )return false;
    return true;
}

QuadTreeNode::QuadTreeNode(const Point& p, double size):lu_corner(p),level(0),size(size),parent(0)
{
    insertedPoint=0;
    insertedEdge=0;
}

QuadTreeNode::QuadTreeNode(const Point& p, double size,QuadTreeNode* parent):lu_corner(p),size(size),parent(parent)
{
    Q_ASSERT(parent);
    level=parent->level+1;
    insertedPoint=0;
    insertedEdge=0;
    //extractNeighbours();
}

void QuadTreeNode::subdivide()
{
    if(isLeaf() && level<MAX_LEVEL)
    {
        NW.reset(new QuadTreeNode(lu_corner,size/2.0,this));
        NE.reset(new QuadTreeNode(Point(lu_corner.x+size/2.0,lu_corner.y),size/2.0,this));
        SE.reset(new QuadTreeNode(Point(lu_corner.x+size/2.0,lu_corner.y+size/2.0),size/2.0,this));
        SW.reset(new QuadTreeNode(Point(lu_corner.x,lu_corner.y+size/2.0),size/2.0,this));

        if(!N.isNull()&&N->depth()<level)N->subdivide();
        if(!E.isNull()&&E->depth()<level)E->subdivide();
        if(!S.isNull()&&S->depth()<level)S->subdivide();
        if(!W.isNull()&&W->depth()<level)W->subdivide();

        //filling obvious neighbours
        NW->E.reset(NE.data());
        NW->S.reset(SW.data());

        NE->W.reset(NW.data());
        NE->S.reset(SE.data());

        SE->W.reset(SW.data());
        SE->N.reset(NE.data());

        SW->N.reset(NW.data());
        SW->E.reset(SE.data());


        if( insertedPoint )
        {
            switch(whichQuadrant(insertedPoint))
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

QuadTreeNode::EQuadrant QuadTreeNode::whichQuadrant(const Point& p)
{
    bool e = p.x >= lu_corner.x+size/2;
    bool s = p.y >= lu_corner.y+size/2;
    if(!e && !s)return EQ_NW;
    else if(e && s) return EQ_SE;
    else if(e) return EQ_NE;
    else if(s) return EQ_SW;
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
         e->e->y >= lu_corner.y &&  e->e->y <= lub_corner.y+size))return true;

    return false;
}

void QuadTreeNode::extractNeighbours()
{
    //TODO: neighbours
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
    std::list<QuadTreeNode> it=leaves.begin();
    for(;it!=leaves.end();it++)
    {

    }

    //TODO: filtering and mesh opitimizations

}


