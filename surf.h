#ifndef SURF_H
#define SURF_H

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp> //This is where actual SURF and SIFT algorithm is located
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;

bool findObjectSURF( Mat image1, Mat image2, int hessianValue );



#endif // SURF_H

