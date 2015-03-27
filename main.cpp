#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>



#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp> //This is where actual SURF and SIFT algorithm is located

using namespace std;
using namespace cv;


#include "surf.h"
#include "ellipse.h"


char* window_name1 = "Canny+contours";
char* window_name2 = "Threshold+contours";
int Threshold;
int const max_lowThreshold = 100;
int kernel_size = 3;
IplImage * img,*imgGray,*imgEdge;
int ratio = 3;
Mat dst, detected_edges;
Mat src, src_gray;
RNG rng(12345);


int threshold_value = 153;
int threshold_type = 0;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;
char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Threshold Value";


char* trackbar_contour_size_min = "Contour Size Value Min";
char* trackbar_contour_size_max = "Contour Size Value Max";
int contour_size_value_min = 130;
int contour_size_max = 2000;
int contour_size_value_max = contour_size_max;



void CannyThreshold(int, void*)
{

  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Reduce noise with a kernel 3x3
  blur( src_gray, detected_edges, Size(3,3) );

  /// Canny detector
  Canny( detected_edges,canny_output, Threshold, Threshold*ratio, kernel_size );


  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);
  src.copyTo( dst, canny_output);
  imshow( window_name1, dst );


  /// Find contours
   findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

   // Draw contours
   Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
   for( int i = 0; i< contours.size(); i++ )
   {
       if (contours[i].size() >= 500)
       {
           RotatedRect temp = fitEllipse(Mat(contours[i]));
           //if ( temp.boundingRect() <= 1.1 * contourArea(contours[i])) {
               {
                    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                    drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
               }

       }
   }


   /// Show in a window
   namedWindow(window_name1, CV_WINDOW_AUTOSIZE );
   imshow( window_name1, drawing );



}


void Exctact_Ellipse(InputArray imageIn,  vector<vector<Point> > tab )
{

       Mat M = imageIn.getMat();
       qDebug() << "taille de l'image " << imageIn.size().width << " x "  << imageIn.size().height ;

       int cols =  M.cols, rows =  M.rows;


       if(M.isContinuous())
       {
           cols *= rows;
           rows = 1;
       }
        qDebug() << "taille de l'image : col = " << cols << " et ligne = "  << rows ;


        const double* Mi = M.ptr<double>(0);
        qDebug() << "element = " << Mi[100] ;
}





void Threshold_Demo( int, void* )
{
  /* 0: Binary
     1: Binary Inverted
     2: Threshold Truncated
     3: Threshold to Zero
     4: Threshold to Zero Inverted
   */

  threshold( src_gray, dst, threshold_value, max_BINARY_value,threshold_type );
  imshow( window_name2, dst );

  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  canny_output=dst;
  /// Find contours
   findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

   // Draw contours
   vector<vector<Point> > GoodContours;
   Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
   for( int i = 0; i< contours.size(); i++ )
   {
       //qDebug() << "Size contours between " <<  contour_size_value_min  << " and " << contour_size_value_max ;
       if ( (contours[i].size() <= contour_size_value_max )  &&  (contours[i].size() >= contour_size_value_min ) )
       {
                    qDebug() << "Taille du contour n°" << i << " = " << contours[i].size() ;
                    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                    drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );

                    //on conserve les contours de taille correctes
                     GoodContours.push_back(contours[i]);


       }

   }
   //Draw best fitting Ellipse for all Goodcontours
   for( int i = 0; i< GoodContours.size(); i++ )
   {
        RotatedRect rect;
        rect = fitEllipse(GoodContours[i]);
        Point2f vertices[4];
        rect.points(vertices);
        for (int i = 0; i < 4; i++)
            line(drawing, vertices[i], vertices[(i+1)%4], Scalar(0,255,255));
    }



   /// Show in a window
   namedWindow("test", CV_WINDOW_AUTOSIZE );
   imshow( "test", drawing );



   //en cours de developement à finir
   vector<vector<Point> > temp;
    Exctact_Ellipse(canny_output,temp);


}



int main(int argc, char *argv[])
{


    // ouverture de l'image input
    src = imread( "../data/varroa/varroa2.jpg");



    /// Create a matrix of the same type and size as src (for dst)
     dst.create( src.size(), src.type() );

     /// Convert the image to grayscale
     cvtColor( src, src_gray, CV_BGR2GRAY );

     /*
     /// Create a window
      namedWindow(  window_name1, CV_WINDOW_AUTOSIZE );
       //Create a Trackbar for user to enter threshold
       createTrackbar( "Threshold for canny edge:", window_name1, &Threshold, max_lowThreshold, CannyThreshold );
       CannyThreshold(0, 0);
       */

        /// Create a window to display results
          namedWindow( window_name2, CV_WINDOW_AUTOSIZE );

          /// Create Trackbar to choose type of Threshold
          createTrackbar( trackbar_type, window_name2, &threshold_type, max_type, Threshold_Demo );
          createTrackbar( trackbar_value,window_name2, &threshold_value, max_value, Threshold_Demo );
          createTrackbar( trackbar_contour_size_min,window_name2, &contour_size_value_min, contour_size_max, Threshold_Demo );
          createTrackbar( trackbar_contour_size_max,window_name2, &contour_size_value_max, contour_size_max, Threshold_Demo );


      Threshold_Demo( 0, 0 );



        waitKey(0);

}



