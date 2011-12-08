#include "QuadTree.h"
#include <vector>
#include <list>
#include <queue>
#include <algorithm>
#include "Triangulator.h"
#include "DrawingStep.h"

uint QuadTreeNode::MAX_LEVEL = 1000;

int Edge::getSide(Point *c){
    double res = (b->x-c->x)*(e->y-c->y) -(e->x-c->x)*(b->y-c->y);
    if( res< 0 ) return -1;
    else if (res > 0 ) return 1;
    return 0;
}

bool Edge::intersects(Edge* e)
{
    if( getSide(e->b) == getSide(e->e))return false;
    if( e->getSide(this->b)== e->getSide(this->e)) return false;
    return true;
}
bool Edge::intersects(Point *p,Point *p2)
{
    if( getSide(p) == getSide(p2))return false;
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

    QPen pen(QColor(191,190,255));
    pen.setWidth(2);
    l1->setPen(pen);
    l2->setPen(pen);
    l3->setPen(pen);

    area->addToQueue(l1);
    area->addToQueue(l2);
    area->addToQueue(l3);
}

///QuadTree Node

void QuadTreeNode::initialize(const Point &p,double size,CornerPoint *corners,BorderPoint *borderPoints,Edge *borders,Edge ***crossEdges){

    corners[P_NW] = CornerPoint(p.x,p.y);
    corners[P_NE] = CornerPoint(p.x+size,p.y);
    corners[P_SE] = CornerPoint(p.x+size,p.y+size);
    corners[P_SW] = CornerPoint(p.x,p.y+size);

    borderPoints[P_N] = BorderPoint(p.x+size/2,p.y);
    borderPoints[P_E] = BorderPoint(p.x+size,p.y+size/2);
    borderPoints[P_S] = BorderPoint(p.x+size/2,p.y+size);
    borderPoints[P_W] = BorderPoint(p.x,p.y+size/2);

    borders[P_N] = Edge(&corners[P_NW],&corners[P_NE]);
    borders[P_E] = Edge(&corners[P_NE],&corners[P_SE]);
    borders[P_S] = Edge(&corners[P_SE],&corners[P_SW]);
    borders[P_W] = Edge(&corners[P_SW],&corners[P_NW]);

    corners[P_NW].left = &corners[P_NE];
    corners[P_NW].right = &corners[P_SW];
    corners[P_NW].opposite = &corners[P_SE];

    corners[P_NE].left = &corners[P_SE];
    corners[P_NE].right = &corners[P_NW];
    corners[P_NE].opposite = &corners[P_SW];

    corners[P_SE].left = &corners[P_SW];
    corners[P_SE].right = &corners[P_NE];
    corners[P_SE].opposite = &corners[P_NW];

    corners[P_SW].left = &corners[P_NW];
    corners[P_SW].right = &corners[P_SE];
    corners[P_SW].opposite = &corners[P_NE];

    borderPoints[P_N].left= &corners[P_NE];
    borderPoints[P_N].right= &corners[P_NW];
    borderPoints[P_N].oppositeLeft= &corners[P_SE];
    borderPoints[P_N].oppositeRight= &corners[P_SW];

    borderPoints[P_E].left= &corners[P_SE];
    borderPoints[P_E].right= &corners[P_NE];
    borderPoints[P_E].oppositeLeft= &corners[P_SW];
    borderPoints[P_E].oppositeRight= &corners[P_NW];

    borderPoints[P_S].left= &corners[P_SW];
    borderPoints[P_S].right= &corners[P_SE];
    borderPoints[P_S].oppositeLeft= &corners[P_NW];
    borderPoints[P_S].oppositeRight= &corners[P_NE];

    borderPoints[P_W].left= &corners[P_NW];
    borderPoints[P_W].right= &corners[P_SW];
    borderPoints[P_W].oppositeLeft= &corners[P_NE];
    borderPoints[P_W].oppositeRight= &corners[P_SE];


    *crossEdges = new Edge*[2];
    (*crossEdges)[0] = NULL;
    (*crossEdges)[1] = NULL;
}

QuadTreeNode::QuadTreeNode(const Point& p, double size):lu_corner(p),level(0),size(size),parent(0)
{
    insertedPoint=NULL;
    insertedEdge=NULL;

    N=0;
    S=0;
    W=0;
    E=0;
    initialize(p,size,corners,borderPoints,borders,&crossEdges);

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
    initialize(p,size,corners,borderPoints,borders,&crossEdges);


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

        if(insertedPoint ){
           Edge *e1=insertedPoint->next;
           Edge *e2=insertedPoint->prev;
           insertedPoint=NULL;

           if( e1 )insert(e1);
           if( e2 )insert(e2);
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

    throw "Hahahahhaha! HaHa... Ha...";
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
    if(insertedEdge==e || (insertedPoint && (insertedPoint->next==e || insertedPoint->prev==e) ))return;
    if( isLeaf() && !insertedEdge && !insertedPoint ){
        insertedEdge=e;
        Point *points = findItersectionPoints(e);
        crossEdges[0] = new Edge(&points[0],&points[1]);
    }else{

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

Point * QuadTreeNode::findItersectionPoints(Edge* e)
{
    Point *points = new Point[2];
    int j=0;
    for(int i=0;i<4;i++){
        if(j==2) break;
        if(borders[i].intersects(e))
            points[j++] = borders[i].intersectionPoint(e);
    }
    return points;
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

            //            for(int i=0;i<4;++i){
            //                edg[i].b=&pts[i];
            //                edg[i].e=&pts[(i+1)%4];

            //                if (insertedPoint->next && edg[i].intersects(insertedPoint->next)){
            //                    Point pp=insertedPoint->next->intersectionPoint(&edg[i]);
            //                    QGraphicsRectItem *r = new QGraphicsRectItem(pp.x-5,pp.y-5,10,10);
            //                    r->setPen(pen2);
            //                    area->addToQueue(r);
            //                }

            //                if (insertedPoint->prev && edg[i].intersects(insertedPoint->prev)){
            //                    Point pp=insertedPoint->prev->intersectionPoint(&edg[i]);
            //                    QGraphicsRectItem *r = new QGraphicsRectItem(pp.x-5,pp.y-5,10,10);
            //                    r->setPen(pen2);
            //                    area->addToQueue(r);
            //                }
            //            }


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

        tris.push_back(Triangle(p[7],p[2],p[6]));
        tris.push_back(Triangle(p[6],p[3],p[5]));

        tris.push_back(Triangle(p[7],p[6],p[4]));
        tris.push_back(Triangle(p[4],p[6],p[5]));

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
        }
        if(e.intersects(ip->prev)){
            Point pr = e.intersectionPoint(ip->prev);
            tris.push_back(Triangle(p[start],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[start+1]));
        }
        if( !e.intersects(ip->prev) && !e.intersects(ip->next))
            tris.push_back(Triangle(p[start],ip,p[start+1]));


        Edge e2(p[start+1],p[(start+2)%8]);
        if(e2.intersects(ip->next)){
            Point pr = e2.intersectionPoint(ip->next);
            tris.push_back(Triangle(p[start+1],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[(start+2)%8]));
        }
        if(e2.intersects(ip->prev)){
            Point pr = e2.intersectionPoint(ip->prev);
            tris.push_back(Triangle(p[start+1],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[(start+2)%8]));

        }

        if( !e2.intersects(ip->prev) && !e2.intersects(ip->next))
            tris.push_back(Triangle(p[start+1],ip,p[(start+2)%8]));
    }else
    {
        Edge e(p[start],p[(start+2)%8]);
        if(e.intersects(ip->next)){

            Point pr = e.intersectionPoint(ip->next);
            tris.push_back(Triangle(p[start],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[(start+2)%8]));
        }
        if(e.intersects(ip->prev)){

            Point pr = e.intersectionPoint(ip->prev);

            tris.push_back(Triangle(p[start],ip,new Point(pr)));
            tris.push_back(Triangle(new Point(pr),ip,p[(start+2)%8]));
        }
        if( !e.intersects(ip->prev) && !e.intersects(ip->next)) {
            tris.push_back(Triangle(p[start],ip,p[(start+2)%8]));
        }
    }

}

void QuadTree::applyEdgeTriangulation(QuadTreeNode *n)
{

    Edge *e = n->crossEdges[0];
    BorderPoint* left[4];
    BorderPoint* right[4];

    int left_size=0;
    int right_size=0;

    memset(left,0,4);
    memset(right,0,4);

    int j=0;

    if( n->N && !n->N->isLeaf() )
        if(n->crossEdges[0]->getSide(&n->borderPoints[0])<0){
            left[left_size++] = &n->borderPoints[0];
        }else if(n->crossEdges[0]->getSide(&n->borderPoints[0])>0){
            right[right_size++] = &n->borderPoints[0];
        }
    if( n->E && !n->E->isLeaf() )
        if(n->crossEdges[0]->getSide(&n->borderPoints[1])<0){
            left[left_size++] = &n->borderPoints[1];
        }else if(n->crossEdges[0]->getSide(&n->borderPoints[1])>0){
            right[right_size++] = &n->borderPoints[1];
        }
    if( n->S && !n->S->isLeaf() )
        if(n->crossEdges[0]->getSide(&n->borderPoints[2])<0){
            left[left_size++] = &n->borderPoints[2];
        }else if(n->crossEdges[0]->getSide(&n->borderPoints[2])>0){
            right[right_size++] = &n->borderPoints[2];
        }
    if( n->W && !n->W->isLeaf() )
        if(n->crossEdges[0]->getSide(&n->borderPoints[3])<0){
            left[left_size++] = &n->borderPoints[3];
        }else if(n->crossEdges[0]->getSide(&n->borderPoints[3])>0){
            right[right_size++] = &n->borderPoints[3];
        }
    BorderPoint *p;

    if(left_size>0){
        p= left[0];
        tris.push_back(Triangle(p,e->b,e->e));
        calculateTrianglesForEdgeCase(n->crossEdges[0],p,p->left,p->oppositeLeft,left,left_size);
        calculateTrianglesForEdgeCase(n->crossEdges[0],p,p->oppositeLeft,p->oppositeRight,left,left_size);
        calculateTrianglesForEdgeCase(n->crossEdges[0],p,p->right,p->oppositeRight,left,left_size);
    }
    else
    {
        //no points on this side, find
        for(int i=0;i<4;i++){
            CornerPoint *c = n->corners+i;
            if(e->getSide(c)<0 && e->intersects(c,c->opposite)){

                tris.push_back(Triangle(c,e->b,e->e));
                Edge right_edge = Edge(c->right,c->right->right);
                if(right_edge.getSide(e->b)==0){
                    tris.push_back(Triangle(c,c->right,e->b));
                }else if(right_edge.getSide(e->e)==0){
                    tris.push_back(Triangle(c,c->right,e->e));
                }
                Edge left_edge = Edge(c->left,c->left->left);

                if(left_edge.getSide(e->b)==0){
                    tris.push_back(Triangle(c,c->left,e->b));
                }else if(left_edge.getSide(e->e)==0){
                    tris.push_back(Triangle(c,c->left,e->e));
                }
                break;
            }
        }
    }
    if(right_size>0){
        p = right[0];
        tris.push_back(Triangle(p,e->b,e->e));
        calculateTrianglesForEdgeCase(n->crossEdges[0],p,p->left,p->oppositeLeft,right,right_size);
        calculateTrianglesForEdgeCase(n->crossEdges[0],p,p->oppositeLeft,p->oppositeRight,right,right_size);
        calculateTrianglesForEdgeCase(n->crossEdges[0],p,p->right,p->oppositeRight,right,right_size);
    }else
    {
        //no points on this side, find
        for(int i=0;i<4;i++){
            CornerPoint * c = n->corners+i;
            if(e->getSide(c)>0 && e->intersects(c,c->opposite)){
                tris.push_back(Triangle(c,e->b,e->e));
                Edge right_edge = Edge(c->right,c->right->right);
                if(right_edge.getSide(e->b)==0){
                    tris.push_back(Triangle(c,c->right,e->b));
                }else if(right_edge.getSide(e->e)==0){
                    tris.push_back(Triangle(c,c->right,e->e));
                }
                Edge left_edge = Edge(c->left,c->left->left);

                if(left_edge.getSide(e->b)==0){
                    tris.push_back(Triangle(c,c->left,e->b));
                }else if(left_edge.getSide(e->e)==0){
                    tris.push_back(Triangle(c,c->left,e->e));
                }
                break;
            }

  }
    }



}

void QuadTree::calculateTrianglesForEdgeCase(Edge *e,Point *p,Point *sp,Point *op,BorderPoint **otherBP,int othersSize){
    Edge border = Edge(op,sp);

    //we will search for border point which might be on the border
    Point *p2 = NULL;
    //and edge point which also might be located there
    Point *ep = NULL;
    //and we will check if we are on the same border as one of the edge's end
    for(int i=1;i<othersSize;i++){
        if(border.getSide(otherBP[i])==0){
            p2 = otherBP[i]; //got it!
            break;
        }
    }

    if(border.getSide(e->b)==0){
        ep = e->b;
    }else if(border.getSide(e->e)==0){
        ep = e->e;
    }

    //now lets create triangles with the left border in it
    if(p2 && ep){ //cross point and border point
        tris.push_back(Triangle(p,p2,ep)); //put a triangle between border point, cross point and p
        if(!e->intersects(p,sp))
            tris.push_back(Triangle(p,p2,sp)); //put a triangle between border point, opossite  and p
        else
            tris.push_back(Triangle(p,p2,op));
    }else if(ep && !p2) //cross point but no border point
    {
        if(!e->intersects(p,sp))
            tris.push_back(Triangle(p,ep,sp)); //put triangle between cross point, opposite and p
        else
            tris.push_back(Triangle(p,ep,op)); //put triangle between cross point, opposite and p
    }else if(p2 && !ep){ //no cross point but there is border point
        tris.push_back(Triangle(p,p2,sp)); ///put triangle between border point, same side and p
        tris.push_back(Triangle(p,p2,op)); //put triangle between border point opposite side and p
    }else //empty border
    {
        if(!e->intersects(p,op))//make sure that we won't cross the edge
            tris.push_back(Triangle(p,sp,op));
    }

}

void QuadTree::draw(DrawingArea *area)
{
    area->startStep(DrawingStep::QUADRANT);

    QGraphicsRectItem* quad=new QGraphicsRectItem(QRectF(root->lu_corner.x,root->lu_corner.y,
                                                         root->size,root->size));
    QPen pen(Qt::yellow);
    quad->setPen(pen);
    area->addToQueue(quad);
    root->draw(area);
    area->stopStep();

    area->startStep(DrawingStep::TRIANGLE);
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
