
#include "surf.h"





bool findObjectSURF( Mat image1, Mat image2, int hessianValue )
{


    bool objectFound = false;
    float nndrRatio = 0.7f;

       SurfFeatureDetector surf(2500);
       vector< cv::KeyPoint > keypoints1; //keypoints for object
       vector< cv::KeyPoint > keypoints2; //keypoints for scene

       surf.detect(image1, keypoints1);
       surf.detect(image2, keypoints2);

       //Descriptor matrices
       Mat descriptors_object, descriptors_scene;

       SurfDescriptorExtractor extractor;
       extractor.compute( image1, keypoints2, descriptors_scene );
       extractor.compute( image2, keypoints1, descriptors_object );


       //Declering flann based matcher
       FlannBasedMatcher matcher;

       //BFMatcher for SURF algorithm can be either set to NORM_L1 or NORM_L2.
       //But if you are using binary feature extractors like ORB, //instead of NORM_L* you use "hamming"
       //BFMatcher matcher(NORM_L1);

       vector< vector< DMatch >  > matches;
       matcher.knnMatch( descriptors_object, descriptors_scene, matches, 2 ); // find the 2 nearest neighbors
       vector< DMatch > good_matches;
       good_matches.reserve(matches.size());


       for (size_t i = 0; i < matches.size(); ++i)
       {
           if (matches[i].size() < 2)
                       continue;

           const DMatch &m1 = matches[i][0];
           const DMatch &m2 = matches[i][1];

           if(m1.distance <= nndrRatio * m2.distance)
           good_matches.push_back(m1);
       }




       std::vector< Point2f >  obj;
      std::vector< Point2f >  scene;

      for( unsigned int i = 0; i < good_matches.size(); i++ )
      {
          //-- Get the keypoints from the good matches
          obj.push_back( keypoints1[ good_matches[i].queryIdx ].pt );
          scene.push_back( keypoints2[ good_matches[i].trainIdx ].pt );
      }

      Mat H = findHomography( obj, scene, CV_RANSAC );



      //-- Get the corners from the image_1 ( the object to be "detected" )
      std::vector< Point2f > obj_corners(4);
      obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( image1.cols, 0 );
      obj_corners[2] = cvPoint( image1.cols, image1.rows ); obj_corners[3] = cvPoint( 0, image1.rows );
      std::vector< Point2f > scene_corners(4);

      perspectiveTransform( obj_corners, scene_corners, H);

/*
  //-- Draw lines between the corners (the mapped object in the scene - image_2 )
      line( outImg, scene_corners[0] , scene_corners[1],Scalar( 0, 0, 255 ), 2 ); //TOP line
      line( outImg, scene_corners[1] , scene_corners[2], Scalar( 0, 0, 255 ), 2 );
      line( outImg, scene_corners[2] , scene_corners[3], Scalar( 0, 0, 255 ), 2 );
      line( outImg, scene_corners[3] , scene_corners[0] ,Scalar( 0, 0, 255 ), 2 );
*/



  //  return a.exec();
}
