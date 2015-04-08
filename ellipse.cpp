
#include "ellipse.h"




/** decide whether point p is in the ROI.
*** The ROI is a rotated rectange whose 4 corners are stored in roi[]
**/
bool isInROI(Point p, Point2f roi[])
{
    double pro[4];
    for(int i=0; i<4; ++i)
    {
        pro[i] = computeProduct(p, roi[i], roi[(i+1)%4]);
    }
    if(pro[0]*pro[2]<0 && pro[1]*pro[3]<0)
    {
        return true;
    }
    return false;
}

/** function pro = kx-y+j, take two points a and b,
*** compute the line argument k and j, then return the pro value
*** so that can be used to determine whether the point p is on the left or right
*** of the line ab
**/
double computeProduct(Point p, Point2f a, Point2f b)
{
    double k = (a.y-b.y) / (a.x-b.x);
    double j = a.y - k*a.x;
    return k*p.x - p.y + j;
}




void Exctact_Descriptor(InputArray In)
{
    Mat img_object1 = imread( "../data/varroa/varroa/varroa.jpg");
    //img_object1 = imread( "../data/varroa/varroa2.jpg");
    Mat img_scene = In.getMat();


    namedWindow("Input ROI", WINDOW_NORMAL );
    imshow( "Input ROI", img_scene );
      waitKey(0);


if( !img_object1.data  )
      { qDebug() <<  " --(!) Error reading images for detect varroa"; exit -1;}

    //-- Step 1: Detect the keypoints using SURF Detector
    qDebug() <<  " Step 1: Detect the keypoints using SURF Detector ";
    int minHessian = 400;

     //SurfFeatureDetector detector( minHessian );
       SiftFeatureDetector detector(  0.05, 5.0 );
     //FastFeatureDetector detector();

     std::vector<KeyPoint> keypoints_object, keypoints_scene;

     detector.detect( img_object1, keypoints_object );
     detector.detect( img_scene, keypoints_scene );

     //-- Step 2: Calculate descriptors (feature vectors)
     qDebug() <<  " Step 2: Calculate descriptors (feature vectors) ";

      //SurfDescriptorExtractor extractor;
      SiftDescriptorExtractor extractor(3.0);

      Mat descriptors_object, descriptors_scene;

      extractor.compute( img_object1, keypoints_object, descriptors_object );
      extractor.compute( img_scene, keypoints_scene, descriptors_scene );

      //-- Show descriptors
         qDebug() <<  " nb keypoints object " << keypoints_object.size() ;
         qDebug() <<  " nb keypoints scene " << keypoints_scene.size() ;

      namedWindow("Key points", WINDOW_NORMAL );
      imshow( "Key points", img_object1 );


      //-- Step 3: Matching descriptor vectors using FLANN matcher
      qDebug() <<  "  Step 3: Matching descriptor vectors using FLANN matcher ";
      //FlannBasedMatcher matcher;
      BFMatcher matcher(NORM_L2);

      std::vector< DMatch > matches;

      matcher.match( descriptors_object, descriptors_scene, matches );


      double max_dist = 0; double min_dist = 100;
      //-- Quick calculation of max and min distances between keypoints
      qDebug() <<  "  Step 4: Quick calculation of max and min distances between keypoints ";
       for( int i = 0; i < descriptors_object.rows; i++ )
       { double dist = matches[i].distance;
         if( dist < min_dist ) min_dist = dist;
         if( dist > max_dist ) max_dist = dist;
       }

        qDebug() << "-- Max dist :" << max_dist ;
       qDebug() << "-- Min dist : " << min_dist ;


       //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
       qDebug() <<  "  Step 5: Draw only \"good\" matches (i.e. whose distance is less than 3*min_dist ) ";
       std::vector< DMatch > good_matches;

       for( int i = 0; i < descriptors_object.rows; i++ )
       { if( matches[i].distance < 3*min_dist )
          { good_matches.push_back( matches[i]); }
       }
        qDebug() <<  " Nb \"good\" matches =  " << good_matches.size()
                     ;
       Mat img_matches;
       drawMatches( img_object1, keypoints_object, img_scene, keypoints_scene,
                    good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                    vector<char>() );
        /*
       //-- Localize the object
       std::vector<Point2f> obj;
       std::vector<Point2f> scene;

       for( int i = 0; i < good_matches.size(); i++ )
       {
         //-- Get the keypoints from the good matches
         obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
         scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
       }

       Mat H = findHomography( obj, scene, CV_RANSAC );

       //-- Get the corners from the image_1 ( the object to be "detected" )
       std::vector<Point2f> obj_corners(4);
       obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object1.cols, 0 );
       obj_corners[2] = cvPoint( img_object1.cols, img_object1.rows ); obj_corners[3] = cvPoint( 0, img_object1.rows );
       std::vector<Point2f> scene_corners(4);

       perspectiveTransform( obj_corners, scene_corners, H);

       //-- Draw lines between the corners (the mapped object in the scene - image_2 )
       line( img_matches, scene_corners[0] + Point2f( img_object1.cols, 0), scene_corners[1] + Point2f( img_object1.cols, 0), Scalar(0, 255, 0), 4 );
       line( img_matches, scene_corners[1] + Point2f( img_object1.cols, 0), scene_corners[2] + Point2f( img_object1.cols, 0), Scalar( 0, 255, 0), 4 );
       line( img_matches, scene_corners[2] + Point2f( img_object1.cols, 0), scene_corners[3] + Point2f( img_object1.cols, 0), Scalar( 0, 255, 0), 4 );
       line( img_matches, scene_corners[3] + Point2f( img_object1.cols, 0), scene_corners[0] + Point2f( img_object1.cols, 0), Scalar( 0, 255, 0), 4 );

       */

       //-- Show detected matches
       namedWindow("Good Matches & Object detection", WINDOW_NORMAL );
       imshow( "Good Matches & Object detection", img_matches );
         waitKey(0);

}




























void Exctact_Ellipse(InputArray imageIn,  vector<vector<Point> > tab )
{

       Mat M = imageIn.getMat();
       //qDebug() << "taille de l'image " << imageIn.size().width << " x "  << imageIn.size().height ;

       int cols =  M.cols, rows =  M.rows;
       int taille = cols*rows;

      /* if(M.isContinuous())
       {
           cols *= rows;
           rows = 1;
       }
*/
        qDebug() << "taille de l'image : col = " << cols << " et ligne = "  << rows ;
        qDebug() << "size of M = " <<  M.size().height  << " x "<<  M.size().width;







        //on compte les pixels blanc de l'image
        // et on construit un tableau 1D
        vector<bool> Img_Binary_1d;
        int count=0;
        for (uint i=0;i<M.rows;i++)
        {
            for (uint j=0;j<M.cols;j++)
            {
                bool pixel = M.at<uchar>(i,j);
                Img_Binary_1d.push_back(pixel);
                //pour chaque pixel blanc
                if ( pixel != 0) {
                    count++;
                    //qDebug() << " image(" << i << ")(" << j << ")="  << M.at<uchar>(i,j);
                }
          }
        }


        int count2=0;

        //On recherche les ellipses



        for (int i=0;i<=Img_Binary_1d.size();i++)
        {
            bool pixel1 = Img_Binary_1d[i];

                //pour chaque pixel blanc
                if ( pixel1 != 0)
                {
                    int x_1= i% cols ;
                    int y_1= i/ cols;
                    //qDebug() << " x1= " << x_1 << " y1= " << y_1 ;

                    //on boucle autour du voisinage de x_1,y_1
                    int k_l=10; // nombre de lignes du voisinage
                    int k_c=20; // nombre de colones du voisinage

                    for (int k= -k_l ;k<=k_l  ;k++)
                    {
                        int borne_inf=  std::max(0, (i-k_c)+(k*cols) ) ;
                        int borne_sup= std::max(0,std::min(taille, i+k_c+k*cols ));
                        //qDebug() << " borne inf " << borne_inf << " borne sup " << borne_sup;
                        for (uint j=borne_inf ; j<=borne_sup  ;j++)
                        {
                            bool pixel2 = Img_Binary_1d[j];
                            //pour chaque pixel blanc
                            if ( pixel2 != 0)
                            {
                                int x_2= j% cols ;
                                int y_2= j/ cols;
                                //qDebug() << " x2= " << x_2 << " y2= " << y_2 ;

                                //si distance (en pixel) entre x_1,y_1 et x_2,y_2
                                double distance =  sqrt(  (x_2-x_1)^2+(y_2-y_1)^2 ) ;
                                //qDebug() << " distance = " << distance ;

                                if  ( ( distance >= 30 ) && ( distance <= 50 ) )
                                {
                                    count2++;
                                    //qDebug() << " distance = " << distance ;

                                    //calcul de centre de l'ellipse
                                    int x_0=(x_1+x_2)/2;
                                    int y_0=(y_1+y_2)/2;
                                    double a= sqrt(  (x_2-x_1)^2+(y_2-y_1)^2 )/2.0  ; //c'est distance/2 !
                                    double alpha = std::atan(  (x_2-x_1)/(y_2-y_1) );

                                    //pour chaque troisième pixel
                                    for (int l=0;l<=Img_Binary_1d.size();l++)
                                    {
                                        bool pixel3 = Img_Binary_1d[l];

                                            //pour chaque pixel blanc
                                            if ( pixel3 != 0)
                                            {
                                                int x= l% cols ;
                                                int y= l/ cols;

                                                double d= sqrt(  (x_0-x)^2+(y_0-y)^2 );
                                                double tau = acos( pow(a,2)+pow(d,2) );
                                                double b= (pow(a,2)*pow(d,2)*pow(sin(tau),2)  ) / ( 2 );
                                            }
                                    }

                                }
                            }

                        }
                        //qDebug() << " next pixel 2";

                    }
                    //qDebug() << " next pixel 1 ";

                }


        }

        Img_Binary_1d.clear();


        qDebug() << " Nb pixels total = "  << (rows)*(cols);
        qDebug() << " Nb pixels blancs = "  << count;
        qDebug() << " pourcentage de pixel blanc = "  << (double)count/((rows)*(cols)) *100 ;
        qDebug() << " Nb pixels blancs = "  <<  countNonZero(M);

        qDebug() << " Nb distance good= "  << count2;






        /// Show in a window
        namedWindow("temp", WINDOW_NORMAL );
        //Mat roi(M, Rect(10,10,100,100));
        imshow( "temp",M );

}
