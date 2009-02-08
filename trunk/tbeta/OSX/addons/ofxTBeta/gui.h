#ifndef __GUI_DEFINITION
#define __GUI_DEFINITION

#include "TBetaBase.h"

TBetaBase  *appPtr;

void TBetaBase ::setupGUI()
{
	appPtr = this;

	//if(!gui->buildFromXml(OFXGUI_XML))
	//{
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

		ofxGuiPanel* gPanel = gui->addPanel(appPtr->gpuPanel, "GPU Properties", 735, 114, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		gPanel->addButton(appPtr->gpuPanel_use, "GPU Mode (g)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		gPanel->mObjWidth = 200;

		ofxGuiPanel* oPanel = gui->addPanel(appPtr->optionPanel, "Communication", 735, 177, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		oPanel->addButton(appPtr->optionPanel_tuio, "Send TUIO (t)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		oPanel->mObjWidth = 200;

		ofxGuiPanel* cPanel = gui->addPanel(appPtr->calibrationPanel, "Calibration", 735, 240, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		cPanel->addButton(appPtr->calibrationPanel_calibrate, "Enter Calibration (c)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
		cPanel->mObjWidth = 200;

		ofxGuiPanel* panel2 = gui->addPanel(appPtr->savePanel, "files", 735, 303, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		//savePanel->addFiles(kParameter_File, "files", 110, OFXGUI_FILES_HEIGHT, "", "", "xml");
		panel2->addButton(appPtr->kParameter_SaveXml, "Save Settings (s)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
		panel2->mObjWidth = 200;

		//Tracked Image
		ofxGuiPanel* trackPanel = gui->addPanel(appPtr->trackedPanel, "Tracked Image", 386, 270, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
		trackPanel->addButton(appPtr->trackedPanel_outlines, "Show Outlines (o)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		trackPanel->addButton(appPtr->trackedPanel_ids, "Show IDs (i)", OFXGUI_BUTTON_HEIGHT, OFXGUI_BUTTON_HEIGHT, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		trackPanel->addSlider(appPtr->trackedPanel_threshold, "Threshold (a/z)", 300, 13, 0.0f, 255.0f, threshold, kofxGui_Display_Int, 0);
		trackPanel->mObjHeight = 85;
		trackPanel->mObjWidth = 319;
		trackPanel->mObjects[1]->mObjX = 130;
		trackPanel->mObjects[1]->mObjY = 32;
		trackPanel->mObjects[2]->mObjY = 52;

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
		bkPanel1->mObjWidth = 127;
		bkPanel1->mObjHeight = 65;

		//Smooth Image
		ofxGuiPanel* sPanel = gui->addPanel(appPtr->smoothPanel, "Smooth", 236, 487, 10, 7);
		sPanel->addButton(smoothPanel_use, "", 12, 12, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		sPanel->addSlider(smoothPanel_smooth, "Smooth", 110, 13, 0.0f, 15.0f, smooth, kofxGui_Display_Int, 0);
		sPanel->mObjects[0]->mObjX = 105;
		sPanel->mObjects[0]->mObjY = 10;
		sPanel->mObjects[1]->mObjY = 30;
		sPanel->mObjWidth = 127;
		sPanel->mObjHeight = 65;

		//Highpass Image
		ofxGuiPanel* hpPanel = gui->addPanel(appPtr->highpassPanel, "Highpass", 386, 487, OFXGUI_PANEL_BORDER, 7);
		hpPanel->addButton(highpassPanel_use, "", 12, 12, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		hpPanel->addSlider(highpassPanel_blur, "Blur", 110, 13, 0.0f, 200.0f, highpassBlur, kofxGui_Display_Int, 0);
		hpPanel->addSlider(highpassPanel_noise, "Noise", 110, 13, 0.0f, 30.0f, highpassNoise, kofxGui_Display_Int, 0);
		hpPanel->mObjects[0]->mObjX = 105;
		hpPanel->mObjects[0]->mObjY = 10;
		hpPanel->mObjects[1]->mObjY = 30;
		hpPanel->mObjects[2]->mObjY = 60;
		hpPanel->mObjWidth = 127;
		hpPanel->mObjHeight = 95;


		//Amplify Image
		ofxGuiPanel* ampPanel = gui->addPanel(appPtr->amplifyPanel, "Amplify", 536, 487, OFXGUI_PANEL_BORDER, 7);
		ampPanel->addButton(amplifyPanel_use, "", 12, 12, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		ampPanel->addSlider(amplifyPanel_amp, "Amplify", 110, 13, 0.0f, 300.0f, highpassAmp, kofxGui_Display_Int, 0);
		ampPanel->mObjects[0]->mObjX = 105;
		ampPanel->mObjects[0]->mObjY = 10;
		ampPanel->mObjects[1]->mObjY = 30;
		ampPanel->mObjWidth = 127;
		ampPanel->mObjHeight = 65;

		//do update while inactive
		gui->forceUpdate(false);
		gui->activate(true);
	//}
		/****************************
		* Set GUI values on startup
		****************************/
		gui->update(appPtr->propertiesPanel_flipV, kofxGui_Set_Bool, &appPtr->bVerticalMirror, sizeof(bool));
		gui->update(appPtr->propertiesPanel_flipH, kofxGui_Set_Bool, &appPtr->bHorizontalMirror, sizeof(bool));
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
		gui->update(appPtr->backgroundPanel_dynamic, kofxGui_Set_Bool, &appPtr->bDynamicBG, sizeof(bool));
		//Smooth
		gui->update(appPtr->smoothPanel_use, kofxGui_Set_Bool, &appPtr->bSmooth, sizeof(bool));
		gui->update(appPtr->smoothPanel_smooth, kofxGui_Set_Bool, &appPtr->smooth, sizeof(float));
		//Highpass
		gui->update(appPtr->highpassPanel_use, kofxGui_Set_Bool, &appPtr->bHighpass, sizeof(bool));
		gui->update(appPtr->highpassPanel_blur, kofxGui_Set_Bool, &appPtr->highpassBlur, sizeof(float));
		gui->update(appPtr->highpassPanel_noise, kofxGui_Set_Bool, &appPtr->highpassNoise, sizeof(float));
		//Amplify
		gui->update(appPtr->amplifyPanel_use, kofxGui_Set_Bool, &appPtr->bAmplify, sizeof(bool));
		gui->update(appPtr->amplifyPanel_amp, kofxGui_Set_Bool, &appPtr->highpassAmp, sizeof(float));
		//Threshold
		gui->update(appPtr->trackedPanel_threshold, kofxGui_Set_Bool, &appPtr->threshold, sizeof(float));
		//Send TUIO
		gui->update(appPtr->optionPanel_tuio, kofxGui_Set_Bool, &appPtr->bTUIOMode, sizeof(bool));
		//GPU Mode
		gui->update(appPtr->gpuPanel_use, kofxGui_Set_Bool, &appPtr->bGPUMode, sizeof(bool));
}

void TBetaBase ::handleGui(int parameterId, int task, void* data, int length)
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
							vidGrabber.close();
							vidGrabber.setDeviceID(deviceID);
							vidGrabber.setVerbose(false);
							camWidth = vidGrabber.width;
							camHeight = vidGrabber.height;
							vidGrabber.initGrabber(camWidth,camHeight);

							calibrate.setCamRes(camHeight, camWidth);
							calibrate.computeCameraToScreenMap();

							//reset gpu textures and filters
							resetGPUTextures();

							processedImg.allocate(camWidth, camHeight); //Processed Image
							processedImg.setUseTexture(false);
							sourceImg.allocate(camWidth, camHeight);    //Source Image
							sourceImg.setUseTexture(false);
							grayImg.allocate(camWidth, camHeight);		//Gray Image
							grayBg.allocate(camWidth, camHeight);		//Background Image
							subtractBg.allocate(camWidth, camHeight);   //Background After subtraction
							grayDiff.allocate(camWidth, camHeight);		//Difference Image between Background and Source
							highpassImg.allocate(camWidth, camHeight);  //Highpass Image
							ampImg.allocate(camWidth, camHeight);		//Amplify Image
							fiLearn.allocate(camWidth, camHeight);		//ofxFloatImage used for simple dynamic background subtracti
							pressureMap.allocate(camWidth, camHeight);	//Pressure Map Image

							activeInput = true;		//set to active again
							bLearnBakground = true; //recapture background
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
							vidPlayer.loadMovie("test_videos/" + videoFileName);
							vidPlayer.play();
							printf("Video Mode\n");
							camHeight = vidPlayer.height;
							camWidth = vidPlayer.width;

							calibrate.setCamRes(camHeight, camWidth);
							calibrate.computeCameraToScreenMap();

							//reset gpu textures and filters
							resetGPUTextures();

							processedImg.allocate(camWidth, camHeight); //Processed Image
							processedImg.setUseTexture(false);
							sourceImg.allocate(camWidth, camHeight);    //Source Image
							sourceImg.setUseTexture(false);
							grayImg.allocate(camWidth, camHeight);		//Gray Image
							grayBg.allocate(camWidth, camHeight);		//Background Image
							subtractBg.allocate(camWidth, camHeight);   //Background After subtraction
							grayDiff.allocate(camWidth, camHeight);		//Difference Image between Background and Source
							highpassImg.allocate(camWidth, camHeight);  //Highpass Image
							ampImg.allocate(camWidth, camHeight);		//Amplify Image
							fiLearn.allocate(camWidth, camHeight);		//ofxFloatImage used for simple dynamic background subtracti
							pressureMap.allocate(camWidth, camHeight);	//Pressure Map Image

							activeInput = true;
							bLearnBakground = true;
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
						if(deviceID >= vidGrabber.getDeviceCount()) {deviceID = vidGrabber.getDeviceCount();}
						else{
							vidGrabber.close();
							vidGrabber.setDeviceID(deviceID);
							vidGrabber.setVerbose(true);
							vidGrabber.initGrabber(camWidth,camHeight);
							bLearnBakground = true;
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
						if(deviceID < 0) deviceID = 0;
						else{
							vidGrabber.close();
							vidGrabber.setDeviceID(deviceID);
							vidGrabber.setVerbose(true);
							vidGrabber.initGrabber(camWidth,camHeight);
							bLearnBakground = true;
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
						vidGrabber.videoSettings();
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
					bHorizontalMirror = *(bool*)data;
				break;
			case propertiesPanel_flipV:
				if(length == sizeof(bool))
					bVerticalMirror = *(bool*)data;
				break;
			//GPU
			case gpuPanel_use:
				if(length == sizeof(bool))
					bGPUMode= *(bool*)data;
				break;
			//Communication
			case optionPanel_tuio:
				if(length == sizeof(bool))
					bTUIOMode = *(bool*)data;
					myTUIO.blobs.clear();
				break;
			//Background
			case backgroundPanel_dynamic:
				if(length == sizeof(bool))
					bDynamicBG = *(bool*)data;
				break;
			case backgroundPanel_remove:
				if(length == sizeof(bool))
					bLearnBakground = *(bool*)data;
				break;
			//Highpass
			case highpassPanel_use:
				if(length == sizeof(bool))
					bHighpass = *(bool*)data;
				break;
			case highpassPanel_blur:
				if(length == sizeof(float))
					highpassBlur = *(float*)data;
				break;
			case highpassPanel_noise:
				if(length == sizeof(float))
					highpassNoise = *(float*)data;
				break;
			//Amplify
			case amplifyPanel_use:
				if(length == sizeof(bool))
					bAmplify = *(bool*)data;
				break;
			case amplifyPanel_amp:
				if(length == sizeof(float))
					highpassAmp = *(float*)data;
				break;
			case trackedPanel_threshold:
				if(length == sizeof(float))
					threshold = *(float*)data;
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
					smooth = *(float*)data;
				break;
			case smoothPanel_use:
				if(length == sizeof(bool))
					bSmooth = *(bool*)data;
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
			case kParameter_File:
				if(length == sizeof(string))
				{
					string file = *(string*)data;
					gui->buildFromXml(file);
				}
				break;

		}
	}

}





#endif //__GUI_DEFINITION

