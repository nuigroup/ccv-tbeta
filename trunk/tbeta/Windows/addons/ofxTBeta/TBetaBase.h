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
#define OF_ADDON_USING_OFXXMLSETTINGS


//height and width of the source/tracked draw window
#define MAIN_WINDOW_HEIGHT 240.0f
#define MAIN_WINDOW_WIDTH  320.0f

#include "ofAddons.h"
#include "ofMain.h"

#include "ofxTBeta.h"
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
			TouchEvents.addListener(this);
			calibration = false;
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


		///this is what we want to happen on say, key presses or draws....


		//some listener stuff:::
		//void keyPressed( int key );
		//void keyReleased( int key );








		//image processing stuff
		void grabFrameToCPU();
		void applyImageFilters();

		void grabFrameToGPU(GLuint target);
		void applyGPUImageFilters();
		void resetGPUTextures();

		void bgCapture(ofxCvGrayscaleImage & _giLive);        //Background Capture
	    void learnBackground( ofxCvGrayscaleImage & _giLive,  //Background Learn (bgCapture and dynamic Bg subtraction
							 ofxCvGrayscaleImage & _grayBg,
							 ofxCvFloatImage & _fiLearn,
							 float _fLearnRate );




		void drawToScreen();
		void drawFingerOutlines();








		//Other Methods
		void loadXMLSettings();								  // Load Settings


        void doCalibration();
		void drawCalibrationBlobs();
		void drawCalibrationPointsAndBox();

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
		int 				blurValue;
		int 				blurGaussianValue;
		int					wobbleThreshold;
		int 				camRate;
		int 				camWidth;
		int 				camHeight;
		int					winWidth;
		int					winHeight;
		int					minWidth;
		int					minHeight;
		int 				snapCounter;
		int					highpassBlur;
		int					highpassNoise;
		int					highpassAmp;
		int					smooth;

		bool				calibration;

		bool				bDrawVideo;
		bool  				bFastMode;
		bool				bShowInterface;
		bool				bShowPressure;
		bool				bLearnBakground;
		bool				bInvertVideo;
		bool				bToggleHelp;
		bool				bDrawOutlines;
		bool				bTUIOMode;
		bool  				bFullscreen;
		bool 				bSnapshot;
		bool 				bCalibration;
		bool				bVerticalMirror;
		bool				bHorizontalMirror;
		bool				bSlimMode;
		bool				bShowLabels;
		bool				bNewFrame;


		bool				bcamera;

		//filters
		bool				bHighpass;
		bool				bAmplify;
		bool				bThreshold;
		bool				bSmooth;
		bool				bDynamicBG;
		bool				bAutoBackground;

		//modes
		bool				bGPUMode;

		/****************************************************
		 *End config.xml variables
		 *****************************************************/

		bool				bW;
		bool				bS;
		bool				bA;
		bool				bD;

		bool				activeInput;

		float				fLearnRate;// rate to learn background

		float				transformedX;
		float				transformedY;


		//FPS variables
		int 					frames;
		int  					fps;
		float					lastFPSlog;
		int						differenceTime;
		int						tuioTime;

		char				eventString[255];
		char				timeString[255];

		//---------------------------------------Fonts
		ofTrueTypeFont		verdana;
		ofTrueTypeFont      sidebarTXT;
		ofTrueTypeFont		calibrationText;
		ofTrueTypeFont		bigvideo;
		ofImage				logo;

		//---------------------------------------Images
		ofImage				background;
		ofImage				menuBG;

		//---------------------------------------Blob Tracker
		BlobTracker			tracker;

		//---------------------------------------Calibration Stuff
		ofImage calibrationParticle;
		bool	bShowTargets;
		float	downColor;
		std::map<int, int> blobcolor;

        string				tmpLocalHost;
        int					tmpPort;


        /****************************************************************
		 *						Private Stuff
		 ****************************************************************/

		string videoFileName;
		int	maxBlobs;

		//---------------------------------------Calibration
		calibrationB calibrate;

		//---------------------------------------Blob Finder
		ofxTBetaCvContourFinder	contourFinder;

		//---------------------------------------Images

		ofxCvColorImage		sourceImg;
		ofxCvGrayscaleImage processedImg;

        ofxCvGrayscaleImage grayImg;
		ofxCvGrayscaleImage grayBg;
		ofxCvGrayscaleImage subtractBg;
		ofxCvGrayscaleImage grayDiff;
		ofxCvGrayscaleImage highpassImg;
		ofxCvGrayscaleImage ampImg;

		//---------------------------------------Pressure Map
		ofxCvColorImage		pressureMap;

		//---------------------------------------Background Subtraction
	    ofxCvFloatImage		fiLearn;

		//---------------------------------------XML Settings Vars (BLOATED)
		ofxXmlSettings		XML;
		ofxXmlSettings		calibrationXML;
		string				xmlStructure;
		string				message;


		//---------------------------------------FOR NETWORK
		TUIOOSC				myTUIO;



		/* 5 things i want to know:

		 1) gene's modified
		 2) method of spreading
		 3) speed of spread
		 4) oncogenes ok?
		 5) which cells are affected
		 */



		//---------------------------------------FOR gpuTracker
		GLuint			gpuSourceTex;
		GLuint			gpuBGTex;

		unsigned char * gpuReadBackBuffer;
		ofxCvColorImage gpuReadBackImage;
		ofxCvGrayscaleImage gpuReadBackImageGS;

		ImageFilter*	contrastFilter;
		ImageFilter*	subtractFilter;
		ImageFilter*	subtractFilter2; //we need 2 because we are showing the output of each
		ImageFilter*	gaussVFilter;
		ImageFilter*	gaussVFilter2;
		ImageFilter*	gaussHFilter;
		ImageFilter*	gaussHFilter2;
		ImageFilter*	threshFilter;


	};

#endif
