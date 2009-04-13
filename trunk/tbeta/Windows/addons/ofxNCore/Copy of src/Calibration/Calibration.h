#ifndef _Calibration_H
#define _Calibration_H
/*
 *  Calibration.h
 *  tbeta
 *
 *  Created by Artem Titoulenko on 2/1/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */

// LOAD CONFIG.XML
#include "ofxXmlSettings.h"

#include "ofMain.h"

#include "calibrationB.h"
#include "../Events/TouchMessenger.h"
#include "../Tracking/ContourFinder.h"
#include "../Tracking/tracking.h"


class Calibration : public TouchListener, public ofSimpleApp {

	public:

		Calibration() {
			TouchEvents.addRAWListener(this);

            ofAddListener(ofEvents.keyPressed, this, &Calibration::_keyPressed);
            ofAddListener(ofEvents.keyReleased, this, &Calibration::_keyReleased);

			calibrating = false;

			calibrating = false;
			bShowTargets = true;
			bW			= false;
			bA			= false;
			bS			= false;
			bD			= false;

			downColor = 0xFF0000;
			arcAngle = 0;
		}

		/****************************************************************
		 *						Public
        ****************************************************************/
		//Basic Methods
		void setup(int _camWidth, int _camHeight, BlobTracker *trackerIn);
		//Key Events
		void _keyPressed(ofKeyEventArgs &e);
		void _keyReleased(ofKeyEventArgs &e);

        //Touch Events
		void RAWTouchDown( Blob b );
		void RAWTouchMoved( Blob b );
		void RAWTouchUp( Blob b );
		void RAWTouchHeld( Blob b);

		void drawFingerOutlines();
        void doCalibration();
		void drawCalibrationBlobs();
		void drawCalibrationPointsAndBox();
		void saveConfiguration();

		void passInContourFinder(int numBlobs, vector<Blob> blobs);
        void passInTracker(BlobTracker *trackerIn);

		ContourFinder	contourFinder;
		bool                calibrating;

		/****************************************************************
		 *						Private
        ****************************************************************/

	private:

        void DrawCircleLoader(double xctr, double yctr, double radius, double startAngle, double endAngle);

        bool				bW;
		bool				bS;
		bool				bA;
		bool				bD;

        int 				camWidth;
		int 				camHeight;
		float               arcAngle;

		//---------------------------------------Fonts
		ofTrueTypeFont		verdana;
		ofTrueTypeFont		calibrationText;

		//---------------------------------------Blob Tracker
		BlobTracker*			tracker;

		//---------------------------------------Calibration Stuff
		ofImage calibrationParticle;
		bool	bShowTargets;
		float	downColor;
		std::map<int, int> blobcolor;

		calibrationB calibrate;

		ofxXmlSettings		calibrationXML;
};

#endif
