/*
*  ofxNCoreVision.cpp
*  NUI Group Community Core Vision
*
*  Created by NUI Group Dev Team A on 2/1/09.
*  Copyright 2009 NUI Group\Inc. All rights reserved.
*
*/

#include "ofxNCoreVision.h"
#include "../Controls/gui.h"

/******************************************************************************
* The setup function is run once to perform initializations in the application
*****************************************************************************/
void ofxNCoreVision::_setup(ofEventArgs &e)
{
	//set the title
	ofSetWindowTitle(" Community Core Vision ");

//	ofSetBackgroundAuto(false);

	//create filter
	if ( filter == NULL ){filter = new ProcessFilters();}

	//Load Settings from config.xml file
	loadXMLSettings();

	//Setup Window Properties
	ofSetWindowShape(winWidth,winHeight);
	ofSetFrameRate(camRate * 1.4);			//This will be based on camera fps in the future
	ofSetVerticalSync(false);	            //Set vertical sync to false for better performance?

	initDevice();

	/*****************************************************************************************************
	* Allocate images (needed for drawing/processing images)
	******************************************************************************************************/
	processedImg.allocate(camWidth, camHeight); //main Image that'll be processed.
	processedImg.setUseTexture(false);
	sourceImg.allocate(camWidth, camHeight);    //Source Image
	sourceImg.setUseTexture(false);				//We don't need to draw this so don't create a texture
	/******************************************************************************************************/

	//Fonts - Is there a way to dynamically change font size?
	verdana.loadFont("verdana.ttf", 8, true, true);	   //Font used for small images
	sidebarTXT.loadFont("verdana.ttf", 8, true, true);
	bigvideo.loadFont("verdana.ttf", 13, true, true);  //Font used for big images.

	//Static Images
	background.loadImage("images/background.jpg"); //Main (Temp?) Background

	controls = ofxGui::Instance(this);
	setupControls();

	calib.setup(camWidth, camHeight, &tracker);

	filter->allocate( camWidth, camHeight );

	if (autoTracker)
	{
		printf("Starting in tracking mode...\n");
		showConfiguration = true;
	}

	if (bMiniMode)
	{
		bShowInterface = false;
		printf("Starting in Mini Mode...\n");
		ofSetWindowShape(190, 200); //minimized size
		//ofSetWindowTitle("Mini");
		filter->bMiniMode = bMiniMode;
	}
	else{
		//full mode
		bShowInterface = true;
	}
	printf("Community Core Vision is setup!\n\n");
}

/****************************************************************
*	Load/Save config.xml file Settings
****************************************************************/
void ofxNCoreVision::loadXMLSettings()
{
	// TODO: a seperate XML to map keyboard commands to action
	message = "Loading config.xml...";
	// Can this load via http?
	if ( XML.loadFile("config.xml"))
		message = "Settings Loaded!\n\n";
	else
		message = "No Settings Found...\n\n"; //FAIL
	
	//--------------------------------------------------------------
	//  START BINDING XML TO VARS
	//--------------------------------------------------------------
	winWidth					= XML.getValue("CONFIG:WINDOW:WIDTH", 950);
	winHeight					= XML.getValue("CONFIG:WINDOW:HEIGHT", 600);
	bcamera						= XML.getValue("CONFIG:CAMERA_0:USECAMERA", 1);
	deviceID					= XML.getValue("CONFIG:CAMERA_0:DEVICE", 0);
	camWidth					= XML.getValue("CONFIG:CAMERA_0:WIDTH", 320);
	camHeight					= XML.getValue("CONFIG:CAMERA_0:HEIGHT", 240);
	camRate						= XML.getValue("CONFIG:CAMERA_0:FRAMERATE", 0);
	videoFileName				= XML.getValue("CONFIG:VIDEO:FILENAME", "RearDI.m4v");
	maxBlobs					= XML.getValue("CONFIG:BLOBS:MAXNUMBER", 20);
	bShowLabels					= XML.getValue("CONFIG:BOOLEAN:LABELS",0);
	bDrawOutlines				= XML.getValue("CONFIG:BOOLEAN:OUTLINES",0);
	filter->bLearnBakground		= XML.getValue("CONFIG:BOOLEAN:LEARNBG",0);
	filter->bVerticalMirror		= XML.getValue("CONFIG:BOOLEAN:VMIRROR",0);
	filter->bHorizontalMirror	= XML.getValue("CONFIG:BOOLEAN:HMIRROR",0);
	//Filters
	filter->bHighpass			= XML.getValue("CONFIG:BOOLEAN:HIGHPASS",1);
	filter->bAmplify			= XML.getValue("CONFIG:BOOLEAN:AMPLIFY", 1);
	filter->bSmooth				= XML.getValue("CONFIG:BOOLEAN:SMOOTH", 1);
	filter->bDynamicBG			= XML.getValue("CONFIG:BOOLEAN:DYNAMICBG", 1);
	//MODES
	bGPUMode					= XML.getValue("CONFIG:BOOLEAN:GPU", 0);
	bMiniMode                   = XML.getValue("CONFIG:BOOLEAN:MINIMODE",0);
	//CONTROLS
	tracker.MIN_MOVEMENT_THRESHOLD	= XML.getValue("CONFIG:INT:MINMOVEMENT",0);
	MIN_BLOB_SIZE				= XML.getValue("CONFIG:INT:MINBLOBSIZE",2);
	MAX_BLOB_SIZE				= XML.getValue("CONFIG:INT:MAXBLOBSIZE",100);
	backgroundLearnRate			= XML.getValue("CONFIG:INT:BGLEARNRATE", 0.01f);
	//Filter Settings
	filter->threshold			= XML.getValue("CONFIG:INT:THRESHOLD",0);
	filter->highpassBlur		= XML.getValue("CONFIG:INT:HIGHPASSBLUR",0);
	filter->highpassNoise		= XML.getValue("CONFIG:INT:HIGHPASSNOISE",0);
	filter->highpassAmp			= XML.getValue("CONFIG:INT:HIGHPASSAMP",0);
	filter->smooth				= XML.getValue("CONFIG:INT:SMOOTH",0);
	//--------------------------------------------------- TODO XML NETWORK SETTINGS
	bTUIOMode					= XML.getValue("CONFIG:BOOLEAN:TUIO",0);
	myTUIO.bOSCMode				= XML.getValue("CONFIG:BOOLEAN:OSCMODE",1);
	myTUIO.bTCPMode				= XML.getValue("CONFIG:BOOLEAN:TCPMODE",1);
	myTUIO.bHeightWidth			= XML.getValue("CONFIG:BOOLEAN:HEIGHTWIDTH",0);
	tmpLocalHost				= XML.getValue("CONFIG:NETWORK:LOCALHOST", "localhost");
	tmpPort						= XML.getValue("CONFIG:NETWORK:TUIOPORT_OUT", 3333);
	myTUIO.setup(tmpLocalHost.c_str(), tmpPort); //have to convert tmpLocalHost to a const char*
	//--------------------------------------------------------------
	//  END XML SETUP
}

void ofxNCoreVision::saveSettings()
{
	XML.setValue("CONFIG:CAMERA_0:USECAMERA", bcamera);
	XML.setValue("CONFIG:CAMERA_0:DEVICE", deviceID);
	XML.setValue("CONFIG:CAMERA_0:WIDTH", camWidth);
	XML.setValue("CONFIG:CAMERA_0:HEIGHT", camHeight);
	XML.setValue("CONFIG:CAMERA_0:FRAMERATE", camRate);
	XML.setValue("CONFIG:BOOLEAN:PRESSURE",bShowPressure);
	XML.setValue("CONFIG:BOOLEAN:LABELS",bShowLabels);
	XML.setValue("CONFIG:BOOLEAN:OUTLINES",bDrawOutlines);
	XML.setValue("CONFIG:BOOLEAN:LEARNBG", filter->bLearnBakground);
	XML.setValue("CONFIG:BOOLEAN:VMIRROR", filter->bVerticalMirror);
	XML.setValue("CONFIG:BOOLEAN:HMIRROR", filter->bHorizontalMirror);
	XML.setValue("CONFIG:BOOLEAN:HIGHPASS", filter->bHighpass);
	XML.setValue("CONFIG:BOOLEAN:AMPLIFY", filter->bAmplify);
	XML.setValue("CONFIG:BOOLEAN:SMOOTH", filter->bSmooth);
	XML.setValue("CONFIG:BOOLEAN:DYNAMICBG", filter->bDynamicBG);
	XML.setValue("CONFIG:BOOLEAN:GPU", bGPUMode);
	XML.setValue("CONFIG:INT:MINMOVEMENT", tracker.MIN_MOVEMENT_THRESHOLD);
	XML.setValue("CONFIG:INT:MINBLOBSIZE", MIN_BLOB_SIZE);
	XML.setValue("CONFIG:INT:MAXBLOBSIZE", MAX_BLOB_SIZE);
	XML.setValue("CONFIG:INT:BGLEARNRATE", backgroundLearnRate);
	XML.setValue("CONFIG:INT:THRESHOLD", filter->threshold);
	XML.setValue("CONFIG:INT:HIGHPASSBLUR", filter->highpassBlur);
	XML.setValue("CONFIG:INT:HIGHPASSNOISE", filter->highpassNoise);
	XML.setValue("CONFIG:INT:HIGHPASSAMP", filter->highpassAmp);
	XML.setValue("CONFIG:INT:SMOOTH", filter->smooth);
	XML.setValue("CONFIG:BOOLEAN:MINIMODE", bMiniMode);
	XML.setValue("CONFIG:BOOLEAN:TUIO",bTUIOMode);
	XML.setValue("CONFIG:BOOLEAN:HEIGHTWIDTH", myTUIO.bHeightWidth);
	XML.setValue("CONFIG:BOOLEAN:OSCMODE", myTUIO.bOSCMode);
	XML.setValue("CONFIG:BOOLEAN:TCPMODE", myTUIO.bTCPMode);

	//	XML.setValue("CONFIG:NETWORK:LOCALHOST", *myTUIO.localHost);
	//	XML.setValue("CONFIG:NETWORK:TUIO_PORT_OUT",myTUIO.TUIOPort);

	XML.saveFile("config.xml");
}

/******************************************************************************
* The update function runs continuously. Use it to update states and variables
*****************************************************************************/
void ofxNCoreVision::_update(ofEventArgs &e)
{
	if(exited) return;

	bNewFrame = false;

	if (activeInput)
	{
		if (bcamera) //if camera
		{
		    #ifdef TARGET_WIN32
/*				if(PS3!=NULL)//ps3 camera
				{
					bNewFrame = PS3->isFrameNew();
				}
				else 
*/				if(ffmv!=NULL)
 				{
					ffmv->grabFrame();
 					bNewFrame = true;
				}
				else if(vidGrabber !=NULL)
				{
 					vidGrabber->grabFrame();
 					bNewFrame = vidGrabber->isFrameNew();
 				}
				else if(dsvl !=NULL)
				{
 					bNewFrame = dsvl->isFrameNew();
 				}
 			#else
                vidGrabber->grabFrame();
 				bNewFrame = vidGrabber->isFrameNew();
 			#endif
		}
		else //if video
		{
			vidPlayer->idleMovie();
			bNewFrame = vidPlayer->isFrameNew();
		}

		//if no new frame, return
		if(!bNewFrame){
			return;
		}
		else//else process camera frame
		{
			ofBackground(0, 0, 0);

			// Calculate FPS of Camera
			frames++;
			float time = ofGetElapsedTimeMillis();
			if (time > (lastFPSlog + 1000))
			{
				fps = frames;
				frames = 0;
				lastFPSlog = time;
			}//End calculation

			float beforeTime = ofGetElapsedTimeMillis();

			if (bGPUMode)
			{
				grabFrameToGPU(filter->gpuSourceTex);
				filter->applyGPUFilters();
				contourFinder.findContours(filter->gpuReadBackImageGS,  (MIN_BLOB_SIZE * 2) + 1, ((camWidth * camHeight) * .4) * (MAX_BLOB_SIZE * .001), maxBlobs, false);
			}
			else
			{
				grabFrameToCPU();
				filter->applyCPUFilters( processedImg );
				contourFinder.findContours(processedImg,  (MIN_BLOB_SIZE * 2) + 1, ((camWidth * camHeight) * .4) * (MAX_BLOB_SIZE * .001), maxBlobs, false);
			}

			//Track found contours/blobss
			tracker.track(&contourFinder);
			//get DSP time
			differenceTime = ofGetElapsedTimeMillis() - beforeTime;

			//Dynamic Background subtraction LearRate
			if (filter->bDynamicBG)
			{
				filter->fLearnRate = backgroundLearnRate * .0001; //If there are no blobs, add the background faster.
				if (contourFinder.nBlobs > 0) //If there ARE blobs, add the background slower.
				{
					filter->fLearnRate = backgroundLearnRate * .0001;
				}
			}//End Background Learning rate

			if (bTUIOMode)
			{
				//Start sending OSC
				myTUIO.sendTUIO();
			}
		}
	}
}

/************************************************
*				Init Device
************************************************/
//Init Device (camera/video)
void ofxNCoreVision::initDevice(){

	//Pick the Source - camera or video
	if (bcamera)
	{
		activeInput = true;

		//check if a firefly, ps3 camera, or other is plugged in
		#ifdef TARGET_WIN32
			/****PS3 - PS3 camera only****/
/*			if(ofxPS3::getDeviceCount() > 0){
				PS3 = new ofxPS3();
				PS3->listDevices();
				PS3->initPS3(camWidth, camHeight, camRate);
				printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, PS3->getCamWidth(), PS3->getCamHeight());
			}
			/****ffmv - firefly camera only****/
//			else
			if(ffmv->getDeviceCount() > 0 && ffmv == NULL){
			   ffmv = new ofxffmv();
			   ffmv->listDevices();
			   ffmv->initFFMV(camWidth,camHeight);
			   printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, ffmv->getCamWidth(), ffmv->getCamHeight());
			   camWidth = ffmv->getCamWidth();
			   camHeight = ffmv->getCamHeight();
			}
			else if( vidGrabber == NULL ) {
				vidGrabber = new ofVideoGrabber();
				vidGrabber->listDevices();
				vidGrabber->setVerbose(true);
				vidGrabber->initGrabber(camWidth,camHeight);
				printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, vidGrabber->width, vidGrabber->height);
				camWidth = vidGrabber->width;
				camHeight = vidGrabber->height;
			}
			else if( dsvl == NULL) {
				dsvl = new ofxDSVL();
				dsvl->initDSVL();
				printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, dsvl->getCamWidth(), dsvl->getCamHeight());
				camWidth = dsvl->getCamWidth();
				camHeight = dsvl->getCamHeight();
			}
		#else
		if( vidGrabber == NULL ) {
            vidGrabber = new ofVideoGrabber();
			vidGrabber->listDevices();
            vidGrabber->setVerbose(true);
            vidGrabber->initGrabber(camWidth,camHeight);
			int grabW = vidGrabber->width;
			int grabH = vidGrabber->height;
			printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, grabW, grabH);
        }
		#endif
	}else{
		if( vidPlayer == NULL ) {
			activeInput = true;
            vidPlayer = new ofVideoPlayer();
            vidPlayer->loadMovie( videoFileName );
            vidPlayer->play();
            vidPlayer->setLoopState(OF_LOOP_NORMAL);
			printf("Video Mode\n\n");
			camHeight = vidPlayer->height;
			camWidth = vidPlayer->width;
        }
	}
}

//get pixels from camera
void ofxNCoreVision::getPixels(){

/*	if(PS3!=NULL){
		sourceImg.setFromPixels(PS3->getPixels(), camWidth, camHeight);
		//convert to grayscale
		processedImg = sourceImg;
	}
	else 
*/	if(ffmv != NULL){
		processedImg.setFromPixels(ffmv->fcImage[ffmv->getDeviceID()].pData, camWidth, camHeight);
	}
	else if(vidGrabber != NULL ) {
		sourceImg.setFromPixels(vidGrabber->getPixels(), camWidth, camHeight);
		//convert to grayscale
		processedImg = sourceImg;
	}
	if(dsvl!=NULL)
	{
		//sourceImg.setFromPixels(vidGrabber->getPixels(), camWidth, camHeight);
		sourceImg.setFromPixels(dsvl->getPixels(), camWidth, camHeight);
		//convert to grayscale
		processedImg = sourceImg;
	}
}


//Grab frame from CPU
void ofxNCoreVision::grabFrameToCPU()
{
	//Set sourceImg as new camera/video frame
	if (bcamera)
	{
	    #ifdef TARGET_WIN32
			getPixels();
 		#else
            sourceImg.setFromPixels(vidGrabber->getPixels(), camWidth, camHeight);
 			//convert to grayscale
 			processedImg = sourceImg;
 		#endif
	}
	else
	{
		sourceImg.setFromPixels(vidPlayer->getPixels(), camWidth, camHeight);
		//convert to grayscale
		processedImg = sourceImg;
	}
}

//Grab frame from GPU
void ofxNCoreVision::grabFrameToGPU(GLuint target)
{
	//grab the frame to a raw openGL texture
	if (bcamera)
	{
		glEnable(GL_TEXTURE_2D);
		//glPixelStorei(1);
		glBindTexture(GL_TEXTURE_2D, target);

		#ifdef TARGET_WIN32
/*			if(PS3!=NULL)
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, PS3->getPixels());
			}
			else 
*/			if(vidGrabber!=NULL)
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, vidGrabber->getPixels());
			}
			else if(dsvl!=NULL)
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, dsvl->getPixels());
			}
		#else
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, vidGrabber->getPixels());
		#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D,0);
	}
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, target);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, vidPlayer->getPixels());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D,0);
	}
}


/******************************************************************************
* The draw function paints the textures onto the screen. It runs after update.
*****************************************************************************/
void ofxNCoreVision::_draw(ofEventArgs &e)
{
	if(exited) return;

	if (showConfiguration) 
	{
		//if calibration
		if (bCalibration)
		{
			//Don't draw main interface
			calib.passInContourFinder(contourFinder.nBlobs, contourFinder.blobs);
			calib.doCalibration();
		}
		//if mini mode
		else if (bMiniMode)
		{
			drawMiniMode();
		}
		//if full mode
		else if (bShowInterface)
		{
			drawFullMode();
			if(bDrawOutlines || bShowLabels) drawFingerOutlines();
		}
		//draw gui controls
		if (!bCalibration && !bMiniMode) {controls->draw();}
	}	
}

void ofxNCoreVision::drawFullMode(){

	ofSetColor(0xFFFFFF);
	//Draw Background Image
	background.draw(0, 0);
	//Draw arrows
	ofSetColor(187, 200, 203);
	ofFill();
	ofTriangle(680, 420, 680, 460, 700, 440);
	ofTriangle(70, 420, 70, 460, 50, 440);
	ofSetColor(255, 255, 0);

	ofSetColor(0xFFFFFF);
	//Draw Image Filters To Screen
	if (bGPUMode) filter->drawGPU();
	else filter->draw();

	ofSetColor(0x000000);
	if (bShowPressure)
	{
		bigvideo.drawString("Pressure Map", 140, 20);
	}
	else
	{
		bigvideo.drawString("Source Image", 140, 20);
	}
	bigvideo.drawString("Tracked Image", 475, 20);

	//draw link to tbeta website
	ofSetColor(79, 79, 79);
	ofFill();
	ofRect(ofGetWidth() - 228,ofGetHeight() - 14, 228, 14);
	ofSetColor(0xFFFFFF);
	ofDrawBitmapString("|  ~  |tbeta.nuigroup.com", ofGetWidth() - 228, ofGetHeight() - 2);

	//Display Application information in bottom right
	string str = "Calc. Time [ms]: ";
	str+= ofToString(differenceTime, 0)+"\n\n";

	if (bcamera)
	{
		string str2 = "Camera [Res]:     ";
        str2+= ofToString(camWidth, 0) + " x " + ofToString(camWidth, 0)  + "\n";
		string str4 = "Camera [fps]:     ";
		str4+= ofToString(fps, 0)+"\n";
		ofSetColor(0xFFFFFF);
		sidebarTXT.drawString(str + str2 + str4, 740, 410);
	}
	else
	{
		string str2 = "Video [Res]:      ";
		str2+= ofToString(vidPlayer->width, 0) + " x " + ofToString(vidPlayer->height, 0)  + "\n";
		string str4 = "Video [fps]:       ";
		str4+= ofToString(fps, 0)+"\n";
		ofSetColor(0xFFFFFF);
		sidebarTXT.drawString(str + str2 + str4, 740, 410);
	}

	if (bTUIOMode)
	{
		//Draw Port and IP to screen
		ofSetColor(0xffffff);
		char buf[256];
		sprintf(buf, "Sending TUIO messages to:\nHost: %s\nPort: %i", myTUIO.localHost, myTUIO.TUIOPort);
		sidebarTXT.drawString(buf, 740, 480);
	}

	ofSetColor(0xFF0000);
	sidebarTXT.drawString("Press spacebar to toggle mini mode", ofGetWidth() - 215, 580);
}

void ofxNCoreVision::drawMiniMode()
{
	//black background
	ofSetColor(0,0,0);
	ofRect(0,0,ofGetWidth(), ofGetHeight());
	//draw outlines
	if (bDrawOutlines){
		for (int i=0; i<contourFinder.nBlobs; i++)
		{
			contourFinder.blobs[i].drawContours(0,0, camWidth, camHeight+175, ofGetWidth(), ofGetHeight());
		}
	}

	//draw grey rectagles for text information
	ofSetColor(128,128,128);
	ofFill();
	ofRect(0,ofGetHeight() - 83, ofGetWidth(), 20);
	ofRect(0,ofGetHeight() - 62, ofGetWidth(), 20);
	ofRect(0,ofGetHeight() - 41, ofGetWidth(), 20);
	ofRect(0,ofGetHeight() - 20, ofGetWidth(), 20);

	//draw text
	ofSetColor(250,250,250);
	sidebarTXT.drawString("Calc. Time  [ms]:        " + ofToString(differenceTime,0),10, ofGetHeight() - 70 );
	if (bcamera){
		sidebarTXT.drawString("Camera [fps]:            " + ofToString(fps,0),10, ofGetHeight() - 50 );
	}
	else {
		sidebarTXT.drawString("Video [fps]:             " + ofToString(fps,0),10, ofGetHeight() - 50 );
	}
	sidebarTXT.drawString("Blob Count:               " + ofToString(contourFinder.nBlobs,0),10, ofGetHeight() - 29 );
	sidebarTXT.drawString("Sending TUIO:  " ,10, ofGetHeight() - 9 );

	//draw green tuio circle
	if (bTUIOMode)
	{
		//Draw GREEN CIRCLE 'ON' LIGHT
		ofSetColor(0x00FF00);
		ofFill();
		ofCircle(ofGetWidth() - 17 , ofGetHeight() - 10, 5);
		ofNoFill();
	}
}

void ofxNCoreVision::drawFingerOutlines()
{
	//Find the blobs for drawing
	for (int i=0; i<contourFinder.nBlobs; i++)
	{
		if (bDrawOutlines)
		{
			//Draw contours (outlines) on the source image
			contourFinder.blobs[i].drawContours(40, 30, camWidth, camHeight, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
		}
		if (bShowLabels) //Show ID label;
		{
			float xpos = contourFinder.blobs[i].centroid.x * (MAIN_WINDOW_WIDTH/camWidth);
			float ypos = contourFinder.blobs[i].centroid.y * (MAIN_WINDOW_HEIGHT/camHeight);

			ofSetColor(0xCCFFCC);
			char idStr[1024];
			sprintf(idStr, "id: %i", contourFinder.blobs[i].id);
			verdana.drawString(idStr, xpos + 365, ypos + contourFinder.blobs[i].boundingRect.height/2 + 45);
		}
	}
	ofSetColor(0xFFFFFF);
}

/*****************************************************************************
* KEY EVENTS
*****************************************************************************/
void ofxNCoreVision::_keyPressed(ofKeyEventArgs &e)
{
	// detect escape key
	if(e.key==0x1b)
	{
		exited=true;
	}

	if (showConfiguration)
	{
		switch (e.key)
		{
			// ughhh!!!!
			// my main problem is that the slider doesn't move....
		case 'a':
			filter->threshold++;
			controls->update(appPtr->trackedPanel_threshold, kofxGui_Set_Int, &appPtr->filter->threshold, sizeof(int));
			break;
		case 'z':
			filter->threshold--;
			controls->update(appPtr->trackedPanel_threshold, kofxGui_Set_Int, &appPtr->filter->threshold, sizeof(int));
			break;
		case 'b':
			filter->bLearnBakground = true;
			break;
		case 'o':
			bDrawOutlines ? bDrawOutlines = false : bDrawOutlines = true;
			controls->update(appPtr->trackedPanel_outlines, kofxGui_Set_Bool, &appPtr->bDrawOutlines, sizeof(bool));
			break;
		case 'h':
			filter->bHorizontalMirror ? filter->bHorizontalMirror = false : filter->bHorizontalMirror = true;
			controls->update(appPtr->propertiesPanel_flipH, kofxGui_Set_Bool, &appPtr->filter->bHorizontalMirror, sizeof(bool));
			break;
		case 'j':
			filter->bVerticalMirror ? filter->bVerticalMirror = false : filter->bVerticalMirror = true;
			controls->update(appPtr->propertiesPanel_flipV, kofxGui_Set_Bool, &appPtr->filter->bVerticalMirror, sizeof(bool));
			break;
		case 't':
			myTUIO.bOSCMode = !myTUIO.bOSCMode;
			myTUIO.bTCPMode = false;
			bTUIOMode = myTUIO.bOSCMode;
			controls->update(appPtr->optionPanel_tuio_tcp, kofxGui_Set_Bool, &appPtr->myTUIO.bTCPMode, sizeof(bool));
			controls->update(appPtr->optionPanel_tuio_osc, kofxGui_Set_Bool, &appPtr->myTUIO.bOSCMode, sizeof(bool));
			//clear blobs
			myTUIO.blobs.clear();
			break;
		case 'f':
			myTUIO.bOSCMode = false;
			myTUIO.bTCPMode = !myTUIO.bTCPMode;
			bTUIOMode = myTUIO.bTCPMode;
			controls->update(appPtr->optionPanel_tuio_tcp, kofxGui_Set_Bool, &appPtr->myTUIO.bTCPMode, sizeof(bool));
			controls->update(appPtr->optionPanel_tuio_osc, kofxGui_Set_Bool, &appPtr->myTUIO.bOSCMode, sizeof(bool));
			//clear blobs
			myTUIO.blobs.clear();
			break;
		case 'g':
			bGPUMode ? bGPUMode = false : bGPUMode = true;
			controls->update(appPtr->gpuPanel_use, kofxGui_Set_Bool, &appPtr->bGPUMode, sizeof(bool));
			filter->bLearnBakground = true;
			break;
		case 'v':
			if (bcamera)
				vidGrabber->videoSettings();
			break;
		case 'l':
			bShowLabels ? bShowLabels = false : bShowLabels = true;
			controls->update(appPtr->trackedPanel_ids, kofxGui_Set_Bool, &appPtr->bShowLabels, sizeof(bool));
			break;
		case 'p':
			bShowPressure ? bShowPressure = false : bShowPressure = true;
			break;
		case ' ':
			if (bMiniMode && !bCalibration) // NEED TO ADD HERE ONLY GO MINI MODE IF NOT CALIBRATING
			{
				bMiniMode = false;
				bShowInterface = true;
				filter->bMiniMode = bMiniMode;
				ofSetWindowShape(950,600); //default size
			}
			else if(!bCalibration)
			{
				bMiniMode = true;
				bShowInterface = false;
				filter->bMiniMode = bMiniMode;
				ofSetWindowShape(190,200); //minimized size
			}
			break;
		case 'x': //Exit Calibrating
			if (bCalibration)
			{	bShowInterface = true;
			bCalibration = false;
			calib.calibrating = false;
			tracker.isCalibrating = false;
			if (bFullscreen == true) ofToggleFullscreen();
			bFullscreen = false;
//			ofSetBackgroundAuto(false);
			}
			break;
		}
	}
}

void ofxNCoreVision::_keyReleased(ofKeyEventArgs &e)
{
	if (showConfiguration)
	{
		if ( e.key == 'c' && !bCalibration)
		{
			bShowInterface = false;
			//Enter/Exit Calibration
			bCalibration = true;
			calib.calibrating = true;
			tracker.isCalibrating = true;
			if (bFullscreen == false) ofToggleFullscreen();
			bFullscreen = true;
//			ofSetBackgroundAuto(true);
		}
	}
	if ( e.key == '~' || e.key == '`' /*&& !bMiniMode*/ && !bCalibration) showConfiguration = !showConfiguration;
}

/*****************************************************************************
*	Touch EVENTS
*****************************************************************************/
void ofxNCoreVision::TouchDown( Blob b)
{
	if (bTUIOMode)//If sending TUIO, add the blob to the map list
	{
		//if blob is not otuside calibration mesh
		if (b.centroid.x != 0 && b.centroid.y != 0)
			myTUIO.blobs[b.id] = b;
	}
}

void ofxNCoreVision::TouchUp( Blob b)
{
	if (bTUIOMode)//If sending TUIO delete Blobs from map list
	{
		std::map<int, Blob>::iterator iter;
		for (iter = myTUIO.blobs.begin(); iter != myTUIO.blobs.end(); iter++)
		{
			if (iter->second.id == b.id)
			{
				myTUIO.blobs.erase(iter);
				break;
			}
		}
	}
}

void ofxNCoreVision::TouchMoved( Blob b)
{
	if (bTUIOMode)//If sending TUIO, add the blob to the map list
	{
		//if blob is not otuside calibration mesh
		if (b.centroid.x != 0 && b.centroid.y != 0)
			myTUIO.blobs[b.id] = b;
	}
}

/*****************************************************************************
*	MOUSE EVENTS
*****************************************************************************/
void ofxNCoreVision::_mouseDragged(ofMouseEventArgs &e)
{
	if (showConfiguration)
		controls->mouseDragged(e.x, e.y, e.button); //guilistener
}

void ofxNCoreVision::_mousePressed(ofMouseEventArgs &e)
{
	if (showConfiguration)
	{
		controls->mousePressed(e.x, e.y, e.button); //guilistener
		if (e.x > ofGetWidth() - 230 && e.y > ofGetHeight() - 14){ofLaunchBrowser("http://www.openframeworks.cc/forum/viewtopic.php?p=9651#9651");}
	}
}

void ofxNCoreVision::_mouseReleased(ofMouseEventArgs &e)
{
	if (showConfiguration)
		controls->mouseReleased(e.x, e.y, 0); //guilistener
}

/*****************************************************************************
* Getters
*****************************************************************************/

std::vector<pair<int,Blob>> ofxNCoreVision::getBlobs(){

	return tracker.getTrackedBlobs();
}

/*****************************************************************************
* ON EXIT
*****************************************************************************/
void ofxNCoreVision::_exit(ofEventArgs &e)
{
    #ifdef TARGET_WIN32
//	if(PS3!=NULL) delete PS3;
	if(ffmv!=NULL) delete ffmv;
	if(dsvl!=NULL) delete dsvl;	
	#endif

	if(vidGrabber!=NULL) delete vidGrabber;
	if(vidPlayer !=NULL) delete vidGrabber;
	// -------------------------------- SAVE STATE ON EXIT
	saveSettings();

	printf("Vision module has exited!\n");
}

