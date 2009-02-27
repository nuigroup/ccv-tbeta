/*
 *  TBetaBase.cpp
 *  tbeta
 *
 *  Created by Artem Titoulenko on 2/1/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */

#include "TBetaBase.h"
#include "gui.h"

/******************************************************************************
 * The setup function is run once to perform initializations in the application
 *****************************************************************************/
void TBetaBase::setup()
{
    //set the title to 'tbeta'
    ofSetWindowTitle("tbeta");

    //removes the 'x' button on windows which causes a crash due to a GLUT bug
#ifdef TARGET_WIN32
	HWND hwndConsole = FindWindowA(NULL, "tbeta");
	HMENU hMnu = ::GetSystemMenu(hwndConsole, FALSE);
	RemoveMenu(hMnu, SC_CLOSE, MF_BYCOMMAND);
#endif

	ofSetBackgroundAuto(false);

    //create filter
    if( filter == NULL ) {
        filter = new ProcessFilters();
    }

	//Load Settings from config.xml file
	loadXMLSettings();

	//Setup Window Properties
	ofSetWindowShape(winWidth,winHeight);
	ofSetFrameRate(camRate * 1.4);			//This will be based on camera fps in the future
	ofSetVerticalSync(false);	            //Set vertical sync to false for better performance?

	//Pick the Source - camera or video
    if(bcamera)
    {
        activeInput = true;
        vidGrabber.listDevices();
        vidGrabber.setDeviceID(deviceID);
        vidGrabber.setVerbose(true);
        vidGrabber.initGrabber(camWidth,camHeight);
        int grabW = vidGrabber.width;
        int grabH = vidGrabber.height;
        printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, grabW, grabH);
    }
    else
    {
        activeInput = true;
        vidPlayer.loadMovie("test_videos/" + videoFileName);
        vidPlayer.play();
        printf("Video Mode\n");
        camHeight = vidPlayer.height;
        camWidth = vidPlayer.width;
    }

	/*****************************************************************************************************
	 * Allocate images (needed for drawing/processing images) ----Most of These won't be needed in the end
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

	gui = ofxGui::Instance(this);
	setupGUI();

    calib.setup(camWidth, camHeight, &tracker);

    filter->allocate( camWidth, camHeight );

	printf("Tbeta is setup!\n\n");
}

/****************************************************************
 *	Load/Save config.xml file Settings
 ****************************************************************/
void TBetaBase::loadXMLSettings(){

	// TODO: a seperate XML to map keyboard commands to action
	message = "Loading config.xml...";
	// Can this load via http?
	if( XML.loadFile("config.xml")){
		message = "Settings Loaded!";
	}else{
		//FAIL!
		message = "No Settings Found...";
	}
	//--------------------------------------------------------------
	//  START BINDING XML TO VARS
	//--------------------------------------------------------------
	//frameRate			= XML.getValue("CONFIG:APPLICATION:FRAMERATE",0);

	winWidth			= XML.getValue("CONFIG:WINDOW:WIDTH",0);
	winHeight			= XML.getValue("CONFIG:WINDOW:HEIGHT",0);
	minWidth			= XML.getValue("CONFIG:WINDOW:MINX",0);
	minHeight			= XML.getValue("CONFIG:WINDOW:MINY",0);

	bcamera				= XML.getValue("CONFIG:CAMERA_0:USECAMERA",0);
	deviceID			= XML.getValue("CONFIG:CAMERA_0:DEVICE",0);
	camWidth			= XML.getValue("CONFIG:CAMERA_0:WIDTH",0);
	camHeight			= XML.getValue("CONFIG:CAMERA_0:HEIGHT",0);
	camRate				= XML.getValue("CONFIG:CAMERA_0:FRAMERATE",0);

	videoFileName		= XML.getValue("CONFIG:VIDEO:FILENAME", "RearDI.m4v");

	maxBlobs			= XML.getValue("CONFIG:BLOBS:MAXNUMBER", 20);

	bShowLabels			= XML.getValue("CONFIG:BOOLEAN:LABELS",0);
	bFastMode			= XML.getValue("CONFIG:BOOLEAN:FAST",0);
	bDrawOutlines		= XML.getValue("CONFIG:BOOLEAN:OUTLINES",0);

	filter->bLearnBakground		= XML.getValue("CONFIG:BOOLEAN:LEARNBG",0);
	filter->bVerticalMirror		= XML.getValue("CONFIG:BOOLEAN:VMIRROR",0);
	filter->bHorizontalMirror	= XML.getValue("CONFIG:BOOLEAN:HMIRROR",0);

	//Filters
	filter->bHighpass			= XML.getValue("CONFIG:BOOLEAN:HIGHPASS",1);
	filter->bAmplify			= XML.getValue("CONFIG:BOOLEAN:AMPLIFY", 1);
	filter->bSmooth				= XML.getValue("CONFIG:BOOLEAN:SMOOTH", 1);
	filter->bDynamicBG			= XML.getValue("CONFIG:BOOLEAN:DYNAMICBG", 1);

	//GPU
	bGPUMode			= XML.getValue("CONFIG:BOOLEAN:GPU", 0);

	//Filter Settings
	filter->threshold			= XML.getValue("CONFIG:INT:THRESHOLD",0);
	filter->highpassBlur		= XML.getValue("CONFIG:INT:HIGHPASSBLUR",0);
	filter->highpassNoise		= XML.getValue("CONFIG:INT:HIGHPASSNOISE",0);
	filter->highpassAmp			= XML.getValue("CONFIG:INT:HIGHPASSAMP",0);
	filter->smooth				= XML.getValue("CONFIG:INT:SMOOTH",0);

	//--------------------------------------------------- TODO XML NETWORK SETTINGS
	bTUIOMode			= XML.getValue("CONFIG:BOOLEAN:TUIO",0);
	tmpLocalHost		= XML.getValue("CONFIG:NETWORK:LOCALHOST", "localhost");
	tmpPort				= XML.getValue("CONFIG:NETWORK:TUIOPORT_OUT", 3333);
	myTUIO.setup(tmpLocalHost.c_str(), tmpPort); //have to convert tmpLocalHost to a const char*
	//--------------------------------------------------------------
	//  END XML SETUP
}

void TBetaBase::saveConfiguration()
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
	XML.setValue("CONFIG:BOOLEAN:TUIO",bTUIOMode);
	XML.setValue("CONFIG:BOOLEAN:VMIRROR", filter->bVerticalMirror);
	XML.setValue("CONFIG:BOOLEAN:HMIRROR", filter->bHorizontalMirror);

	XML.setValue("CONFIG:BOOLEAN:HIGHPASS", filter->bHighpass);
	XML.setValue("CONFIG:BOOLEAN:AMPLIFY", filter->bAmplify);
	XML.setValue("CONFIG:BOOLEAN:SMOOTH", filter->bSmooth);
	XML.setValue("CONFIG:BOOLEAN:DYNAMICBG", filter->bDynamicBG);

	XML.setValue("CONFIG:BOOLEAN:GPU", bGPUMode);

	XML.setValue("CONFIG:INT:THRESHOLD", filter->threshold);
	XML.setValue("CONFIG:INT:HIGHPASSBLUR", filter->highpassBlur);
	XML.setValue("CONFIG:INT:HIGHPASSNOISE", filter->highpassNoise);
	XML.setValue("CONFIG:INT:HIGHPASSAMP", filter->highpassAmp);
	XML.setValue("CONFIG:INT:SMOOTH", filter->smooth);

	//	XML.setValue("CONFIG:NETWORK:LOCALHOST", *myTUIO.localHost);
	//	XML.setValue("CONFIG:NETWORK:TUIO_PORT_OUT",myTUIO.TUIOPort);

	XML.saveFile("config.xml");
}

/******************************************************************************
 * The update function runs continuously. Use it to update states and variables
 *****************************************************************************/
void TBetaBase::update()
{
    bNewFrame = false;

	if(activeInput){

		if(bcamera){
			vidGrabber.grabFrame();
			bNewFrame = vidGrabber.isFrameNew();
		}
		else{
			vidPlayer.idleMovie();
			bNewFrame = vidPlayer.isFrameNew();
		}

		if (bNewFrame)
		{
			ofBackground(0, 0, 0);

			//Calculate FPS of Camera
			frames++;
			float time = ofGetElapsedTimeMillis();
			if(time > (lastFPSlog + 1000)){
				fps = frames;
				frames = 0;
				lastFPSlog = time;
			}//End calculation

			float beforeTime = ofGetElapsedTimeMillis();

			if(bGPUMode){
				grabFrameToGPU(filter->gpuSourceTex);
				filter->applyGPUFilters();
				contourFinder.findContours(filter->gpuReadBackImageGS, 1, (camWidth*camHeight)/25, maxBlobs, false);
			}
			else{
				grabFrameToCPU();
                filter->applyCPUFilters( processedImg );
				contourFinder.findContours(processedImg, 1, (camWidth*camHeight)/25, maxBlobs, false);
			}

			//Track found contours/blobss
			tracker.track(&contourFinder);
			//get DSP time
			differenceTime = ofGetElapsedTimeMillis() - beforeTime;

            //Dynamic Background subtraction LearRate
			if(filter->bDynamicBG){
				filter->fLearnRate = 0.01f; //If there are no blobs, add the background faster.
				if(contourFinder.nBlobs > 0){ //If there ARE blobs, add the background slower.
					filter->fLearnRate = 0.0003f;
				}
			}//End Background Learning rate

			if(bTUIOMode){
				//We're not using frameseq right now with OSC
				//myTUIO.update();

				//Start sending OSC
				myTUIO.sendOSC();
			}
		}
	}
}

/************************************************
 *				Frame Grab
 ************************************************/

//Grab frame from CPU
void TBetaBase::grabFrameToCPU(){
	//Set sourceImg as new camera/video frame
	if(bcamera)
		sourceImg.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight);
	else
		sourceImg.setFromPixels(vidPlayer.getPixels(), 	camWidth, camHeight);

    //convert to grayscale
    processedImg = sourceImg;
}

//Grab frame from GPU
void TBetaBase::grabFrameToGPU(GLuint target){
	//grab the frame to a raw openGL texture
	if(bcamera)
	{
		glEnable(GL_TEXTURE_2D);
		//glPixelStorei(1);
		glBindTexture(GL_TEXTURE_2D, target);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, camWidth, camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, vidGrabber.getPixels());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, vidGrabber.getPixels());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D,0);
	}
	else{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, target);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, camWidth, camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, vidPlayer.getPixels());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, vidPlayer.getPixels());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D,0);
	}
}


/******************************************************************************
 * The draw function paints the textures onto the screen. It runs after update.
 *****************************************************************************/
void TBetaBase::draw(){

    if(showConfiguration) {

        //temporary. only auto draw if in fullscreen
        if(bNewFrame && bFullscreen == false){drawToScreen();}
        else{drawToScreen();}

        if(!bCalibration)
        gui->draw();

		//draw link to tbeta website
		ofSetColor(180, 220, 180, 180);
		ofFill();
		ofRect(ofGetWidth() - 230,ofGetHeight() - 14, 230, 14);
		ofSetColor(0x000000);
		ofDrawBitmapString("|  ~  |tbeta.nuigroup.com", ofGetWidth() - 230, ofGetHeight() - 2);
    }
}

void TBetaBase::drawToScreen(){

    /*********************************
	 * IF CALIBRATING
	 *********************************/
    if(bCalibration)
    {
        //Don't draw main interface
        bShowInterface = false;
		calib.passInContourFinder(contourFinder.nBlobs, contourFinder.blobs);
        calib.doCalibration();
    }

    /********************************
	 * IF SHOWING MAIN INTERFACE STUFF
	 ********************************/
    if(bDrawVideo && bShowInterface && !bFastMode)
    {
        ofSetColor(0xFFFFFF);
        //Draw Everything
        background.draw(0, 0);

        //Draw arrows
        ofSetColor(187, 200, 203);
        ofFill();
        ofTriangle(680, 420, 680, 460, 700, 440);
        ofTriangle(70, 420, 70, 460, 50, 440);
        ofSetColor(255, 255, 0);
		//		ofNoFill();
		//		ofTriangle(70, 420, 70, 460, 50, 440);

        ofSetColor(0xFFFFFF);

        if(bGPUMode) filter->drawGPU();
        else filter->draw();

        ofSetColor(0x000000);
        if(bShowPressure){bigvideo.drawString("Pressure Map", 140, 20);}
        else             {bigvideo.drawString("Source Image", 140, 20);}
		bigvideo.drawString("Tracked Image", 475, 20);
    }

    if(bFastMode)
    {
        //Display Application information in bottom right
        string str = "DSP Milliseconds: ";
        str+= ofToString(differenceTime, 0)+"\n\n";

        if(bcamera)
        {
            string str2 = "Camera Res:     ";
            str2+= ofToString(vidGrabber.width, 0) + " x " + ofToString(vidGrabber.height, 0)  + "\n";
            string str4 = "Camera FPS:     ";
            str4+= ofToString(fps, 0)+"\n\n";
            string str5 = "Blob Count:       ";
            str5+= ofToString(contourFinder.nBlobs, 0)+"\n";
            ofSetColor(0xFFFFFF);
            sidebarTXT.drawString(str + str2 + str4 + str5, 10, 30);
        }
        else
        {
            string str2 = "Video Res:       ";
            str2+= ofToString(vidPlayer.width, 0) + " x " + ofToString(vidPlayer.height, 0)  + "\n";
            string str4 = "Video FPS:       ";
            str4+= ofToString(fps, 0)+"\n\n";
            string str5 = "Blob Count:       ";
            str5+= ofToString(contourFinder.nBlobs, 0)+"\n";
            ofSetColor(0xFFFFFF);
            sidebarTXT.drawString(str + str2 + str4 + str5, 10, 30);
        }

        if(bTUIOMode)
        {	//Draw Port and IP to screen
            ofSetColor(0xffffff);
            char buf[256];
            sprintf(buf, "Sending TUIO messages to:\nHost: %s\nPort: %i", myTUIO.localHost, myTUIO.TUIOPort);
            sidebarTXT.drawString(buf, 10, 105);

            //Draw GREEN CIRCLE 'ON' LIGHT
            ofSetColor(0x00FF00);
            ofFill();
            ofCircle(20, 10, 5);
            ofNoFill();
        }

        ofSetColor(0xFF0000);
        sidebarTXT.drawString("Press spacebar for full mode", 10, 145);
    }

    /*********************************
	 * IF NOT CALIBRATING
	 *********************************/
    if(!bCalibration && !bFastMode)
    {
        //Draw main interface
        bShowInterface = true;

        //Display Application information in bottom right
        string str = "DSP Milliseconds: ";
        str+= ofToString(differenceTime, 0)+"\n\n";

        if(bcamera)
        {
            string str2 = "Camera Res:     ";
            str2+= ofToString(vidGrabber.width, 0) + " x " + ofToString(vidGrabber.height, 0)  + "\n";
            string str4 = "Camera FPS:     ";
            str4+= ofToString(fps, 0)+"\n";
            ofSetColor(0xFFFFFF);
            sidebarTXT.drawString(str + str2 + str4, 740, 410);
        }
        else
        {
            string str2 = "Video Res:       ";
            str2+= ofToString(vidPlayer.width, 0) + " x " + ofToString(vidPlayer.height, 0)  + "\n";
            string str4 = "Video FPS:       ";
            str4+= ofToString(fps, 0)+"\n";
            ofSetColor(0xFFFFFF);
            sidebarTXT.drawString(str + str2 + str4, 740, 410);
        }

        if(bTUIOMode)
        {	//Draw Port and IP to screen
            ofSetColor(0xffffff);
            char buf[256];
            sprintf(buf, "Sending TUIO messages to:\nHost: %s\nPort: %i", myTUIO.localHost, myTUIO.TUIOPort);
            sidebarTXT.drawString(buf, 740, 480);

            //Draw GREEN CIRCLE 'ON' LIGHT
            ofSetColor(0x00FF00);
            ofFill();
            ofCircle(35, 10, 5);
            ofNoFill();
        }

        ofSetColor(0xFF0000);
        sidebarTXT.drawString("Press spacebar for mini mode", 740, 580);

        //Draw PINK CIRCLE 'ON' LIGHT
        ofSetColor(255, 0, 255);
        ofFill();
        ofCircle(20, 10, 5);
        ofNoFill();

		//gui->draw();

    }

    if(bShowInterface && !bFastMode) //IF DRAWING BLOB OUTLINES
    {
        drawFingerOutlines();
    }
}

void TBetaBase::drawFingerOutlines(){

    //Find the blobs
    for(int i=0; i<contourFinder.nBlobs; i++)
    {
        //temp blob to rescale and draw on screen
        ofxTBetaCvBlob drawBlob;
        drawBlob = contourFinder.blobs[i];

        if(bDrawOutlines)
        {
            //Get the contour (points) so they can be drawn
            for( int j=0; j<contourFinder.blobs[i].nPts; j++ )
            {
                drawBlob.pts[j].x = (MAIN_WINDOW_WIDTH/camWidth)  * (drawBlob.pts[j].x);
                drawBlob.pts[j].y = (MAIN_WINDOW_HEIGHT/camHeight) * (drawBlob.pts[j].y);
            }

            //This adjusts the blob drawing for different cameras
            drawBlob.boundingRect.width  *= (MAIN_WINDOW_WIDTH/camWidth);
            drawBlob.boundingRect.height *= (MAIN_WINDOW_HEIGHT/camHeight);
            drawBlob.boundingRect.x		 *= (MAIN_WINDOW_WIDTH/camWidth);
            drawBlob.boundingRect.y		 *= (MAIN_WINDOW_HEIGHT/camHeight);

            //Draw contours (outlines) on the source image
            drawBlob.draw(40, 30);

//			ofSetColor(0x0099FF); //Make Plus Sign
//			ofRect((drawBlob.centroid.x * (MAIN_WINDOW_WIDTH/camWidth)) + 40, (drawBlob.centroid.y * (MAIN_WINDOW_HEIGHT/camHeight)) - drawBlob.boundingRect.height + 30, .5, drawBlob.boundingRect.height * 2); //Horizontal Plus
//			ofRect((drawBlob.centroid.x * (MAIN_WINDOW_WIDTH/camWidth)) - drawBlob.boundingRect.width + 40, (drawBlob.centroid.y  * (MAIN_WINDOW_HEIGHT/camHeight)) + 30, drawBlob.boundingRect.width * 2, .5); //Vertical Plus
        }

        if(bShowLabels)  //Show ID label;
        {
            float xpos = drawBlob.centroid.x * (MAIN_WINDOW_WIDTH/camWidth);
            float ypos = drawBlob.centroid.y * (MAIN_WINDOW_HEIGHT/camHeight);

            ofSetColor(0xCCFFCC);
            char idStr[1024];
            sprintf(idStr, "id: %i,(%i, %i)",drawBlob.id, drawBlob.lastCentroid.x, drawBlob.lastCentroid.y);
            verdana.drawString(idStr, xpos + 365, ypos + drawBlob.boundingRect.height/2 + 45);
        }
    }
    ofSetColor(0xFFFFFF);
}

/*****************************************************************************
 * KEY EVENTS
 *****************************************************************************/
void TBetaBase::keyPressed(int key) {
	if(showConfiguration) {
	switch(key)
	{
		case 'b':
			filter->bLearnBakground = true;
			break;
		case 'o':
			bDrawOutlines ? bDrawOutlines = false : bDrawOutlines = true;
			gui->update(appPtr->trackedPanel_outlines, kofxGui_Set_Bool, &appPtr->bDrawOutlines, sizeof(bool));
			break;
		case 'h':
			bHorizontalMirror ? bHorizontalMirror = false : bHorizontalMirror = true;
			gui->update(appPtr->propertiesPanel_flipH, kofxGui_Set_Bool, &appPtr->bHorizontalMirror, sizeof(bool));
			break;
		case 'j':
			bVerticalMirror ? bVerticalMirror = false : bVerticalMirror = true;
			gui->update(appPtr->propertiesPanel_flipV, kofxGui_Set_Bool, &appPtr->bVerticalMirror, sizeof(bool));
			break;
		case 't':
			if(!bCalibration && bTUIOMode)
			{
				bTUIOMode = false;
				myTUIO.blobs.clear();
				gui->update(appPtr->optionPanel_tuio, kofxGui_Set_Bool, &appPtr->bTUIOMode, sizeof(bool));
			}
			else
			{
				bTUIOMode = true;
				gui->update(appPtr->optionPanel_tuio, kofxGui_Set_Bool, &appPtr->bTUIOMode, sizeof(bool));
			}
			break;
		case 'g':
			bGPUMode ? bGPUMode = false : bGPUMode = true;
			gui->update(appPtr->gpuPanel_use, kofxGui_Set_Bool, &appPtr->bGPUMode, sizeof(bool));
			filter->bLearnBakground = true;
			break;
		case 'v':
			if(bcamera)
				vidGrabber.videoSettings();
			break;
		case 'l':
			bShowLabels ? bShowLabels = false : bShowLabels = true;
			gui->update(appPtr->trackedPanel_ids, kofxGui_Set_Bool, &appPtr->bShowLabels, sizeof(bool));
			break;
		case 'p':
			bShowPressure ? bShowPressure = false : bShowPressure = true;
			break;
		case ' ':
			if(bFastMode)
			{
				bFastMode = false;
				ofSetWindowShape(950,600); //default size
				//ofSetWindowTitle("Configuration");
			}
			else
			{
				bFastMode = true;
				ofSetWindowShape(190,155); //minimized size
				//ofSetWindowTitle("Mini");
			}
			break;
			 /***********************
			 * Keys for Calibration
			 ***********************/
		case 'x': //Begin Calibrating
			if(bCalibration){				
				bCalibration = false;
				calib.calibrating = false;
				tracker.isCalibrating = false;
				if(bFullscreen == true) ofToggleFullscreen(); bFullscreen = false; ofSetBackgroundAuto(false);
			}
			break;
        }
    }
}

void TBetaBase::keyReleased(int key){

    if(showConfiguration)
    {
        if( key == 'c' && !bCalibration)
        { //Enter/Exit Calibration
            bCalibration = true;
            calib.calibrating = true;
			tracker.isCalibrating = true;
            if(bFullscreen == false) ofToggleFullscreen(); bFullscreen = true; ofSetBackgroundAuto(true);
        }
    }

	if( key == '~' || key == '`') showConfiguration = !showConfiguration;
}

/*****************************************************************************
 *	Touch EVENTS
 *****************************************************************************/
void TBetaBase::TouchDown( ofxTBetaCvBlob b)
{
	 if(bTUIOMode)//If sending TUIO, add the blob to the map list
	 {
         //if blob is not otuside calibration mesh
         if(b.centroid.x != 0 && b.centroid.y != 0)
         myTUIO.blobs[b.id] = b;
	 }
}

void TBetaBase::TouchUp( ofxTBetaCvBlob b)
{
	 if(bTUIOMode)//If sending TUIO delete Blobs from map list
	 {
         std::map<int, ofxTBetaCvBlob>::iterator iter;
         for(iter = myTUIO.blobs.begin(); iter != myTUIO.blobs.end(); iter++)
         {
             if(iter->second.id == b.id)
             {
                myTUIO.blobs.erase(iter);
                break;
             }
         }
	 }
}

void TBetaBase::TouchMoved( ofxTBetaCvBlob b)
{
	 if(bTUIOMode)//If sending TUIO, add the blob to the map list
	 {
         //if blob is not otuside calibration mesh
         if(b.centroid.x != 0 && b.centroid.y != 0)
         myTUIO.blobs[b.id] = b;
	 }
}

/*****************************************************************************
 *	MOUSE EVENTS
 *****************************************************************************/
void TBetaBase::mouseMoved(int x, int y)
{
} 

void TBetaBase::mouseDragged(int x, int y, int button)
{
    if(showConfiguration)
	gui->mouseDragged(x, y, button); //guilistener
}

void TBetaBase::mousePressed(int x, int y, int button)
{
	if(showConfiguration){
		gui->mousePressed(x, y, button); //guilistener
		if(x > ofGetWidth() - 230 && y > ofGetHeight() - 14) ofLaunchBrowser("http://www.wordpress.com");	
	}
}

void TBetaBase::mouseReleased()
{
    if(showConfiguration)
	gui->mouseReleased(mouseX, mouseY, 0); //guilistener
}

/*****************************************************************************
 * ON EXIT
 *****************************************************************************/
void TBetaBase::exit()
{
	// -------------------------------- SAVE STATE ON EXIT
	saveConfiguration();

	printf("tBeta module has exited!\n");
}