/*
*  camera calibration
*  stefanix - helloworld@stefnix.net
*/

#ifndef _OFXCAMERACALIBRATION_H
#define _OFXCAMERACALIBRATION_H

#include <iostream>
#include <vector>
#include "ofMain.h"
//#include "ofCvMain.h"
#include "ofxOpenCv.h"
#include "ofxVectorMath.h"

#include <stdio.h>
#include <stdlib.h>


class ofCvCameraCalibration {
  public:
    
	ofCvCameraCalibration ();
	//~ofCvCameraCalibration ();

    CvSize csize;
  
    //input
    vector<IplImage*> colorImages;    
	int nCornersX;
	int nCornersY;
    vector<ofxPoint2f> screenPoints;    //nCornersX*nCornersY per image
    vector<ofxPoint3f> worldPoints;     //nCornersX*nCornersY per image
    //output
    vector<IplImage*> undistortedImg;     
    ofxVec4f  distortionCoeffs;            //1 per camera
                                          // radial, tangential
                                          //[k1, k2, c1, c2]
    ofxMatrix3x3  camIntrinsics;           //1 per camera
                                          // focal, center
                                          //[fx 0 cx; 0 fy cy; 0 0 1]    
    vector<ofxVec3f>  transVectors;        //1 per image
    vector<ofxMatrix3x3>  rotMatrices;     //1 per image  


	CvMat* intrinsic_matrix;//  = cvCreateMat(3,3,CV_32FC1);
	CvMat* distortion_coeffs;// = cvCreateMat(4,1,CV_32FC1);

	IplImage* mapx ;
	IplImage* mapy ;

 
  
    void allocate( CvSize _csize, int _nCornersX=7, int _nCornersY=7 );

    bool addImage( IplImage* img );
    void calibrate();
    void undistort(); 
    /*ding*/
    ofxPoint2f undistortPoints(float _x, float _y);
    ofxPoint2f distortPoints(float _x, float _y);
     /*ding*/
	/*sloopidoopi*/
	 void undistortPoints2(float &_x, float &_y);
	void convertCameraCalibrationData(ofxVec4f& _distortionCoeffs,ofxMatrix3x3& _camIntrinsics)const;
	void createUndistortMap();
	void loadData();
	void saveData();
	void drawPoint(IplImage* img,ofxPoint2f& point,CvScalar col);
    /*sloopidoopi*/

    bool findCorners( const IplImage* bw,
                      vector<ofxPoint2f>& points
                    ) const;
    void calibrateCamera( const int                   nImages,
                          const vector<ofxPoint2f>&    _screenPoints,
                          const vector<ofxPoint3f>&    _worldPoints,
                          ofxVec4f&                    _distortionCoeffs,
                          ofxMatrix3x3&                _camIntrinsics,
                          vector<ofxVec3f>&            _transVectors,
                          vector<ofxMatrix3x3>&        _rotMatrices
                        ) const;
    void drawCircles( IplImage* img, vector<ofxPoint2f>& points );
    CvPoint2D32f* ConvertWorldToPixel( CvPoint3D32f *pts3d, int numImages, 
                                       int *numPts, CvMatr32f cam, 
                                       CvVect32f t, CvMatr32f r);    
    void printIntrinsics( const CvVect32f& distortion, 
                          const CvMatr32f& camera_matrix ) const;
    void PrintIntrinsics( const CvVect32f& distortion, 
                          const CvMatr32f& camera_matrix ) const;
    void PrintMatrix( const CvMatr32f& matrix, 
                      unsigned int rows, unsigned int cols ) const;

     
};

#endif

