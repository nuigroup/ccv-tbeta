/*
*  CalibrationUtils.h
*  
*
*  Created on 2/2/09.
*  Copyright 2009 NUI Group. All rights reserved.
*
*/

#ifndef CALIBRATIONUTILS_H
#define CALIBRATIONUTILS_H

//LOAD CONFIG.XML
#include "ofxXmlSettings.h"
//Used other calibration
#include "rect2d.h"
#include "vector2d.h"

#include "ofCvCameraCalibration.h"

#include "ofxNetwork.h"

class CalibrationUtils
{
	public:

		CalibrationUtils();	//constructor
		~CalibrationUtils();//destructor

		void loadXMLSettings();// Load Settings
		void saveCalibration();// Save Settings
		//Calibration Methods
		void setScreenScale(float s);
		void setScreenBBox(rect2df & bbox);
		void setGrid(int x, int y);
		void setCamRes(int camWidth, int camHeight);
		void initTriangles();
		void computeCameraToScreenMap();
		void cameraToScreenPosition(float &x, float &y);
		void cameraToScreenSpace(float &x, float &y);
		void transformDimension(float &width, float &height);
		void initScreenPoints();
		void initCameraPoints(int camWidth, int camHeight);
		void calculateBox(); //calculates min/max calibration rect
		//---------added by Stefan Schlupek-----------
		void init_UDP_calibrate_connection(string host,int port);
		//-----------
		
		virtual vector2df *getScreenPoints() { return screenPoints; };
		virtual vector2df *getCameraPoints() { return cameraPoints; };

		//! starts calibration
		virtual void beginCalibration();
		//! goes to the next step
		virtual void nextCalibrationStep();
		//! return to the last step
		virtual void revertCalibrationStep();


		



		vector2df* screenPoints;// GRID_X * GRID_Y
		vector2df* cameraPoints;// GRID_X * GRID_Y
		rect2df screenBB;
		rect2df getScreenBBox() { return screenBB; };
		
		//calibration rectangle min/max
		float maxBoxX;
		float minBoxX;
		float maxBoxY;
		float minBoxY;
		//returns if true if inside
		bool isPointInTriangle(vector2df p, vector2df a, vector2df b, vector2df c);
		bool bCalibrating;
		bool bGoToNextStep;
		int  calibrationStep;
		int  GRID_POINTS;
		int  GRID_X;
		int  GRID_Y;
		// returns -1 if none found..
		int findTriangleWithin(vector2df pt);		
		float getScreenScale();

		//---------added by Stefan Schlupek-----------
		//Camera Calibration with openCV
		
		ofCvCameraCalibration camCalib;
		bool test;
	
		//CvMat* intrinsic_matrix;//  = cvCreateMat(3,3,CV_32FC1);
		//CvMat* distortion_coeffs;// = cvCreateMat(4,1,CV_32FC1);
		//IplImage* mapx ;
		//IplImage* mapy ;

		ofxUDPManager	udpConnection;//UDP Server for communication with the MulticamCalibrationGUI-
		


		//----added by Stefan Schlupek

	private:

		vector2df* cameraToScreenMap;
		//set Calibration Points
		int GRID_INDICES;
		int* triangles;	// GRID_X * GRID_Y * 2t * 3i indices for the points
		int	_camWidth;
		int _camHeight;
		bool bscreenPoints;
		bool bcameraPoints;
		ofxXmlSettings		calibrationXML;
		string				xmlStructure;
		string				message;
};
#endif
