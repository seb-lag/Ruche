#ifndef ELLIPSE
#define ELLIPSE

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp> //This is where actual SURF and SIFT algorithm is located
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>



void EdgeDetect(IplImage *pGray,IplImage *pEdge);


#endif // ELLIPSE

