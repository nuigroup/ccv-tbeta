#ifndef _TBETABASE_H
#define _TBETABASE_H
/*
 *  TBetaBase.h
 *  tbeta
 *
 *  Created by Artem Titoulenko on 2/1/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */
#define OF_ADDON_USING_OFXXMLSETTINGS  // LOAD CONFIG.XML
#define OF_ADDON_USING_OFXOPENCV	   // COMPUTER VISION STUFF
#define OF_ADDON_USING_OFXOSC		   // OSC COMMUNICATION
#define OF_ADDON_USING_OFXDIRLIST
#define OF_ADDON_USING_OFXVECTORMATH

//height and width of the source/tracked draw window
#define MAIN_WINDOW_HEIGHT 240.0f
#define MAIN_WINDOW_WIDTH  320.0f

#include "ofAddons.h"
#include "ofMain.h"

#include "ofxTBeta.h"

#include "Filters/CPUImageFilter.h"
#include "Filters/ProcessFilters.h"

#include "Calibration/Calibration.h"

//#include "ConfigurationApp.h"


class TBetaBase : public ofSimpleApp, public ofxGuiListener, public TouchListener//, public BlobManager
	{
		//ofxGUI setup stuff
		enum
		{
			propertiesPanel,
			propertiesPanel_flipV,
			propertiesPanel_flipH,
			propertiesPanel_settings,
			propertiesPanel_pressure,

			gpuPanel,
			gpuPanel_use,

			optionPanel,
			optionPanel_tuio,

			calibrationPanel,
			calibrationPanel_calibrate,
			calibrationPanel_warp,

			sourcePanel,
			sourcePanel_cam,
			sourcePanel_nextCam,
			sourcePanel_previousCam,
			sourcePanel_video,

			backgroundPanel,
			backgroundPanel_remove,
			backgroundPanel_dynamic,

			smoothPanel,
			smoothPanel_use,
			smoothPanel_smooth,

			amplifyPanel,
			amplifyPanel_use,
			amplifyPanel_amp,

			highpassPanel,
			highpassPanel_use,
			highpassPanel_blur,
			highpassPanel_noise,

			trackedPanel,
			trackedPanel_use,
			trackedPanel_threshold,
			trackedPanel_outlines,
			trackedPanel_ids,

			savePanel,
			kParameter_SaveXml,
			kParameter_File,
		};

	public:

		TBetaBase() {
			//listen for Touch Events
			TouchEvents.addListener(this);
			//initialize filter
			filter = NULL; 
			//fps and dsp calculation
			frames		= 0;
			fps			= 0;
			lastFPSlog	= 0;
			differenceTime = 0;
			//bools
			bCalibration= 0;
			bDrawVideo = 1;
			bFullscreen = 0;
			bcamera = 0;	
			bShowLabels = 1;
			bFastMode = 0;
			bDrawOutlines = 1;
			bGPUMode = 0;
			bTUIOMode = 0;
			showConfiguration = 0;	
			//camera
			camRate = 30;
			camWidth = 320;
			camHeight = 240;
		} 

		~TBetaBase(){
			    
			if( filter != NULL ) {
				delete filter;
			}
		}

		/****************************************************************
		 *						Public functions
		 ****************************************************************/
		//Basic Methods
		void setup();
		void update();
		void draw();
		void exit();
		void setupGUI();

		//Key events
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();

		//---------------------------------------GUI
		void		handleGui(int parameterId, int task, void* data, int length);
		ofxGui*		gui;

		////////this is the main stuff

		void TouchDown( ofxTBetaCvBlob b );
		void TouchMoved( ofxTBetaCvBlob b );
		void TouchUp( ofxTBetaCvBlob b );

		//image processing stuff
		void grabFrameToCPU();
		void grabFrameToGPU(GLuint target);

		void drawToScreen();
		void drawFingerOutlines();

		//Other Methods
		void loadXMLSettings();								  // Load Settings
		void saveConfiguration();

		/***************************************************************
		 *						Video Settings
		 ***************************************************************/
		ofVideoGrabber 		vidGrabber;
		ofVideoPlayer 		vidPlayer;

		/****************************************************************
		 *            Variables in config.xml Settings file
		 *****************************************************************/
	    int					deviceID;
		int 				frameseq;
		int 				threshold;
		int					wobbleThreshold;
		int 				camRate;
		int 				camWidth;
		int 				camHeight;
		int					winWidth;
		int					winHeight;
		int					minWidth;
		int					minHeight;

		bool				showConfiguration;

		bool				bcamera;
		bool				bDrawVideo;
		bool  				bFastMode;
		bool				bShowInterface;
		bool				bShowPressure;
		bool				bDrawOutlines;
		bool				bTUIOMode;
		bool  				bFullscreen;
		bool 				bCalibration;
		bool				bVerticalMirror;
		bool				bHorizontalMirror;
		bool				bShowLabels;
		bool				bNewFrame;
		//filters
		bool				bAutoBackground;
		//modes
		bool				bGPUMode;

		/****************************************************
		 *End config.xml variables
		 *****************************************************/
		bool				activeInput;

		//FPS variables
		int 					frames;
		int  					fps;
		float					lastFPSlog;
		int						differenceTime;
		int						tuioTime;

		//---------------------------------------Fonts
		ofTrueTypeFont		verdana;
		ofTrueTypeFont      sidebarTXT;
		ofTrueTypeFont		bigvideo;

		//---------------------------------------Images
		ofImage				background;

		//---------------------------------------Blob Tracker
		BlobTracker			tracker;

        string				tmpLocalHost;
        int					tmpPort;

        //---------------------------------------Blob Tracker
        Filters*  filter;

        /****************************************************************
		 *						Private Stuff
    	 ****************************************************************/
		string videoFileName;
		int	maxBlobs;

        Calibration calib;

		//---------------------------------------Blob Finder
		ofxTBetaCvContourFinder	contourFinder;

		//---------------------------------------Images
 		CPUImageFilter      processedImg;
		ofxCvColorImage		sourceImg;

		//---------------------------------------XML Settings Vars (BLOATED)
		ofxXmlSettings		XML;
		string				message;

		//---------------------------------------FOR NETWORK
		TUIOOSC				myTUIO;
	};

#endif
