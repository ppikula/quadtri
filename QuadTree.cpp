#include "QuadTree.h"
#include <vector>
#include <list>
#include <queue>
#include <algorithm>
#include "Triangulator.h"

uint QuadTreeNode::MAX_LEVEL = 1000;

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

Point Edge::intersectionPoint(Edge* ed)
{

    double a1 = (b->y-e->y)/(b->x-e->x);
    double a2 = (ed->b->y-ed->e->y)/(ed->b->x-ed->e->x);
    double b1 = (b->y+e->y)-a1*(b->x+e->x);
    double b2 = (ed->b->y+ed->e->y)-a2*(ed->b->x+ed->e->x);
    b1*=0.5;
    b2*=0.5;

    Point result(0.0,0.0);

    if( (b->x-e->x)==0 ){
        result.x= b->x;
        result.y=a2*result.x+b2;
    }else if((ed->b->x-ed->e->x)==0){
        result.x= ed->b->x;
        result.y=a1*result.x+b1;
    }else{
        result.x = (b2-b1)/(a1-a2);
        result.y = a1*result.x + b1;
    }

    return result;
}

void Triangle::draw(DrawingArea* area)
{
    QGraphicsLineItem* l1 = new QGraphicsLineItem(QLineF(a->x,a->y,b->x,b->y));
    QGraphicsLineItem* l2 = new QGraphicsLineItem(QLineF(b->x,b->y,c->x,c->y));
    QGraphicsLineItem* l3 = new QGraphicsLineItem(QLineF(c->x,c->y,a->x,a->y));

    QPen pen(Qt::cyan);
    pen.setWidth(3);
    l1->setPen(pen);
    l2->setPen(pen);
    l3->setPen(pen);

    area->addToQueue(l1);
    area->addToQueue(l2);
    area->addToQueue(l3);
}

///QuadTree Node

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

        if( insertedEdge )
        {
            Edge* e =insertedEdge;
            if(NW->contains(e))NW->insert(e);
            if(NE->contains(e))NE->insert(e);
            if(SE->contains(e))SE->insert(e);
            if(SW->contains(e))SW->insert(e);
            insertedEdge=NULL;
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
        if(!insertedPoint)//if in leaf does not exist point
        {
            subdivide();//wont subdivide quadrant already divided
            if(NW->contains(e))NW->insert(e);
            if(NE->contains(e))NE->insert(e);
            if(SE->contains(e))SE->insert(e);
            if(SW->contains(e))SW->insert(e);
        }
    }
}

bool QuadTreeNode::contains(Edge* e) const
{
    Point pts[4]={lu_corner,
                  Point(lu_corner.x+size,lu_corner.y),
                  Point(lu_corner.x+size,lu_corner.y+size),
                  Point(lu_corner.x,lu_corner.y+size)};
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

Point QuadTreeNode::itersectionP(Edge* e)
{

}

void QuadTreeNode::extractNeighbours()
{
    //TODO: neighbours
}

void QuadTreeNode::upadteNeighbours()
{
    if(parent){
        if(type==EQ_NW)
        {
            if(parent->N && !parent->N->isLeaf())   N=parent->N->SW.data();
            if(parent->W && !parent->W->isLeaf())   W=parent->W->NE.data();

        }
        if(type==EQ_NE)
        {
            if(parent->N && !parent->N->isLeaf())   N=parent->N->SE.data();
            if(parent->E && !parent->E->isLeaf())   E=parent->E->NW.data();

        }
        if(type==EQ_SE)
        {
            if(parent->S && !parent->S->isLeaf())   S=parent->S->NE.data();
            if(parent->E && !parent->E->isLeaf())   E=parent->E->SW.data();

        }
        if(type==EQ_SW)
        {
            if(parent->S && !parent->S->isLeaf())   S=parent->S->NW.data();
            if(parent->W && !parent->W->isLeaf())   W=parent->W->SE.data();
        }

    }
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

    }else{
        QPen pen2(Qt::red);


        if(insertedPoint){
            Point pts[4]={lu_corner,
                          Point(lu_corner.x+size,lu_corner.y),
                          Point(lu_corner.x+size,lu_corner.y+size),
                          Point(lu_corner.x,lu_corner.y+size)};
            Edge  edg[4];

            for(int i=0;i<4;++i){
                edg[i].b=&pts[i];
                edg[i].e=&pts[(i+1)%4];

                if (insertedPoint->next && edg[i].intersects(insertedPoint->next)){
                    Point pp=insertedPoint->next->intersectionPoint(&edg[i]);
                    QGraphicsRectItem *r = new QGraphicsRectItem(pp.x-5,pp.y-5,10,10);
                    r->setPen(pen2);
                    area->addToQueue(r);
                }

                if (insertedPoint->prev && edg[i].intersects(insertedPoint->prev)){
                    Point pp=insertedPoint->prev->intersectionPoint(&edg[i]);
                    QGraphicsRectItem *r = new QGraphicsRectItem(pp.x-5,pp.y-5,10,10);
                    r->setPen(pen2);
                    area->addToQueue(r);
                }
            }


        }
    }
}

/////////////////////////////////////////////////////////////
// QuadTree
/////////////////////////////////////////////////////////////

QuadTree::QuadTree(const Point &p, uint size)
{
    root.reset(new QuadTreeNode(p,size));
    points.clear();
    edges.clear();

    //blah FIMXE!!!
    edges.reserve(100000);
    points.reserve(1000000);
    //qDebug()<<"===========";
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
    std::list<QuadTreeNode*>::iterator it=leaves.begin();
    for(;it!=leaves.end();it++)
    {
        QuadTreeNode* n=*it;
        n->upadteNeighbours();

      /*  Point *p1=new Point(n->lu_corner);
        Point *p2=new Point(n->lu_corner.x+n->size,n->lu_corner.y);
        Point *p3=new Point(n->lu_corner.x,n->lu_corner.y+n->size);
        Point *p4=new Point(n->lu_corner.x+n->size,n->lu_corner.y+n->size);*/

        if(n->insertedPoint){
           applyPointTriangulation(n);
        }else if(n->insertedEdge){
           applyEdgeTriangulation(n);
        }else{
           applyTemplateTriangulation(n);
        }

    }

    //TODO: filter  triangles outside use QPolygon

    //TODO: mesh opitimizations

}

void QuadTree::applyTemplateTriangulation(QuadTreeNode* n)
{
    Point *p[8];
    p[0]=new Point(n->lu_corner);
    p[1]=new Point(n->lu_corner.x+n->size,n->lu_corner.y);
    p[2]=new Point(n->lu_corner.x,n->lu_corner.y+n->size);
    p[3]=new Point(n->lu_corner.x+n->size,n->lu_corner.y+n->size);

    p[4]=new Point(n->lu_corner.x+n->size/2.0,n->lu_corner.y);
    p[5]=new Point(n->lu_corner.x+n->size,n->lu_corner.y+n->size/2.0);
    p[6]=new Point(n->lu_corner.x+n->size/2.0,n->lu_corner.y+n->size);
    p[7]=new Point(n->lu_corner.x,n->lu_corner.y+n->size/2.0);



    int nn=0,ss=0,ww=0,ee=0;
    if( n->N && !n->N->isLeaf() )nn++;
    if( n->S && !n->S->isLeaf() )ss++;
    if( n->E && !n->E->isLeaf() )ee++;
    if( n->W && !n->W->isLeaf() )ww++;

    int sum =nn+ss+ww+ee;

    //16 cases - can be  simplified
    if( sum==0 )
    {
        tris.push_back(Triangle(p[0],p[2],p[1]));
        tris.push_back(Triangle(p[2],p[3],p[1]));
    }else if(sum==4 ){
        tris.push_back(Triangle(p[7],p[4],p[0]));
        tris.push_back(Triangle(p[4],p[5],p[1]));

        tris.push_back(Triangle(p[7],p[3],p[5]));
        tris.push_back(Triangle(p[7],p[2],p[6]));

        tris.push_back(Triangle(p[7],p[6],p[4]));
        tris.push_back(Triangle(p[4],p[7],p[5]));

    }else if(sum==3){

        int offset=0;
        if(ss=0)offset=0;
        else if(ww==0)offset=1;
        else if(nn==0)offset=2;
        else if(ee==0)offset=3;
        std::swap(p[2],p[3]);
        //up
        tris.push_back(Triangle(p[offset],p[4+(3+offset)%4],p[4+offset]));
        tris.push_back(Triangle(p[4+offset],p[4+(offset+1)%4],p[(1+offset)%4]));
        tris.push_back(Triangle(p[4+(3+offset)%4],p[4+(offset+1)%4],p[4+offset]));
        //down
        tris.push_back(Triangle(p[(3+offset)%4],p[(2+offset)%4],p[4+(3+offset)%4]));
        tris.push_back(Triangle(p[(2+offset)%4],p[4+(1+offset)%4],p[4+(3+offset)%4]));


    }else if(sum==2)
    {
        //ehh
        if(nn==1 &&ss==1 ){
            tris.push_back(Triangle(p[0],p[2],p[4]));
            tris.push_back(Triangle(p[2],p[6],p[4]));
            tris.push_back(Triangle(p[4],p[6],p[1]));
            tris.push_back(Triangle(p[1],p[6],p[3]));
        }else if(ee==1&&ww==1){
            tris.push_back(Triangle(p[0],p[7],p[1]));
            tris.push_back(Triangle(p[7],p[5],p[1]));
            tris.push_back(Triangle(p[2],p[5],p[7]));
            tris.push_back(Triangle(p[2],p[3],p[5]));
        /////////////////////////////////
        }else if(nn==1 &&ee==1 ){
            tris.push_back(Triangle(p[0],p[2],p[4]));
            tris.push_back(Triangle(p[4],p[5],p[1]));
            tris.push_back(Triangle(p[2],p[5],p[4]));
            tris.push_back(Triangle(p[2],p[3],p[5]));
        }else if(ee==1 &&ss==1 ){
            tris.push_back(Triangle(p[0],p[5],p[6]));
            tris.push_back(Triangle(p[0],p[2],p[6]));
            tris.push_back(Triangle(p[0],p[5],p[1]));
            tris.push_back(Triangle(p[6],p[3],p[5]));
        }else if(ss==1 &&ww==1 ){
            tris.push_back(Triangle(p[0],p[7],p[1]));
            tris.push_back(Triangle(p[7],p[2],p[6]));
            tris.push_back(Triangle(p[7],p[6],p[1]));
            tris.push_back(Triangle(p[6],p[3],p[1]));
        }else if(ww==1 &&nn==1 ){
            tris.push_back(Triangle(p[0],p[7],p[4]));
            tris.push_back(Triangle(p[7],p[3],p[4]));
            tris.push_back(Triangle(p[7],p[2],p[3]));
            tris.push_back(Triangle(p[4],p[3],p[1]));
        }

    }else if(sum==1)
    {
        if(nn){
          tris.push_back(Triangle(p[0],p[2],p[4]));
          tris.push_back(Triangle(p[4],p[3],p[1]));
          tris.push_back(Triangle(p[2],p[3],p[4]));
        }else if(ss)
        {
          tris.push_back(Triangle(p[0],p[2],p[6]));
          tris.push_back(Triangle(p[6],p[3],p[1]));
          tris.push_back(Triangle(p[0],p[6],p[1]));
        }else if(ee)
        {
          tris.push_back(Triangle(p[0],p[5],p[1]));
          tris.push_back(Triangle(p[2],p[3],p[5]));
          tris.push_back(Triangle(p[2],p[5],p[0]));
        }else if(ww){
          tris.push_back(Triangle(p[7],p[1],p[0]));
          tris.push_back(Triangle(p[7],p[2],p[3]));
          tris.push_back(Triangle(p[3],p[1],p[7]));
        }
    }

}

void QuadTree::applyPointTriangulation(QuadTreeNode* n)
{
    Point *p[8];
    p[0]=new Point(n->lu_corner);
    p[1]=new Point(n->lu_corner.x+n->size/2.0,n->lu_corner.y);
    p[2]=new Point(n->lu_corner.x+n->size,n->lu_corner.y);
    p[3]=new Point(n->lu_corner.x+n->size,n->lu_corner.y+n->size/2.0);
    p[4]=new Point(n->lu_corner.x+n->size,n->lu_corner.y+n->size);
    p[5]=new Point(n->lu_corner.x+n->size/2.0,n->lu_corner.y+n->size);
    p[6]=new Point(n->lu_corner.x,n->lu_corner.y+n->size);
    p[7]=new Point(n->lu_corner.x,n->lu_corner.y+n->size/2.0);

    int nn=0,ss=0,ww=0,ee=0;
    if( n->N && !n->N->isLeaf() )nn++;
    if( n->S && !n->S->isLeaf() )ss++;
    if( n->E && !n->E->isLeaf() )ee++;
    if( n->W && !n->W->isLeaf() )ww++;

    Point* ip=n->insertedPoint;
    generatePointTree(p,n,nn,0);
    generatePointTree(p,n,ee,2);
    generatePointTree(p,n,ss,4);
    generatePointTree(p,n,ww,6);

}
void QuadTree::generatePointTree(Point** p,QuadTreeNode*n ,int type,int start)
{
    Point* ip=n->insertedPoint;
    if( type==1){
        Edge e(p[start],p[(start+1)]);
        if(e.intersects(ip->next)){
            Point pr = e.intersectionPoint(ip->next);
            tris.push_back(Triangle(p[start],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[start+1]));
        }if(e.intersects(ip->prev)){
            Point pr = e.intersectionPoint(ip->prev);
            tris.push_back(Triangle(p[start],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[start+1]));
        }else
            tris.push_back(Triangle(p[start],ip,p[start+1]));


       Edge e2(p[start+1],p[(start+2)%8]);
        if(e2.intersects(ip->next)){
            Point pr = e2.intersectionPoint(ip->next);
            tris.push_back(Triangle(p[start+1],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[(start+2)%8]));
        }if(e2.intersects(ip->prev)){
            Point pr = e2.intersectionPoint(ip->prev);
            tris.push_back(Triangle(p[start+1],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[(start+2)%8]));

        }else
            tris.push_back(Triangle(p[start+1],ip,p[(start+2)%8]));
    }else
    {
        Edge e(p[start],p[(start+2)%8]);
        if(e.intersects(ip->next)){

            Point pr = e.intersectionPoint(ip->next);
            tris.push_back(Triangle(p[start],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[(start+2)%8]));
        }else if(e.intersects(ip->prev)){

            Point pr = e.intersectionPoint(ip->prev);

            tris.push_back(Triangle(p[start],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[(start+2)%8]));
        }else {
            tris.push_back(Triangle(p[start],ip,p[(start+2)%8]));
        }
    }

}

void QuadTree::applyEdgeTriangulation(QuadTreeNode *n)
{
    Point *p[8];
    p[0]=new Point(n->lu_corner);//nw
    p[1]=new Point(n->lu_corner.x+n->size/2.0,n->lu_corner.y);
    p[2]=new Point(n->lu_corner.x+n->size,n->lu_corner.y);//ne
    p[3]=new Point(n->lu_corner.x+n->size,n->lu_corner.y+n->size/2.0);
    p[4]=new Point(n->lu_corner.x+n->size,n->lu_corner.y+n->size);//se
    p[5]=new Point(n->lu_corner.x+n->size/2.0,n->lu_corner.y+n->size);
    p[6]=new Point(n->lu_corner.x,n->lu_corner.y+n->size);//sw
    p[7]=new Point(n->lu_corner.x,n->lu_corner.y+n->size/2.0);

    int nn=0,ss=0,ww=0,ee=0;
    if( n->N && !n->N->isLeaf() )nn++;
    if( n->S && !n->S->isLeaf() )ss++;
    if( n->E && !n->E->isLeaf() )ee++;
    if( n->W && !n->W->isLeaf() )ww++;

    int sum = nn + ss +ww +ee;

    Edge *e = n->insertedEdge;
    Edge ned(p[0],p[2]);
    Edge sed(p[4],p[6]);
    Edge eed(p[2],p[4]);
    Edge wed(p[0],p[6]);

    Point *p2[2];
    int side[2];
    Point *opposite[2][2];// vertices the opposite border a of square
    Point *same[2][2]; //vertices of the same border

    int i=0;

    if(e->intersects(&ned)){
        p2[i]= new Point(e->intersectionPoint(&ned));
        side[i] = 0;
        opposite[i][0]=p[4];
        opposite[i][1]=p[6];
        same[i][0]=p[2];
        same[i++][1]=p[0];
    }
    if(e->intersects(&eed)){
        p2[i]= new Point(e->intersectionPoint(&eed));
        side[i] = 1;
        opposite[i][0]=p[0];
        opposite[i][1]=p[6];
        same[i][0]=p[2];
        same[i++][1]=p[4];
    }
    if(e->intersects(&sed)){
        p2[i]= new Point(e->intersectionPoint(&sed));
        side[i] = 2;
        opposite[i][0]=p[0];
        opposite[i][1]=p[2];
        same[i][0]=p[6];
        same[i++][1]=p[4];
    }
    if(e->intersects(&wed)){
        p2[i]= new Point(e->intersectionPoint(&wed));
        side[i] = 3;
        opposite[i][0]=p[2];
        opposite[i][1]=p[4];
        same[i][0]=p[0];
        same[i++][1]=p[6];
    }


    if((side[0]+side[1])%2==0){//oposite borders
        tris.push_back(Triangle(p2[0],opposite[0][0],same[0][0]));
        tris.push_back(Triangle(p2[0],opposite[0][1],same[0][1]));

        tris.push_back(Triangle(p2[0],p2[1],opposite[0][0]));
        tris.push_back(Triangle(p2[0],p2[1],opposite[0][1]));
    }else//neigh borders
    {
        Edge a = Edge(p2[1],opposite[0][1]);
        if(Edge(p2[0],opposite[0][0]).intersects(&a)){
            tris.push_back(Triangle(p2[0],p2[1],opposite[0][1]));
            tris.push_back(Triangle(p2[0],opposite[0][1],same[0][1]));
            tris.push_back(Triangle(p2[0],p2[1],same[0][0]));
        }else
        {
            tris.push_back(Triangle(p2[0],p2[1],opposite[0][0]));
            tris.push_back(Triangle(p2[0],opposite[0][0],same[0][0]));
            tris.push_back(Triangle(p2[0],p2[1],same[0][1]));
        }
        tris.push_back(Triangle(p2[1],opposite[0][1],opposite[0][0]));
    }




}

void QuadTree::draw(DrawingArea *area)
{
    area->startStep(1);

    QGraphicsRectItem* quad=new QGraphicsRectItem(QRectF(root->lu_corner.x,root->lu_corner.y,
                                                         root->size,root->size));
    QPen pen(Qt::yellow);
    quad->setPen(pen);
    area->addToQueue(quad);
    root->draw(area);
    area->stopStep();

    area->startStep(1);
    for(int i=0;i<tris.size();++i)
        tris[i].draw(area);
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

    for(it=poly->boundary.constBegin();it!=poly->boundary.constEnd();it++){
        insert(Point((*it)->center().x(),(*it)->center().y()));
    }

    for(int i=0;i<points.size();++i)
    {
        edges.push_back(Edge(&points[i],&points[(i+1)%points.size()]));
        edges.back().setPointsPtr();
        root->insert(&edges.back());
    }

    int a=0;
}

void QuadTree::insert(const Point& p)
{
    points.push_back(p);
    root->insert(&points.back());
}
