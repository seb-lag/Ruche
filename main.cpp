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
#define PI 3.14159265


#include "events.h"

 char* nameb1 = "button1";

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
Mat copy_src;
Mat drawing,drawingNB;

int threshold_value = 80;
int threshold_type = 0;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;
char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Threshold Value";


char* trackbar_contour_size_min = "Contour Size Value Min";
char* trackbar_contour_size_max = "Contour Size Value Max";
int contour_size_value_min = 800;
int contour_size_max = 4000;
int contour_size_value_max = 4000;

char* trackbar_contour_ellipse_max  = "Contour Ellipse Max";
int contour_ellipse_value_max= 2100;





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
   namedWindow(window_name1, WINDOW_NORMAL );
   imshow( window_name1, drawing );

    exit(1);

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
   drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
   drawingNB = Mat::zeros( canny_output.size(), CV_8UC1 );

   for( int i = 0; i< contours.size(); i++ )
   {
       //qDebug() << "Size contours between " <<  contour_size_value_min  << " and " << contour_size_value_max ;
       if ( ( contourArea(contours[i]) <= contour_size_value_max )  &&  ( contourArea(contours[i]) >= contour_size_value_min ) )
       {
                    //qDebug() << "Taille du contour n°" << i << " = " << contours[i].size() ;
                    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                    drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
                    drawContours( drawingNB, contours, i, 255 , 2, 8, hierarchy, 0, Point() );

                    //on conserve les contours de taille correctes
                     GoodContours.push_back(contours[i]);


       }

   }

   //Draw best fitting Ellipse for all Goodcontours
   //copy_src.delete_oj();
   src.copyTo( copy_src);
   for( int i = 0; i< GoodContours.size(); i++ )
   {
        RotatedRect rect;
        rect = fitEllipse(GoodContours[i]);
        cv::Rect roi = cv::boundingRect(GoodContours[i]);

        // on affiche que les contours de la bonne couleur
        //calcul de la couleur moyenne dans un contour

        cv::Scalar mean = cv::mean(src(roi));
        qDebug() << "contour " << i << " mean B = " << mean[0] << " mean V= " << mean[1] << " mean = " << mean[2] ;


        //attention la couleur est en BVR
        //la couleur du varroa est environ B=36 V=63 R=94

        double seuilColor = 400;

        int errorColor = pow(mean[0]-36,2)+pow(mean[1]-63,2)+pow(mean[2]-94,2);
        qDebug() << " error color " << errorColor ;
        //if ( errorColor <= seuilColor  )
        //{


        //Pour chaque contour, on extrait les descripteurs SURF et SIFT

        //Exctact_Descriptor(src(roi) );

        //si la taille de l'ellipse est trop grande,
        //on coupe l'ellipse suivant le petit axe et on cherche a nouveau des ellipse dans
        //les 2 zones construites.
        if ( contourArea(GoodContours[i]) >= contour_ellipse_value_max )
        {


            Point2f sommets[4],sommets1[4],sommets2[4];



            vector<Point> goodcontour1;
            vector<Point> goodcontour2;


            rect.points(sommets);
            // affichage zone initiale a coupé en 2
            for (int i = 0; i < 4; i++)
            {
                qDebug() << "zone init point " << i << " x=" << sommets[i].x << " y= " << sommets[i].y  ;
            }

            Point debut ( sommets[0].x , sommets[0].y);
            Point milieu1 ( sommets[0].x + (sommets[1%4].x - sommets[0].x )/2  ,   sommets[0].y + (sommets[1].y - sommets[0].y )/2 ) ;
            Point milieu2 ( sommets[2].x + (sommets[3%4].x - sommets[2].x )/2  ,   sommets[2].y + (sommets[3].y - sommets[2].y )/2 ) ;
            Point width (  (sommets[1%4].x - sommets[0].x )/2  ,   (sommets[1].y - sommets[0].y )/2 ) ;
            Point mili ( sommets[2].x + (sommets[3%4].x - sommets[2].x )/2  ,   sommets[2].y + (sommets[3].y - sommets[2].y )/2 ) ;

              // coupe 1
            qDebug() << "centre  " << i << " x=" << rect.center.x << " y= " << rect.center.y  <<  "angle =" << rect.angle << "cos " << cos(rect.angle  * PI / 180.0) << "sin " << sin(rect.angle  * PI / 180.0)  << "width " << rect.size.width << "height " << rect.size.height ;

            RotatedRect rect1 ( Point2f( rect.center.x - rect.size.height*sin(rect.angle* PI / 180.0)/4 ,   rect.center.y + rect.size.height*cos(rect.angle* PI / 180.0)/4 ) , Size2f(rect.size.width , rect.size.height/2) ,rect.angle ) ;
            rect1.points(sommets1);
            for (int i = 0; i < 4; i++)
            {
                qDebug() << "zone 1 point " << i << " x=" << sommets1[i].x << " y= " << sommets1[i].y  ;
            }

            //coupe 2
            RotatedRect rect2 ( Point2f( rect.center.x + rect.size.height*sin(rect.angle* PI / 180.0)/4 ,   rect.center.y - rect.size.height*cos(rect.angle* PI / 180.0)/4 ) , Size2f(rect.size.width , rect.size.height/2) ,rect.angle ) ;
            rect2.points(sommets2);
            for (int i = 0; i < 4; i++)
            {
                qDebug() << "zone 2 x=" << sommets2[i].x << " y= " << sommets2[i].y  ;
            }

            for (int k =0 ; k < GoodContours[i].size() ; k++ )
            {
                //on parcours tous les points du GoodContours[i]
                 //qDebug() << "contour x= " << GoodContours[i][k].x << " y= " << GoodContours[i][k].y;

                 if ( isInROI(GoodContours[i][k],  sommets1) ) //test si le point du contour est dans la zone 1
                 {
                     goodcontour1.push_back(GoodContours[i][k]);
                 }
                 else  if ( isInROI(GoodContours[i][k],  sommets2) ) //sinon  le point du contour est dans la zone 2
                 {
                     goodcontour2.push_back(GoodContours[i][k]);
                 }


            }



            RotatedRect rect_out1 = fitEllipse(goodcontour1);
            Point2f rect_out1_vertices[4];
            rect_out1.points(rect_out1_vertices);
            for (int i = 0; i < 4; i++)
            {
                line(drawing, rect_out1_vertices[i], rect_out1_vertices[(i+1)%4], Scalar(0,1255,155));
                line(copy_src, rect_out1_vertices[i], rect_out1_vertices[(i+1)%4], Scalar(0,125,125));
                ellipse( copy_src, rect_out1, Scalar(255,255,255), 2, 8 );
            }
            RotatedRect rect_out2 = fitEllipse(goodcontour2);
            Point2f rect_out2_vertices[4];
            rect_out2.points(rect_out2_vertices);
            for (int i = 0; i < 4; i++)
            {
                line(drawing, rect_out2_vertices[i], rect_out2_vertices[(i+1)%4], Scalar(0,1255,155));
                line(copy_src, rect_out2_vertices[i], rect_out2_vertices[(i+1)%4], Scalar(0,125,125));
                ellipse( copy_src, rect_out2, Scalar(255,255,255), 2, 8 );
            }



        }


            Point2f vertices[4];
            rect.points(vertices);
            for (int i = 0; i < 4; i++)
            {
                line(drawing, vertices[i], vertices[(i+1)%4], Scalar(0,255,255));
                line(copy_src, vertices[i], vertices[(i+1)%4], Scalar(0,255,255));
                ellipse( copy_src, rect, Scalar(0,125,255), 2, 8 );
            }
        //}
    }
    qDebug() << "nb varrao " << GoodContours.size() ;

    GoodContours.clear();



   /// Show in a window
   namedWindow("test", WINDOW_NORMAL );
   imshow( "test", drawing );
   /// Show in a window
   namedWindow("Source", WINDOW_NORMAL );
   setMouseCallback("Source", CallBackFunc, &src);
   imshow( "Source", copy_src );



}



int main(int argc, char *argv[])
{



    // ouverture de l'image input
    src = imread( "../data/varroa/varroa2.jpg");
    src = imread( "../data/varroa/varroa/P1020945.JPG");
    //src = imread( "../data/varroa/varroa4.jpg");
    //src = imread( "../data/varroa/lange2.jpg");


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
         namedWindow( window_name2, WINDOW_NORMAL  );

          /// Create Trackbar to choose type of Threshold
          createTrackbar( trackbar_type, window_name2, &threshold_type, max_type, Threshold_Demo );
          createTrackbar( trackbar_value,window_name2, &threshold_value, max_value, Threshold_Demo );
          createTrackbar( trackbar_contour_size_min,window_name2, &contour_size_value_min, contour_size_max, Threshold_Demo );
          createTrackbar( trackbar_contour_size_max,window_name2, &contour_size_value_max, contour_size_max, Threshold_Demo );
          createTrackbar( trackbar_contour_ellipse_max,window_name2, &contour_ellipse_value_max, contour_size_max, Threshold_Demo );
            Threshold_Demo( 0, 0 );




            int nb =  waitKey(0);
            qDebug() << "key pressed is " << nb ;

            while ( nb != 113 ) // appuie sur le caractere 'q' du clavier
            {



                if ( nb == 101) // appuie sur le caractere 'e' du clavier
                {
                    qDebug() << "searching ellipse ..." ;
                    //en cours de developement à finir
                    vector<vector<Point> > temp;
                     //Exctact_Ellipse(drawingNB,temp);
                     temp.clear();
                     qDebug() << "end searching ellipse ..." ;
                }

                if ( nb == 102) // appuie sur le caractere 'f' du clavier
                {
                    qDebug() << "searching SURF destciptor ..." ;
                    //en cours de developement à finir

                     Exctact_Descriptor(src);

                     qDebug() << "end searching SURF destciptor  ..." ;
                }







                if ( nb == 115) // appuie sur le caractere 's' du clavier
                {
                    qDebug() << "save image ..." ;
                    imwrite( "../data/varroa/out/ouput.jpg", copy_src );
                     qDebug() << "end save image ..." ;
                }


                nb =  waitKey(0);
                qDebug() << "key pressed is " << nb ;
            }
}







