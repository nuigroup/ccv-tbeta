/*
*  camera calibration
*  stefanix - helloworld@stefnix.net
*  augmented by sloopidoopi stefan@schlupek.com
*/

#include "ofCvCameraCalibration.h"

ofCvCameraCalibration::ofCvCameraCalibration (){

	intrinsic_matrix  = cvCreateMat(3,3,CV_32FC1);
	distortion_coeffs = cvCreateMat(4,1,CV_32FC1);
}

void ofCvCameraCalibration::allocate( CvSize _csize, int _nCornersX, int _nCornersY ) {
	std::cout << "allocate:"<< _csize.width << ":" << _csize.height << std::endl;
    csize = _csize;
    nCornersX = _nCornersX;
    nCornersY = _nCornersY;
	 mapx = cvCreateImage( _csize, IPL_DEPTH_32F, 1 );
	 mapy = cvCreateImage(_csize, IPL_DEPTH_32F, 1 );
}


bool ofCvCameraCalibration::addImage( IplImage* img ) {

    vector<ofxPoint2f> points;
    IplImage* bw = cvCreateImage( csize, IPL_DEPTH_8U, 1 );
    cvCvtColor( img, bw, CV_RGB2GRAY );
    
    bool bFound = findCorners( bw, points );
    if( bFound ) {
        //image
        IplImage* color = cvCreateImage( csize, IPL_DEPTH_8U, 3 );
        cvCopy( img, color );
        drawCircles( color, points );
        colorImages.push_back( color );
        
        //screen points
        for( int i=0; i<(nCornersX*nCornersY); ++i ) {
            screenPoints.push_back( ofxPoint2f(points[i].x, points[i].y) );
        }
        
        //world points based on a 8"x8" chess board; assumes z == 0 plane
        for( int x=0; x<nCornersX; ++x ) {
            for( int y=0; y<nCornersY; ++y ) {
                worldPoints.push_back( ofxPoint3f(x,y,0) );
            }
        }
        cvReleaseImage( &bw );
        cout << "Image# " << colorImages.size() << endl;
        return true;
    } else {
        cvReleaseImage( &bw );
        cout << "Finding corners FAILED!" << endl;
        return false;
    }
}




void ofCvCameraCalibration::calibrate() {
    calibrateCamera( colorImages.size(),
                     screenPoints,
                     worldPoints,
                     distortionCoeffs,
                     camIntrinsics,
                     transVectors,
                     rotMatrices );

	createUndistortMap();
}




void ofCvCameraCalibration::undistort() {
	
   // return;
	//convert
    CvVect32f  dist = new float[4];
	CvMatr32f  camM = new float[9];

    for( int i=0; i<4; ++i) {
        dist[i] = distortionCoeffs[i];
    }        
    
    for( int i=0; i<9; ++i) {
        camM[i] = camIntrinsics[i];
    }    
    
	for (int i=0; i<colorImages.size(); i++) {
        undistortedImg.push_back( cvCreateImage(csize, IPL_DEPTH_8U, 3) );                 
      //  cvUnDistortOnce(colorImages[i], undistortedImg[i], camM, dist, 1);
        
        /*
        drawCircles( undistortedImg[i], points );
		DrawCircles( image, &backprojPts2d[iImg*cornersFound[iImg]], 
                    cornersFound[iImg], "Image", 1 );
		DrawCircles( undistort, &backprojPts2d[iImg*cornersFound[iImg]], 
                     cornersFound[iImg], "Undistorted", 1 );
        */

		//IplImage *t = cvCloneImage(colorImages[i]);
		 cvRemap( colorImages[i], undistortedImg[i], mapx, mapy );     // Undistort image
		// cvReleaseImage(&t);
	}
    
    delete dist;
    delete camM;
}

/*--------------ding------------------*/

ofxPoint2f ofCvCameraCalibration::undistortPoints(float _x, float _y) {

/*
cvUndistortPoints( const CvMat* src, CvMat* dst,
                               const CvMat* camera_matrix,
                               const CvMat* dist_coeffs,
                               const CvMat* R CV_DEFAULT(0),
                               const CvMat* P CV_DEFAULT(0));
							   */


   float u0 = camIntrinsics[2],
         v0 = camIntrinsics[5],
         fx = camIntrinsics[0],
         fy = camIntrinsics[4];
   float _fx = 1.0/fx,
         _fy = 1.0/fy;
   float k1 = -distortionCoeffs[0],
         k2 = -distortionCoeffs[1],
         p1 = -distortionCoeffs[2],
         p2 = -distortionCoeffs[3];

   float y         = (_y - v0)*_fy;
   float y2      = y*y;
   float ky      = 1 + (k1 + k2*y2)*y2;
   float k2y      = 2*k2*y2;
   float _2p1y      = 2*p1*y;
   float _3p1y2   = 3*p1*y2;
   float p2y2      = p2*y2;
   
   float x      = (_x - u0)*_fx;
   float x2   = x*x;
   float kx   = (k1 + k2*x2)*x2;
   float d      = kx + ky + k2y*x2;
   float _u   = fx*(x*(d + _2p1y) + p2y2 + (3*p2)*x2) + u0;
   float _v   = fy*(y*(d + (2*p2)*x) + _3p1y2 + p1*x2) + v0;
   
   return ofxPoint2f(_u, _v);   
}

ofxPoint2f ofCvCameraCalibration::distortPoints(float _x, float _y) {
   float R[9];
   float fx = camIntrinsics[0];
   float fy = camIntrinsics[4];
   float cx = camIntrinsics[2];
   float cy = camIntrinsics[5];
   float x = (_x - cx)/fx;
   float y = (_y - cy)/fy;
   
   float r2, r4, a1, a2, a3, cdist;
   float xd, yd;
   
   r2 = x*x + y*y;
   r4 = r2*r2;
   a1 = 2*x*y;
   a2 = r2 + 2*x*x;
   a3 = r2 + 2*y*y;
   cdist   = 1 + distortionCoeffs[0]*r2 + distortionCoeffs[1]*r4;
   xd      = x*cdist + distortionCoeffs[2]*a1 + distortionCoeffs[3]*a2;
   yd      = y*cdist + distortionCoeffs[2]*a3 + distortionCoeffs[3]*a1;
   
   float _u = xd*fx + cx;
   float _v = yd*fy + cy;
   
   return ofxPoint2f(_u, _v);
}


/*-----------------ding------------*/




/*---------sloopidoopi-------------*/

// Build the undistort map which we will use for all 
  // subsequent frames.
  //

void ofCvCameraCalibration::undistortPoints2(float &_x, float &_y){


	CvMat* src_mtr = cvCreateMat(1,2,CV_32FC2);
	
	CV_MAT_ELEM(*src_mtr,float,0,0) = _x;
	CV_MAT_ELEM(*src_mtr,float,0,1) = _y;
	
	//std::cout << "src_mtr:" << CV_MAT_ELEM(*src_mtr,float,0,0) <<":"<< CV_MAT_ELEM(*src_mtr,float,0,1) << std::endl;

	CvMat* dest_mtr = cvCreateMat(1,2,CV_32FC2);
	CV_MAT_ELEM(*dest_mtr,float,0,0) = 0.f;
	CV_MAT_ELEM(*dest_mtr,float,0,1) = 0.f;

	cvUndistortPoints(
		 src_mtr,
		dest_mtr,
		intrinsic_matrix,
		distortion_coeffs,
		NULL,
		intrinsic_matrix
	);//calib.intrinsic_matrix



	//destPoint.x = (CV_MAT_ELEM(*dest_mtr,float,0,0)*1)+0;//cvmGet(dest_mtr,1,1);
	//destPoint.y = (CV_MAT_ELEM(*dest_mtr,float,0,1)*1)+0;//cvmGet(dest_mtr,1,2);

	_x=(CV_MAT_ELEM(*dest_mtr,float,0,0)*1)+0;//cvmGet(dest_mtr,1,1);
	_y = (CV_MAT_ELEM(*dest_mtr,float,0,1)*1)+0;//cvmGet(dest_mtr,1,2);

	cvReleaseMat(&src_mtr);
	cvReleaseMat(&dest_mtr);

}
void ofCvCameraCalibration::createUndistortMap(){
  std::cout << "ofCvCameraCalibration.createUndistortMap" << endl;
  cvInitUndistortMap(
    intrinsic_matrix,
    distortion_coeffs,
    mapx,
    mapy
  );
   std::cout << "ofCvCameraCalibration.createUndistortMap2" << endl;
}
void ofCvCameraCalibration::loadData(){
 // EXAMPLE OF LOADING THESE MATRICES BACK IN:
	std::cout << "ofCvCameraCalibration.loadData" << endl;
  intrinsic_matrix = (CvMat*)cvLoad("Intrinsics.xml");
  distortion_coeffs = (CvMat*)cvLoad("Distortion.xml");

	//convertCameraCalibrationData();
  if(intrinsic_matrix != 0 && distortion_coeffs !=0){
	convertCameraCalibrationData(distortionCoeffs, camIntrinsics);
  }
	//generate Map:
	//createUndistortMap();
  
}

void ofCvCameraCalibration::saveData(){

		cvSave("Intrinsics.xml",intrinsic_matrix);
		cvSave("Distortion.xml",distortion_coeffs);
}
/*----------------------*/

bool ofCvCameraCalibration::findCorners( const IplImage* bw,
                               vector<ofxPoint2f>& points
                             ) const {
    IplImage* tempImg = cvCreateImage( csize, IPL_DEPTH_8U, 1 );
    
	CvPoint2D32f* p = new CvPoint2D32f[nCornersX*nCornersY];
    int nCorners;
    
	/*
    int bPerfect = cvFindChessBoardCornerGuesses( bw, 
                                                  tempImg, 
                                                  NULL, 
                                                  cvSize(nCornersX, nCornersY), 
                                                  p, 
                                                  &nCorners );
												  */
/*---------sloopidoopi-------------*/
	//Find chessboard corners:
	int bPerfect = cvFindChessboardCorners(
		bw, cvSize(nCornersX, nCornersY), p, &nCorners,
		CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS
		);
/*
#ifdef OpenCV3_1
  cvFindChessBoardCornerGuesses(img, m_threshImg, 0, cvSize(NUM_H_CORNERS,NUM_V_CORNERS), corners, &cornerCount );
 #else // OpenCV 5
  cvFindChessboardCorners(img, cvSize(NUM_H_CORNERS,NUM_V_CORNERS), corners, &cornerCount, 0 );
 #endif
*/

//-----------------------------


    cvFindCornerSubPix( bw, 
                        p, 
                        nCorners, 
                        cvSize(5,5), cvSize(-1,-1), 
                        cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 100, 0.1) );
	//cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.1);
	//CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1
    //convert back
    for( int i=0; i<(nCornersX*nCornersY); ++i ) {
        points.push_back( ofxPoint2f(p[i].x,p[i].y) );
    }    

    cvReleaseImage( &tempImg );
    delete p;	    

    if( !bPerfect ) {
        cout << "Did not find expected number of points...\n";
        return false;
    } else {
        return true;
    }
}






void ofCvCameraCalibration::calibrateCamera( const int                 nImages,
                                   const vector<ofxPoint2f>&  _screenPoints,
                                   const vector<ofxPoint3f>&  _worldPoints,
                                   ofxVec4f&                  _distortionCoeffs,
                                   ofxMatrix3x3&              _camIntrinsics,
                                   vector<ofxVec3f>&          _transVectors,
                                   vector<ofxMatrix3x3>&      _rotMatrices
                                 ) const {



//--------------------
	//int n_boards = nImages; //Will be set by input list
	
	//int board_w = 7;
	//int board_h = 7;
	int board_n  = nCornersX * nCornersY;
	CvMat* image_points      = cvCreateMat(nImages*nCornersX*nCornersY,2,CV_32FC1);
	CvMat* object_points     = cvCreateMat(nImages*nCornersX*nCornersY,3,CV_32FC1);
	//CvMat* intrinsic_matrix  = cvCreateMat(3,3,CV_32FC1);
	//CvMat* distortion_coeffs = cvCreateMat(4,1,CV_32FC1);

	CvMat* point_counts      = cvCreateMat(nImages,1,CV_32SC1);

	CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0f;
	CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0f;
//-------------------
  std::cout << "1:------------"<<std::endl;

    //assume all points have been for all images
    int* nCorners = new int[nImages];
    for( int i=0; i<nImages; ++i ) {
        nCorners[i] = nCornersX*nCornersY;

		CV_MAT_ELEM(*point_counts, int,i,0) = nCorners[i];  
    }
    std::cout << "2:------------"<<std::endl;
	CvPoint2D32f* screenP = new CvPoint2D32f[nImages*nCornersX*nCornersY];
    for( int i=0; i<nImages*nCornersX*nCornersY; ++i ) {
        screenP[i].x = _screenPoints[i].x;
        screenP[i].y = _screenPoints[i].y;

		CV_MAT_ELEM(*image_points, float,i,0) = _screenPoints[i].x;
		CV_MAT_ELEM(*image_points, float,i,1) = _screenPoints[i].y;
    } 

   std::cout << "3:------------"<<std::endl;
    
	CvPoint3D32f* worldP = new CvPoint3D32f[nImages*nCornersX*nCornersY];
    for( int i=0; i<nImages*nCornersX*nCornersY; ++i ) {
        worldP[i].x = _worldPoints[i].x;
        worldP[i].y = _worldPoints[i].y;
        worldP[i].z = _worldPoints[i].z;
 //std::cout << "3:------------"<< i<< std::endl;
		CV_MAT_ELEM(*object_points,float,i,0) = _worldPoints[i].x;
        CV_MAT_ELEM(*object_points,float,i,1) = _worldPoints[i].y;
        CV_MAT_ELEM(*object_points,float,i,2) = _worldPoints[i].z;

    }
      std::cout << "4:------------"<<std::endl;
	CvVect32f  distortion = new float[4];
	CvMatr32f  camera_matrix = new float[9];
	CvVect32f  translation_vectors = new float[3*nImages];
    CvMatr32f  rotation_matrices = new float[9*nImages];

	CvMat *  translation_vectors2 =  cvCreateMat(nImages,3,CV_32FC1);// new float[3*nImages];
	CvMat * rotation_matrices2 = cvCreateMat(nImages,9,CV_32FC1);// new float[9*nImages];




//----
 //CALIBRATE THE CAMERA!
	
  cvCalibrateCamera2(
      object_points, image_points,
      point_counts,  csize,
      intrinsic_matrix, distortion_coeffs,
      NULL, NULL,0  //CV_CALIB_FIX_ASPECT_RATIO
  );
  
	/*
   cvCalibrateCamera2(
      object_points, image_points,
      point_counts,  csize,
      intrinsic_matrix, distortion_coeffs,
      rotation_matrices2, translation_vectors2,0  //CV_CALIB_FIX_ASPECT_RATIO
  );
  */
  std::cout << "5:------------"<<std::endl;
 //------------   

  /*
	cvCalibrateCamera( nImages,            //Number of the images.
                       nCorners,             //Array of the number of points in each image.
                       csize,                //Size of the image.
                       screenP,              //Pointer 2D points in screen space. 
                       worldP,               //Pointer 3D points in real space
                       distortion,           //output: 4 distortion coefficients
                       camera_matrix,        //output: intrinsic camera matrix
                       translation_vectors,  //output: Array of translations vectors
		               rotation_matrices,    //output: Array of rotation matrices
                       0 );                  //intrisic guess needed

	*/				   

    delete nCorners;
    delete screenP;
    delete worldP;
    
    
    /*
	//PrintIntrinsics( distortion, camera_matrix );
	//convert coefficients
    _distortionCoeffs.set( distortion[0], distortion[1], 
                           distortion[2], distortion[3] );   
    
    //convert camera matrix
    for( int i=0; i<9; ++i) {
        _camIntrinsics[i] = camera_matrix[i];
    }
	*/
	convertCameraCalibrationData(_distortionCoeffs, _camIntrinsics);
   

    //convert translation vectors
    for( int iImg=0; iImg<nImages; ++iImg ) {
		
        _transVectors.push_back( ofxVec3f(translation_vectors[iImg*3], 
                                         translation_vectors[iImg*3+1], 
                                         translation_vectors[iImg*3+2]) ); 

		/*
		  _transVectors.push_back( ofxVec3f ( 
			  CV_MAT_ELEM( *translation_vectors2, float, iImg, 0 ),
			  CV_MAT_ELEM( *translation_vectors2, float, iImg, 1 ),
			  CV_MAT_ELEM( *translation_vectors2, float, iImg, 2 )
			  ));
			  */
    }
  std::cout << "7:------------"<<std::endl;
    //convert rotation matrices
    for( int iImg=0; iImg<nImages; ++iImg ) {
		
        _rotMatrices.push_back( ofxMatrix3x3(rotation_matrices[iImg*9], 
                                           rotation_matrices[iImg*9+1], 
                                           rotation_matrices[iImg*9+2],
                                           rotation_matrices[iImg*9+3],
                                           rotation_matrices[iImg*9+4],
                                           rotation_matrices[iImg*9+5],
                                           rotation_matrices[iImg*9+6],
                                           rotation_matrices[iImg*9+7],
                                           rotation_matrices[iImg*9+8]) );
										   
		/*
    _rotMatrices.push_back( ofxMatrix3x3(
		CV_MAT_ELEM( *rotation_matrices2, float, iImg, 0 ),
		CV_MAT_ELEM( *rotation_matrices2, float, iImg, 1 ),
		CV_MAT_ELEM( *rotation_matrices2, float, iImg, 2 ),
		CV_MAT_ELEM( *rotation_matrices2, float, iImg, 3 ),
		CV_MAT_ELEM( *rotation_matrices2, float, iImg, 4 ),
		CV_MAT_ELEM( *rotation_matrices2, float, iImg, 5 ),
		CV_MAT_ELEM( *rotation_matrices2, float, iImg, 6 ),
		CV_MAT_ELEM( *rotation_matrices2, float, iImg, 7 ),
		CV_MAT_ELEM( *rotation_matrices2, float, iImg, 8 )
		
		));
*/
    }    
    
	  std::cout << "8:------------"<<std::endl;
    delete distortion;
    delete camera_matrix;
    delete translation_vectors;
    delete rotation_matrices;

	cvReleaseMat(&object_points);
	cvReleaseMat(&image_points);
	cvReleaseMat(&point_counts);
	//cvReleaseMat(&intrinsic_matrix);
	//cvReleaseMat(&distortion_coeffs);
	cvReleaseMat(&rotation_matrices2 );
	cvReleaseMat(&translation_vectors2);

}



void ofCvCameraCalibration::convertCameraCalibrationData(ofxVec4f& _distortionCoeffs,ofxMatrix3x3& _camIntrinsics)const{

 //convert coefficients
	//ofxVec4f&   _distortionCoeffs,ofxMatrix3x3&   _camIntrinsics,
_distortionCoeffs.set(
		CV_MAT_ELEM(*distortion_coeffs,float,0,0) ,
		CV_MAT_ELEM(*distortion_coeffs,float,1,0) ,
		CV_MAT_ELEM(*distortion_coeffs,float,2,0),
		CV_MAT_ELEM(*distortion_coeffs,float,3,0)		
		);
	
     
    //convert camera matrix
    for( int r=0; r<3; ++r) {	
		for( int c=0; c<3; ++c) {
			std::cout << "r:"<<r<<" c:"<< c <<std::endl;
			_camIntrinsics[(r*3)+c] =CV_MAT_ELEM(*intrinsic_matrix,float,r,c);
		}
    }


}

void ofCvCameraCalibration::drawPoint(IplImage* img,ofxPoint2f& point,CvScalar col){
 CvPoint pt;
 pt.x = (int)point.x;
 pt.y = (int)point.y;

cvCircle( img, pt, 1, col, CV_FILLED );

}

void ofCvCameraCalibration::drawCircles( IplImage* img, vector<ofxPoint2f>& points ) {
    CvPoint pt;
	for( int i= 0; i<points.size(); ++i ) {
		int color = (int)( (float)i/points.size()*255.0 );
		pt.x = (int)points[i].x;
		pt.y = (int)points[i].y;
		cvCircle( img, pt, 5, CV_RGB(255-color,0,color), CV_FILLED );
	}
}







/* 
 * Backprojection from World to Image coordinates
 */
CvPoint2D32f* ofCvCameraCalibration::ConvertWorldToPixel( CvPoint3D32f *pts3d, 
                                                int numImages, 
                                                int *numPts, 
                                                CvMatr32f cam, 
                                                CvVect32f t, 
                                                CvMatr32f r) {
	int i, j, k;
	CvPoint2D32f *pts2d    = new CvPoint2D32f[numImages * 49];

	CvMat *C		= cvCreateMat(3, 3, CV_32FC1);
	CvMat *point3D  = cvCreateMat(3, 1, CV_32FC1);
	CvMat *R        = cvCreateMat(3, 3, CV_32FC1);
	CvMat *T        = cvCreateMat(3, 1, CV_32FC1);

	CvPoint3D32f *pts3dCur = pts3d;
	CvPoint2D32f *pts2dCur = pts2d;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
		{
			CV_MAT_ELEM(*C, float, i, j) = cam[3 * i + j];
		}

	for (k = 0; k < numImages; k++) {
		for (j = 0; j < 9; j++) {
			if (j < 3) CV_MAT_ELEM(*T, float, j, 0) = t[k*3 + j];
			CV_MAT_ELEM(*R, float, j / 3, j%3) = r[k*9 + j];
		}

		for (i = 0; i < numPts[k]; i++)
		{	
			CV_MAT_ELEM(*point3D, float, 0, 0) = pts3dCur[i].x;
			CV_MAT_ELEM(*point3D, float, 1, 0) = pts3dCur[i].y;
			CV_MAT_ELEM(*point3D, float, 2, 0) = pts3dCur[i].z;


			cvMatMulAdd(R, point3D, T, point3D); //rot and translate
			cvMatMul(C, point3D, point3D);     //camera 
			
			pts2dCur[i].x = CV_MAT_ELEM(*point3D, float, 0, 0) / CV_MAT_ELEM(*point3D, float, 2, 0);
			pts2dCur[i].y = CV_MAT_ELEM(*point3D, float, 1, 0) / CV_MAT_ELEM(*point3D, float, 2, 0);
		}
		pts3dCur += numPts[k];
		pts2dCur += numPts[k];
	}
	
	cvReleaseMat(&point3D);
	cvReleaseMat(&T);
	cvReleaseMat(&R);

	return pts2d;
}




void ofCvCameraCalibration::printIntrinsics( const CvVect32f& distortion, 
                                   const CvMatr32f& camera_matrix) const {
    if (distortion) {
		cout << "Distortion Coefficients:" << endl;
        for( int i=0; i<4; ++i) {
            cout << distortion[0] << ", ";
        }
        cout << endl;
    }

	if (camera_matrix)	{
		printf("Camera Matrix:\n");
        int m = 0;
		for( int j=0; j<3; ++j ) {
			for( int i=0; i<3; ++i ) {
				cout << camera_matrix[m++] << ", ";
			}
			cout << endl;
		}
	}
}



void ofCvCameraCalibration::PrintIntrinsics( const CvVect32f& _distortion, 
                                   const CvMatr32f& _camera_matrix) const {
	int i;
	if (_distortion)
	{
		printf("Distortion Coefficients:\n");
		printf("%4.10f ", _distortion[0]);
		printf("%4.10f ", _distortion[1]);
		printf("%4.10f ", _distortion[2]);
		printf("%4.10f ", _distortion[3]);
		printf("\n");
	}

	if (_camera_matrix)
	{
		printf("Camera Matrix:\n");
		PrintMatrix(_camera_matrix, 3, 3);
	}
}

void ofCvCameraCalibration::PrintMatrix( const CvMatr32f& matrix, 
                               unsigned int rows, unsigned int cols) const {
	int m, i, j;
	if (matrix)
	{
		m = 0;
		for (j = 0; j < rows; j++)
		{
			for (i = 0; i < cols; i++)
			{
				printf("%4.10f ",matrix[m++]);
			}
			printf("\n");
		}
	}
}


