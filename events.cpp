
#include "events.h"
#include <QDebug>


#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>



#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp> //This is where actual SURF and SIFT algorithm is located

using namespace std;
using namespace cv;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
     if  ( event == EVENT_LBUTTONDOWN )
     {
          cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;

             Mat* rgb = (Mat*)  userdata;
             if (event == CV_EVENT_LBUTTONDOWN)
                 {
                     printf("B %d, V %d, R %d\n",
                     (int)(*rgb).at<Vec3b>(y, x)[0],
                     (int)(*rgb).at<Vec3b>(y, x)[1],
                     (int)(*rgb).at<Vec3b>(y, x)[2]);
                 }


     }
     else if  ( event == EVENT_RBUTTONDOWN )
     {
          cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if  ( event == EVENT_MBUTTONDOWN )
     {
          cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if ( event == EVENT_MOUSEMOVE )
     {
          //qcout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

     }
}

