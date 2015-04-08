#ifndef ELLIPSE
#define ELLIPSE

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>


#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp> //This is where actual SURF and SIFT algorithm is located

#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include <QDebug>



using namespace std;
using namespace cv;


void Exctact_Ellipse(InputArray imageIn,  vector<vector<Point> > tab );
void Exctact_Descriptor(InputArray imageInsrc);


bool isInROI(Point p, Point2f roi[]);
double computeProduct(Point p, Point2f a, Point2f b);
#endif // ELLIPSE

