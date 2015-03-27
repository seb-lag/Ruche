
#include "ellipse.h"



void EdgeDetect(IplImage *pGray,IplImage *pEdge)
{
    if(pGray!=NULL && pEdge!=NULL)
    {
        cvCanny(pGray,pEdge,100,50);
    }
}
