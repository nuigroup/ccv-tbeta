#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#define OF_ADDON_USING_OFXTBETA

#include "ofAddons.h"

class testApp : public ofSimpleApp, public ofxGuiListener, public TBetaBase
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

		/****************************************************************
		*						Public functions
		****************************************************************/

		//Basic Methods
		void setup();
		void update();
		void draw();
		void exit();
		void setupGUI();
	void drawToScreen();

		//Key events
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();

		//Touch Events
		void TouchDown(ofxTBetaCvBlob b);
		void TouchMoved(ofxTBetaCvBlob b);
		void TouchUp(ofxTBetaCvBlob b);
		
		//---------------------------------------GUI
		void		handleGui(int parameterId, int task, void* data, int length);
		ofxGui*		gui;

};
#endif
