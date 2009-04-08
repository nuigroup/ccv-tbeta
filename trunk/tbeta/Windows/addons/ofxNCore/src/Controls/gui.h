#ifndef __GUI_DEFINITION
#define __GUI_DEFINITION

#include "ofxNCoreVision.h"

ofxNCoreVision  *appPtr;

void ofxNCoreVision ::setupGUI()
{
	appPtr = this;

		//panel border color
		gui->mGlobals->mBorderColor.r = 0;
		gui->mGlobals->mBorderColor.g = 0;
		gui->mGlobals->mBorderColor.b = 0;
		gui->mGlobals->mBorderColor.a = .3;
		//panel color
		gui->mGlobals->mCoverColor.r = 1;
		gui->mGlobals->mCoverColor.g = 1;
		gui->mGlobals->mCoverColor.b = 1;
		gui->mGlobals->mCoverColor.a = .4;
		//control outline color
		gui->mGlobals->mFrameColor.r = 0;
		gui->mGlobals->mFrameColor.g = 0;
		gui->mGlobals->mFrameColor.b = 0;
		gui->mGlobals->mFrameColor.a = .3; 
		//text color
		gui->mGlobals->mTextColor.r = 0;
		gui->mGlobals->mTextColor.g = 0;
		gui->mGlobals->mTextColor.b = 0;
		gui->mGlobals->mTextColor.a = 1;
		//button color
		gui->mGlobals->mButtonColor.r = .9;
		gui->mGlobals->mButtonColor.g = 1;
		gui->mGlobals->mButtonColor.b = 0;
		gui->mGlobals->mButtonColor.a = .8;
		//slider tip color
		gui->mGlobals->mHandleColor.r = 0;
		gui->mGlobals->mHandleColor.g = 0;
		gui->mGlobals->mHandleColor.b = 0;
		//slider color
		gui->mGlobals->mSliderColor.r = 1;
		gui->mGlobals->mSliderColor.g = 0;
		gui->mGlobals->mSliderColor.b = 0;
		gui->mGlobals->mSliderColor.a = .8;

		ofxGuiPanel* propPanel = gui->addPanel(appPtr->propertiesPanel, "Source Properties", 735, 10, 12, OFXGUI_PANEL_SPACING);
		propPanel->addButton(appPtr->propertiesPanel_settings, "Camera Settings (v)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
		propPanel->addButton(appPtr->propertiesPanel_flipV, "Flip Vertical (j)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		propPanel->addButton(appPtr->propertiesPanel_flipH, "Flip Horizontal (h)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		propPanel->mObjWidth = 200;

		ofxGuiPanel* gPanel = gui->addPanel(appPtr->gpuPanel, "GPU Properties", 735, 110, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		gPanel->addButton(appPtr->gpuPanel_use, "GPU Mode (g)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		gPanel->mObjWidth = 200;

		ofxGuiPanel* oPanel = gui->addPanel(appPtr->optionPanel, "Communication", 735, 167, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		oPanel->addButton(appPtr->optionPanel_tuio_osc, "Send TUIO OSC (t)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		oPanel->addButton(appPtr->optionPanel_tuio_tcp, "Send TUIO TCP | For Flash (f)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		oPanel->addButton(appPtr->optionPanel_tuio_height_width, "Send Height & Width ", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		oPanel->mObjWidth = 200;

		ofxGuiPanel* cPanel = gui->addPanel(appPtr->calibrationPanel, "Calibration", 735, 264, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		cPanel->addButton(appPtr->calibrationPanel_calibrate, "Enter Calibration (c)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
		cPanel->mObjWidth = 200;

		ofxGuiPanel* panel2 = gui->addPanel(appPtr->savePanel, "Files", 735, 322, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		panel2->addButton(appPtr->kParameter_SaveXml, "Save Settings (s)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
		panel2->mObjWidth = 200;

		//Tracked Image
		ofxGuiPanel* trackPanel = gui->addPanel(appPtr->trackedPanel, "Tracked Image", 386, 270, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		trackPanel->addButton(appPtr->trackedPanel_outlines, "Show Outlines (o)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		trackPanel->addButton(appPtr->trackedPanel_ids, "Show IDs (i)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		trackPanel->addSlider(appPtr->trackedPanel_threshold, "Image Threshold (a/z)", 140, 13, 0.0f, 255.0f, filter->threshold, kofxGui_Display_Int, 0);
		trackPanel->addSlider(appPtr->trackedPanel_min_movement, "Movement Threshold", 140, 13, 0.0f, 15.0f, tracker.MIN_MOVEMENT_THRESHOLD, kofxGui_Display_Int, 0);
		trackPanel->addSlider(appPtr->trackedPanel_min_blob_size, "Min Blob Size", 140, 13, 1.0f, 1000.0f, MIN_BLOB_SIZE, kofxGui_Display_Int, 0);
		trackPanel->addSlider(appPtr->trackedPanel_max_blob_size, "Max Blob Size", 140, 13, 1.0f, 1000.0f, MAX_BLOB_SIZE, kofxGui_Display_Int, 0);
		trackPanel->mObjHeight = 100;
		trackPanel->mObjWidth = 319;
		trackPanel->mObjects[0]->mObjX = 120;
		trackPanel->mObjects[0]->mObjY = 11;
		trackPanel->mObjects[1]->mObjX = 235;
		trackPanel->mObjects[1]->mObjY = 11;
		trackPanel->mObjects[2]->mObjY = 32;		
		trackPanel->mObjects[3]->mObjX = 165;
		trackPanel->mObjects[3]->mObjY = 32;
		trackPanel->mObjects[4]->mObjY = 67;
		trackPanel->mObjects[5]->mObjX = 165;
		trackPanel->mObjects[5]->mObjY = 67;
		trackPanel->adjustToNewContent(100, 0);

		//Source Image
		ofxGuiPanel* srcPanel = gui->addPanel(appPtr->sourcePanel, "Source Image", 41, 270, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		srcPanel->addButton(appPtr->sourcePanel_cam, "Use Camera", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		srcPanel->addButton(appPtr->sourcePanel_previousCam, "Previous Camera", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
		srcPanel->addButton(appPtr->sourcePanel_nextCam, "Next Camera", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
		srcPanel->addButton(appPtr->sourcePanel_video, "Use Video", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		srcPanel->mObjHeight = 85;
		srcPanel->mObjWidth = 319;
		srcPanel->mObjects[1]->mObjX = 100;
		srcPanel->mObjects[1]->mObjY = 32;
		srcPanel->mObjects[2]->mObjX = 214;
		srcPanel->mObjects[2]->mObjY = 32;
		srcPanel->mObjects[3]->mObjY = 55;
		srcPanel->adjustToNewContent(100, 0);

		//Background Image
		ofxGuiPanel* bkPanel1 = gui->addPanel(appPtr->backgroundPanel, "Background", 86, 487, 10, 7);
		bkPanel1->addButton(backgroundPanel_remove, "Remove BG (b)", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
		bkPanel1->addButton(backgroundPanel_dynamic, "Dynamic Subtract", 10, 10, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		bkPanel1->addSlider(appPtr->backgroundPanel_learn_rate, "Learn Speed", 110, 13, 1.0f, 500.0f, backgroundLearnRate, kofxGui_Display_Int, 0);
		bkPanel1->mObjWidth = 127;
		bkPanel1->mObjHeight = 95;

		//Smooth Image
		ofxGuiPanel* sPanel = gui->addPanel(appPtr->smoothPanel, "Smooth", 236, 487, 10, 7);
		sPanel->addButton(smoothPanel_use, "", 12, 12, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		sPanel->addSlider(smoothPanel_smooth, "Smooth", 110, 13, 0.0f, 15.0f, filter->smooth, kofxGui_Display_Int, 0);
		sPanel->mObjects[0]->mObjX = 105;
		sPanel->mObjects[0]->mObjY = 10;
		sPanel->mObjects[1]->mObjY = 30;
		sPanel->mObjWidth = 127;
		sPanel->mObjHeight = 65;

		//Highpass Image
		ofxGuiPanel* hpPanel = gui->addPanel(appPtr->highpassPanel, "Highpass", 386, 487, OFXGUI_PANEL_BORDER, 7);
		hpPanel->addButton(highpassPanel_use, "", 12, 12, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		hpPanel->addSlider(highpassPanel_blur, "Blur", 110, 13, 0.0f, 200.0f, filter->highpassBlur, kofxGui_Display_Int, 0);
		hpPanel->addSlider(highpassPanel_noise, "Noise", 110, 13, 0.0f, 30.0f, filter->highpassNoise, kofxGui_Display_Int, 0);
		hpPanel->mObjects[0]->mObjX = 105;
		hpPanel->mObjects[0]->mObjY = 10;
		hpPanel->mObjects[1]->mObjY = 30;
		hpPanel->mObjects[2]->mObjY = 60;
		hpPanel->mObjWidth = 127;
		hpPanel->mObjHeight = 95;

		//Amplify Image
		ofxGuiPanel* ampPanel = gui->addPanel(appPtr->amplifyPanel, "Amplify", 536, 487, OFXGUI_PANEL_BORDER, 7);
		ampPanel->addButton(amplifyPanel_use, "", 12, 12, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		ampPanel->addSlider(amplifyPanel_amp, "Amplify", 110, 13, 0.0f, 300.0f, filter->highpassAmp, kofxGui_Display_Int, 0);
		ampPanel->mObjects[0]->mObjX = 105;
		ampPanel->mObjects[0]->mObjY = 10;
		ampPanel->mObjects[1]->mObjY = 30;
		ampPanel->mObjWidth = 127;
		ampPanel->mObjHeight = 65;

		//do update while inactive
		gui->forceUpdate(false);
		gui->activate(true);

		/****************************
		* Set GUI values on startup
		****************************/
		gui->update(appPtr->propertiesPanel_flipV, kofxGui_Set_Bool, &appPtr->filter->bVerticalMirror, sizeof(bool));
		gui->update(appPtr->propertiesPanel_flipH, kofxGui_Set_Bool, &appPtr->filter->bHorizontalMirror, sizeof(bool));
		gui->update(appPtr->trackedPanel_outlines, kofxGui_Set_Bool, &appPtr->bDrawOutlines, sizeof(bool));
		gui->update(appPtr->trackedPanel_ids, kofxGui_Set_Bool, &appPtr->bShowLabels, sizeof(bool));
		//Source
		gui->update(appPtr->sourcePanel_cam, kofxGui_Set_Bool, &appPtr->bcamera, sizeof(bool));
		if(!bcamera){
		bool bvideo = true;
		gui->update(appPtr->sourcePanel_video, kofxGui_Set_Bool, &bvideo, sizeof(bool));
		}
		//Calibration
		gui->update(appPtr->calibrationPanel_calibrate, kofxGui_Set_Bool, &appPtr->bCalibration, sizeof(bool));
		//Dynamic Background
		gui->update(appPtr->backgroundPanel_dynamic, kofxGui_Set_Bool, &appPtr->filter->bDynamicBG, sizeof(bool));
		//Smooth
		gui->update(appPtr->smoothPanel_use, kofxGui_Set_Bool, &appPtr->filter->bSmooth, sizeof(bool));
		gui->update(appPtr->smoothPanel_smooth, kofxGui_Set_Bool, &appPtr->filter->smooth, sizeof(float));
		//Highpass
		gui->update(appPtr->highpassPanel_use, kofxGui_Set_Bool, &appPtr->filter->bHighpass, sizeof(bool));
		gui->update(appPtr->highpassPanel_blur, kofxGui_Set_Bool, &appPtr->filter->highpassBlur, sizeof(float));
		gui->update(appPtr->highpassPanel_noise, kofxGui_Set_Bool, &appPtr->filter->highpassNoise, sizeof(float));
		//Amplify
		gui->update(appPtr->amplifyPanel_use, kofxGui_Set_Bool, &appPtr->filter->bAmplify, sizeof(bool));
		gui->update(appPtr->amplifyPanel_amp, kofxGui_Set_Bool, &appPtr->filter->highpassAmp, sizeof(float));
		//Threshold
		gui->update(appPtr->trackedPanel_threshold, kofxGui_Set_Bool, &appPtr->filter->threshold, sizeof(float));
		//Min Movement
		gui->update(appPtr->trackedPanel_min_movement, kofxGui_Set_Bool, &appPtr->tracker.MIN_MOVEMENT_THRESHOLD, sizeof(float));
		//Min Blob Size
		gui->update(appPtr->trackedPanel_min_blob_size, kofxGui_Set_Bool, &appPtr->MIN_BLOB_SIZE, sizeof(float));
		//Max Blob Size
		gui->update(appPtr->trackedPanel_max_blob_size, kofxGui_Set_Bool, &appPtr->MAX_BLOB_SIZE, sizeof(float));
		//Background Learn Rate
		gui->update(appPtr->backgroundPanel_learn_rate, kofxGui_Set_Bool, &appPtr->backgroundLearnRate, sizeof(float));
		//Send TUIO
		gui->update(appPtr->optionPanel_tuio_osc, kofxGui_Set_Bool, &appPtr->myTUIO.bOSCMode, sizeof(bool));
		gui->update(appPtr->optionPanel_tuio_tcp, kofxGui_Set_Bool, &appPtr->myTUIO.bTCPMode, sizeof(bool));
		//TUIO Height Width
		gui->update(appPtr->optionPanel_tuio_height_width, kofxGui_Set_Bool, &appPtr->myTUIO.bHeightWidth, sizeof(bool));
		//GPU Mode
		gui->update(appPtr->gpuPanel_use, kofxGui_Set_Bool, &appPtr->bGPUMode, sizeof(bool));
}

void ofxNCoreVision ::handleGui(int parameterId, int task, void* data, int length)
{
	if(activeInput)
	{
		switch(parameterId)
		{
			case sourcePanel_cam:
				if(length == sizeof(bool))
				{
					if(*(bool*)data)
					{
						if(!bcamera){
							activeInput = false; //this stops the app from doing everything when changing source
							bcamera = true;
							if(deviceID > vidGrabber->getDeviceCount())
							{
								//vidGrabber.close();
								ffmv.setDeviceID(deviceID);
							}
							else{
								vidGrabber->close();
								vidGrabber->setDeviceID(deviceID);
								vidGrabber->setVerbose(false);
								camWidth = vidGrabber->width;
								camHeight = vidGrabber->height;
								vidGrabber->initGrabber(camWidth,camHeight);
							}
							//reset gpu textures and filters
							processedImg.allocate(camWidth, camHeight); //Processed Image
							processedImg.setUseTexture(false);
							sourceImg.allocate(camWidth, camHeight);    //Source Image
							sourceImg.setUseTexture(false);
							filter->allocate(camWidth, camHeight);
							activeInput = true;		//set to active again
							//Turn off the video button;
							bool setBool = false;
							gui->update(sourcePanel_video, kofxGui_Set_Bool, &setBool, length);
						}
					}
				}
				break;
			case sourcePanel_video:
				if(length == sizeof(bool))
				{
					if(*(bool*)data)
					{
						if(bcamera){
							activeInput = false; //this stops the app from doing everything when changing source
							bcamera = false;
							vidPlayer->loadMovie(videoFileName);
							vidPlayer->play();
							printf("Video Mode\n");
							camHeight = vidPlayer->height;
							camWidth = vidPlayer->width;
							//reset gpu textures and filters
							processedImg.allocate(camWidth, camHeight); //Processed Image
							processedImg.setUseTexture(false);
							sourceImg.allocate(camWidth, camHeight);    //Source Image
							sourceImg.setUseTexture(false);
							filter->allocate(camWidth, camHeight);
							activeInput = true;
							//Turn off the camera button;
							bool setBool = false;
							gui->update(sourcePanel_cam, kofxGui_Set_Bool, &setBool, length);
						}
					}
				}
				break;
			case sourcePanel_nextCam:
				if(length == sizeof(bool))
				{
					if(*(bool*)data)
					{
						activeInput = false; //this stops the app from doing everything when changing source

						deviceID += 1;
						if(deviceID >= vidGrabber->getDeviceCount())
						{
							if(deviceID>=(vidGrabber->getDeviceCount()+ffmv.getDeviceCount()))
							{
								deviceID=vidGrabber->getDeviceCount()+ffmv.getDeviceCount();
							}
							else
							{
								vidGrabber->close();
								ffmv.setDeviceID(deviceID);
								filter->exposureStartTime = ofGetElapsedTimeMillis();
							}
							//deviceID = vidGrabber.getDeviceCount();
						}
						else{
							//vidGrabber.close();
							vidGrabber->setDeviceID(deviceID);
							vidGrabber->setVerbose(true);
							vidGrabber->initGrabber(camWidth,camHeight);
							filter->exposureStartTime = ofGetElapsedTimeMillis();
						}
						activeInput = true;
					}
				}
				break;
			case sourcePanel_previousCam:
				if(length == sizeof(bool))
				{
					if(*(bool*)data)
					{
						activeInput = false; //this stops the app from doing everything when changing source

						deviceID -= 1;
						if(deviceID <= vidGrabber->getDeviceCount())
						{
							if(deviceID < 0) deviceID = 0;
							else{
								vidGrabber->close();
								vidGrabber->setDeviceID(deviceID);
								vidGrabber->setVerbose(true);
								vidGrabber->initGrabber(camWidth,camHeight);
								filter->exposureStartTime = ofGetElapsedTimeMillis();
							}
						}
						else
						{
							//vidGrabber.close();
							ffmv.setDeviceID(deviceID);
							filter->exposureStartTime = ofGetElapsedTimeMillis();
						}
						activeInput = true;
					}
				}
				break;
			case propertiesPanel_settings:
				if(length == sizeof(bool))
				{
					if(*(bool*)data && bcamera)
					{
						vidGrabber->videoSettings();
					}
				}
				break;
			//Calibration
			case calibrationPanel_calibrate:
					bCalibration = true;
					bFullscreen = true;
				break;
			//Source
			case propertiesPanel_flipH:
				if(length == sizeof(bool))
					filter->bHorizontalMirror = *(bool*)data;
				break;
			case propertiesPanel_flipV:
				if(length == sizeof(bool))
					filter->bVerticalMirror = *(bool*)data;
				break;
			//GPU
			case gpuPanel_use:
				if(length == sizeof(bool))
					bGPUMode= *(bool*)data;
				break;
			//Communication
			case optionPanel_tuio_osc:
				if(length == sizeof(bool))
					myTUIO.bOSCMode = *(bool*)data;
					bTUIOMode = *(bool*)data;
					//set tcp to opposite
					myTUIO.bTCPMode = false;
					gui->update(appPtr->optionPanel_tuio_tcp, kofxGui_Set_Bool, &appPtr->myTUIO.bTCPMode, sizeof(bool));
					//clear blobs
					myTUIO.blobs.clear();
				break;
			case optionPanel_tuio_tcp:
				if(length == sizeof(bool))
					myTUIO.bTCPMode = *(bool*)data;
					bTUIOMode = *(bool*)data;
					//set osc to opposite
					myTUIO.bOSCMode = false;
					gui->update(appPtr->optionPanel_tuio_osc, kofxGui_Set_Bool, &appPtr->myTUIO.bOSCMode, sizeof(bool));
					//clear blobs
					myTUIO.blobs.clear();
				break;
			case optionPanel_tuio_height_width:
				if(length == sizeof(bool))
					myTUIO.bHeightWidth = *(bool*)data;
				break;				
			//Background
			case backgroundPanel_dynamic:
				if(length == sizeof(bool))
					filter->bDynamicBG = *(bool*)data;
				break;
			case backgroundPanel_remove:
				if(length == sizeof(bool))
					filter->bLearnBakground = *(bool*)data;
				break;
			case backgroundPanel_learn_rate:
				if(length == sizeof(float))
					backgroundLearnRate = *(float*)data;
				break;
			//Highpass
			case highpassPanel_use:
				if(length == sizeof(bool))
					filter->bHighpass = *(bool*)data;
				break;
			case highpassPanel_blur:
				if(length == sizeof(float))
					filter->highpassBlur = *(float*)data;
				break;
			case highpassPanel_noise:
				if(length == sizeof(float))
					filter->highpassNoise = *(float*)data;
				break;
			//Amplify
			case amplifyPanel_use:
				if(length == sizeof(bool))
					filter->bAmplify = *(bool*)data;
				break;
			case amplifyPanel_amp:
				if(length == sizeof(float))
					filter->highpassAmp = *(float*)data;
				break;
			case trackedPanel_threshold:
				if(length == sizeof(float))
					filter->threshold = *(float*)data;
				break;
			case trackedPanel_min_movement:
				if(length == sizeof(float))
					tracker.MIN_MOVEMENT_THRESHOLD = *(float*)data;
				break;
			case trackedPanel_min_blob_size:
				if(length == sizeof(float))
					MIN_BLOB_SIZE = *(float*)data;
				break;
			case trackedPanel_max_blob_size:
				if(length == sizeof(float))
					MAX_BLOB_SIZE = *(float*)data;
				break;
			case trackedPanel_outlines:
				if(length == sizeof(bool))
					bDrawOutlines = *(bool*)data;
				break;
			case trackedPanel_ids:
				if(length == sizeof(bool))
					bShowLabels = *(bool*)data;
				break;
			//smooth
			case smoothPanel_smooth:
				if(length == sizeof(float))
					filter->smooth = *(float*)data;
				break;
			case smoothPanel_use:
				if(length == sizeof(bool))
					filter->bSmooth = *(bool*)data;
				break;
			//Save Settings
			case kParameter_SaveXml:
				if(length == sizeof(bool))
				{
					if(*(bool*)data)
					{
						gui->saveToXml(OFXGUI_XML);
						saveConfiguration();
					}
				}
				break;
		}
	}
}
#endif //__GUI_DEFINITION

