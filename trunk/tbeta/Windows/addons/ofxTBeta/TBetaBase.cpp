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
                drawBlob.pts[j].x = (MAIN_WINDOW_WIDTH/camera.camWidth)  * (drawBlob.pts[j].x);
                drawBlob.pts[j].y = (MAIN_WINDOW_HEIGHT/camera.camHeight) * (drawBlob.pts[j].y);
            }

            //This adjusts the blob drawing for different cameras
            drawBlob.boundingRect.width  *= (MAIN_WINDOW_WIDTH/camera.camWidth);
            drawBlob.boundingRect.height *= (MAIN_WINDOW_HEIGHT/camera.camHeight);
            drawBlob.boundingRect.x		 *= (MAIN_WINDOW_WIDTH/camera.camWidth);
            drawBlob.boundingRect.y		 *= (MAIN_WINDOW_HEIGHT/camera.camHeight);

            //Draw contours (outlines) on the source image
            drawBlob.draw(40, 30);

			//           ofSetColor(0x0099FF); //Make Plus Sign
			//           ofRect((drawBlob.centroid.x * (MAIN_WINDOW_WIDTH/camera.camWidth)) + 40, (drawBlob.centroid.y * (MAIN_WINDOW_HEIGHT/camera.camHeight)) - drawBlob.boundingRect.height + 30, .5, drawBlob.boundingRect.height * 2); //Horizontal Plus
			//           ofRect((drawBlob.centroid.x * (MAIN_WINDOW_WIDTH/camera.camWidth)) - drawBlob.boundingRect.width + 40, (drawBlob.centroid.y  * (MAIN_WINDOW_HEIGHT/camera.camHeight)) + 30, drawBlob.boundingRect.width * 2, .5); //Vertical Plus
        }

        if(bShowLabels)  //Show ID label;
        {
            float xpos = drawBlob.centroid.x * (MAIN_WINDOW_WIDTH/camera.camWidth);
            float ypos = drawBlob.centroid.y * (MAIN_WINDOW_HEIGHT/camera.camHeight);

            ofSetColor(0xCCFFCC);
            char idStr[1024];
            sprintf(idStr, "id: %i,(%i, %i)",drawBlob.id, drawBlob.lastCentroid.x, drawBlob.lastCentroid.y);
            verdana.drawString(idStr, xpos + 365, ypos + drawBlob.boundingRect.height/2 + 45);
        }
    }
    ofSetColor(0xFFFFFF);
}

/************************************************
 *				FILTERS
 ************************************************/
void TBetaBase::applyGPUImageFilters(){

	if (bLearnBakground == true){
		camera.grabFrameToGPU(gpuBGTex);
		bLearnBakground = false;
	}

	GLuint processedTex;

	//tmp = contrastFilter->apply(gpuBGTex);
	processedTex = subtractFilter->apply(gpuSourceTex, gpuBGTex);

	if(bSmooth){//Smooth
		gaussHFilter->parameters["kernel_size"]->value = (float)smooth;
		gaussVFilter->parameters["kernel_size"]->value = (float)smooth;
		processedTex = gaussHFilter->apply(processedTex);
		processedTex = gaussVFilter->apply(processedTex);
	}

	if(bHighpass){//Highpass
		gaussHFilter2->parameters["kernel_size"]->value = (float)highpassBlur;
		gaussVFilter2->parameters["kernel_size"]->value = (float)highpassBlur;
		processedTex = gaussHFilter2->apply(processedTex);
		processedTex = gaussVFilter2->apply(processedTex);
		processedTex = subtractFilter2->apply(gaussVFilter->output_texture, processedTex);
	}

	if(bAmplify){}//amplify

	threshFilter->parameters["Threshold"]->value = (float)threshold / 255.0; //threshold
	threshFilter->apply(processedTex);

	//until the rest of the pipeline is fixed well just download the preprocessing result from the gpu and use that for the blob detection
	//TODO: make this part not super slow ;)
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, threshFilter->output_buffer);
	//glReadBuffer(gaussVFilter->output_buffer);
	glReadPixels(0,0,camera.camWidth, camera.camHeight, GL_RGB, GL_UNSIGNED_BYTE, gpuReadBackBuffer);
	gpuReadBackImage.setFromPixels(gpuReadBackBuffer, camera.camWidth, camera.camHeight);
	gpuReadBackImageGS = gpuReadBackImage;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}


void TBetaBase::applyImageFilters(){

	processedImg = camera.sourceImg;


	//Set Mirroring Horizontal/Vertical
	if(bVerticalMirror || bHorizontalMirror)processedImg.mirror(bVerticalMirror, bHorizontalMirror);

	if(!bFastMode)grayImg = processedImg; //for drawing

	//Dynamic background with learn rate...eventually learnrate will have GUI sliders
	if(bDynamicBG){
		learnBackground( processedImg, grayBg, fiLearn, fLearnRate);
	}

	//Capture full background
	if (bLearnBakground == true){
		bgCapture( processedImg );
		bLearnBakground = false;
	}

	processedImg.absDiff(grayBg, processedImg); //Background Subtraction

	if(bSmooth){//Smooth
		processedImg.blur((smooth * 2) + 1); //needs to be an odd number
		if(!bFastMode)subtractBg = processedImg; //for drawing
	}

	if(bHighpass){//HighPass
		processedImg.highpass(highpassBlur, highpassNoise);
		if(!bFastMode)highpassImg = processedImg; //for drawing
	}

	if(bAmplify){//Amplify
		processedImg.amplify(processedImg, highpassAmp);
		if(!bFastMode)ampImg = processedImg; //for drawing
	}

	processedImg.threshold(threshold); //Threshold
	if(!bFastMode)grayDiff = processedImg; //for drawing
}


void TBetaBase::resetGPUTextures(){


	delete gpuReadBackBuffer;
	gpuReadBackBuffer = new unsigned char[camera.camWidth*camera.camHeight*3];
	gpuReadBackImage.allocate(camera.camWidth, camera.camHeight);
	gpuReadBackImageGS.allocate(camera.camWidth, camera.camHeight);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gpuSourceTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  camera.camWidth, camera.camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, gpuBGTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  camera.camWidth, camera.camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	subtractFilter = new ImageFilter("filters/absSubtract.xml", camera.camWidth, camera.camHeight);
	subtractFilter2 = new ImageFilter("filters/subtract.xml", camera.camWidth, camera.camHeight);
	contrastFilter = new ImageFilter("filters/contrast.xml", camera.camWidth, camera.camHeight);
	gaussVFilter = new ImageFilter("filters/gaussV.xml", camera.camWidth, camera.camHeight);
	gaussHFilter = new ImageFilter("filters/gauss.xml", camera.camWidth, camera.camHeight);
	gaussVFilter2 = new ImageFilter("filters/gaussV2.xml", camera.camWidth, camera.camHeight);
	gaussHFilter2 = new ImageFilter("filters/gauss2.xml", camera.camWidth, camera.camHeight);
	threshFilter = new ImageFilter("filters/threshold.xml", camera.camWidth, camera.camHeight);
}



void TBetaBase::learnBackground( ofxCvGrayscaleImage & _giLive, ofxCvGrayscaleImage & _grayBg, ofxCvFloatImage & _fiLearn, float _fLearnRate )
{
	_fiLearn.addWeighted( _giLive, _fLearnRate);
    _grayBg = _fiLearn;

}
void TBetaBase::bgCapture( ofxCvGrayscaleImage & _giLive )
{
	learnBackground( _giLive, grayBg, fiLearn, 1.0f);
}

/******************************************************************************
 *							OTHER FUNCTIONS
 *****************************************************************************/

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

	camera.bcamera				= XML.getValue("CONFIG:CAMERA_0:USECAMERA",0);
	camera.deviceID			= XML.getValue("CONFIG:CAMERA_0:DEVICE",0);
	camera.camWidth			= XML.getValue("CONFIG:CAMERA_0:WIDTH",0);
	camera.camHeight			= XML.getValue("CONFIG:CAMERA_0:HEIGHT",0);
	camera.camRate				= XML.getValue("CONFIG:CAMERA_0:FRAMERATE",0);

	videoFileName		= XML.getValue("CONFIG:VIDEO:FILENAME", "RearDI.m4v");

	maxBlobs			= XML.getValue("CONFIG:BLOBS:MAXNUMBER", 20);

	bShowPressure		= XML.getValue("CONFIG:BOOLEAN:PRESSURE",0);

	bShowLabels			= XML.getValue("CONFIG:BOOLEAN:LABELS",0);
	bFastMode			= XML.getValue("CONFIG:BOOLEAN:FAST",0);
	bDrawOutlines		= XML.getValue("CONFIG:BOOLEAN:OUTLINES",0);
	bLearnBakground		= XML.getValue("CONFIG:BOOLEAN:LEARNBG",0);

	bVerticalMirror		= XML.getValue("CONFIG:BOOLEAN:VMIRROR",0);
	bHorizontalMirror	= XML.getValue("CONFIG:BOOLEAN:HMIRROR",0);

	//Filters
	bHighpass			= XML.getValue("CONFIG:BOOLEAN:HIGHPASS",1);
	bAmplify			= XML.getValue("CONFIG:BOOLEAN:AMPLIFY", 1);
	bSmooth				= XML.getValue("CONFIG:BOOLEAN:SMOOTH", 1);
	bDynamicBG			= XML.getValue("CONFIG:BOOLEAN:DYNAMICBG", 1);

	//GPU
	bGPUMode			= XML.getValue("CONFIG:BOOLEAN:GPU", 0);

	//Filter Settings
	threshold			= XML.getValue("CONFIG:INT:THRESHOLD",0);
	highpassBlur		= XML.getValue("CONFIG:INT:HIGHPASSBLUR",0);
	highpassNoise		= XML.getValue("CONFIG:INT:HIGHPASSNOISE",0);
	highpassAmp			= XML.getValue("CONFIG:INT:HIGHPASSAMP",0);
	smooth				= XML.getValue("CONFIG:INT:SMOOTH",0);

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

	XML.setValue("CONFIG:CAMERA_0:USECAMERA", camera.bcamera);
	XML.setValue("CONFIG:CAMERA_0:DEVICE", camera.deviceID);
	XML.setValue("CONFIG:CAMERA_0:WIDTH", camera.camWidth);
	XML.setValue("CONFIG:CAMERA_0:HEIGHT", camera.camHeight);
	XML.setValue("CONFIG:CAMERA_0:FRAMERATE", camera.camRate);

	XML.setValue("CONFIG:BOOLEAN:PRESSURE",bShowPressure);
	XML.setValue("CONFIG:BOOLEAN:LABELS",bShowLabels);
	XML.setValue("CONFIG:BOOLEAN:OUTLINES",bDrawOutlines);
	XML.setValue("CONFIG:BOOLEAN:LEARNBG",bLearnBakground);
	XML.setValue("CONFIG:BOOLEAN:TUIO",bTUIOMode);
	XML.setValue("CONFIG:BOOLEAN:VMIRROR",bVerticalMirror);
	XML.setValue("CONFIG:BOOLEAN:HMIRROR",bHorizontalMirror);

	XML.setValue("CONFIG:BOOLEAN:HIGHPASS", bHighpass);
	XML.setValue("CONFIG:BOOLEAN:AMPLIFY", bAmplify);
	XML.setValue("CONFIG:BOOLEAN:SMOOTH", bSmooth);
	XML.setValue("CONFIG:BOOLEAN:DYNAMICBG", bDynamicBG);

	XML.setValue("CONFIG:BOOLEAN:GPU", bGPUMode);

	XML.setValue("CONFIG:INT:THRESHOLD", threshold);
	XML.setValue("CONFIG:INT:HIGHPASSBLUR", highpassBlur);
	XML.setValue("CONFIG:INT:HIGHPASSNOISE",highpassNoise);
	XML.setValue("CONFIG:INT:HIGHPASSAMP",highpassAmp);
	XML.setValue("CONFIG:INT:SMOOTH", smooth);

	//	XML.setValue("CONFIG:NETWORK:LOCALHOST", *myTUIO.localHost);
	//	XML.setValue("CONFIG:NETWORK:TUIO_PORT_OUT",myTUIO.TUIOPort);

	XML.saveFile("config.xml");
}


///lets start moving stuff from the testApp.h and testApp.cpp

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

        if(bGPUMode){
            drawGLTexture(40, 30, 320, 240, gpuSourceTex);
            //subtractFilter->drawOutputTexture(85, 392, 128, 96);
            drawGLTexture(85, 392, 128, 96, gpuBGTex);
            gaussVFilter->drawOutputTexture(235, 392, 128, 96);
            subtractFilter2->drawOutputTexture(385, 392, 128, 96);
            threshFilter->drawOutputTexture(535, 392, 128, 96);
            gpuReadBackImageGS.draw(385, 30, 320, 240);
        }
        else
        {
            if(bShowPressure)
                pressureMap.draw(40, 30, 320, 240);
            else
                grayImg.draw(40, 30, 320, 240);

            grayDiff.draw(385, 30, 320, 240);
            fiLearn.draw(85, 392, 128, 96);
            subtractBg.draw(235, 392, 128, 96);
            highpassImg.draw(385, 392, 128, 96);
            ampImg.draw(535, 392, 128, 96);
        }

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

        if(camera.bcamera)
        {
            string str2 = "Camera Res:     ";
            str2+= ofToString(camera.vidGrabber.width, 0) + " x " + ofToString(camera.vidGrabber.height, 0)  + "\n";
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
            str2+= ofToString(camera.vidPlayer.width, 0) + " x " + ofToString(camera.vidPlayer.height, 0)  + "\n";
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

        if(camera.bcamera)
        {
            string str2 = "Camera Res:     ";
            str2+= ofToString(camera.vidGrabber.width, 0) + " x " + ofToString(camera.vidGrabber.height, 0)  + "\n";
            string str4 = "Camera FPS:     ";
            str4+= ofToString(fps, 0)+"\n";
            ofSetColor(0xFFFFFF);
            sidebarTXT.drawString(str + str2 + str4, 740, 410);
        }
        else
        {
            string str2 = "Video Res:       ";
            str2+= ofToString(camera.vidPlayer.width, 0) + " x " + ofToString(camera.vidPlayer.height, 0)  + "\n";
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
	//printf("b: %f, %f\n", b.centroid.x, b.centroid.y);
	 if(bTUIOMode)//If sending TUIO, add the blob to the map list
	 {
         //if blob is not otuside calibration mesh
         if(b.centroid.x != 0 && b.centroid.y != 0)
         myTUIO.blobs[b.id] = b;
	 }
}



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


	/********************
	 * Initalize Variables
	 *********************/
//	calibrationParticle.loadImage("images/particle.png");
//	calibrationParticle.setUseTexture(true);

	//Background Subtraction Learning Rate
	fLearnRate	= 0.0001f;
	//Intialize FPS variables
	frames		= 0;
	fps			= 0;
	lastFPSlog	= 0;
	//Calibration Booleans
	bCalibration= false;

	differenceTime = 0;

	bDrawVideo = true;
	bFullscreen = false;
	ofSetBackgroundAuto(false);

	//Load Settings from config.xml file
	loadXMLSettings();

	//Setup Window Properties
	ofSetWindowShape(winWidth,winHeight);
	ofSetFrameRate(camera.camRate * 1.4);			//This will be based on camera fps in the future
	ofSetVerticalSync(false);	            //Set vertical sync to false for better performance

	//Pick the Source - camera or video
	if(camera.bcamera)
	{
        activeInput = true;
		camera.vidGrabber.listDevices();
		camera.vidGrabber.setDeviceID(camera.deviceID);
		camera.vidGrabber.setVerbose(true);
        camera.vidGrabber.initGrabber(camera.camWidth,camera.camHeight);
		int grabW = camera.vidGrabber.width;
		int grabH = camera.vidGrabber.height;
		printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camera.camWidth, camera.camHeight, grabW, grabH);
	}
	else
	{
		activeInput = true;
		camera.vidPlayer.loadMovie("test_videos/" + videoFileName);
        camera.vidPlayer.play();
		printf("Video Mode\n");
		camera.camHeight = camera.vidPlayer.height;
		camera.camWidth = camera.vidPlayer.width;
	}

	/*****************************************************************************************************
	 * Allocate images (needed for drawing/processing images) ----Most of These won't be needed in the end
	 ******************************************************************************************************/
	processedImg.allocate(camera.camWidth, camera.camHeight); //main Image that'll be processed.
	processedImg.setUseTexture(false);
	camera.sourceImg.allocate(camera.camWidth, camera.camHeight);    //Source Image
	camera.sourceImg.setUseTexture(false);				//We don't need to draw this so don't create a texture

	//These images are used for drawing only
	grayImg.allocate(camera.camWidth, camera.camHeight);		//Gray Image
	grayBg.allocate(camera.camWidth, camera.camHeight);		//Background Image
	subtractBg.allocate(camera.camWidth, camera.camHeight);   //Background After subtraction
	grayDiff.allocate(camera.camWidth, camera.camHeight);		//Difference Image between Background and Source
	highpassImg.allocate(camera.camWidth, camera.camHeight);  //Highpass Image
	ampImg.allocate(camera.camWidth, camera.camHeight);		//Amplied Image
	fiLearn.allocate(camera.camWidth, camera.camHeight);		//ofxFloatImage used for simple dynamic background subtracti
	//	fiLearn.setUseTexture(false);
	pressureMap.allocate(camera.camWidth, camera.camHeight);	//Pressure Map Image

	/********************************************************************************************************/


	/**********************************************************/
	//GPU stuff initialization
	/**********************************************************/
	glGenTextures(1, &gpuSourceTex);
	glGenTextures(1, &gpuBGTex);

	//initialize texture once with glTexImage2D so we can use gltexSubImage2D afetrwards (fastser)
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gpuSourceTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  camera.camWidth, camera.camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, gpuBGTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  camera.camWidth, camera.camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	camera.grabFrameToGPU(gpuBGTex);

	//so very inefficient..but only for now..until i fix the gpu blob detection and geoemtry shader for variable length output
	gpuReadBackBuffer = new unsigned char[camera.camWidth*camera.camHeight*3];
	gpuReadBackImage.allocate(camera.camWidth, camera.camHeight);
	gpuReadBackImageGS.allocate(camera.camWidth, camera.camHeight);

	subtractFilter = new ImageFilter("filters/absSubtract.xml", camera.camWidth, camera.camHeight);
	subtractFilter2 = new ImageFilter("filters/subtract.xml", camera.camWidth, camera.camHeight);
	contrastFilter = new ImageFilter("filters/contrast.xml", camera.camWidth, camera.camHeight);
	gaussVFilter = new ImageFilter("filters/gaussV.xml", camera.camWidth, camera.camHeight);
	gaussHFilter = new ImageFilter("filters/gauss.xml", camera.camWidth, camera.camHeight);
	gaussVFilter2 = new ImageFilter("filters/gaussV2.xml", camera.camWidth, camera.camHeight);
	gaussHFilter2 = new ImageFilter("filters/gauss2.xml", camera.camWidth, camera.camHeight);
	threshFilter = new ImageFilter("filters/threshold.xml", camera.camWidth, camera.camHeight);

	/**********************************************************/

	//Fonts - Is there a way to dynamically change font size?
	verdana.loadFont("verdana.ttf", 8, true, true);	   //Font used for small images
	sidebarTXT.loadFont("verdana.ttf", 8, true, true);
	bigvideo.loadFont("verdana.ttf", 13, true, true);  //Font used for big images.

	//Static Images
	background.loadImage("images/background.jpg"); //Main (Temp?) Background

	gui = ofxGui::Instance(this);
	setupGUI();



    calib.setup(camera.camWidth, camera.camHeight, &tracker);


	printf("Tbeta pplication is setup!\n\n");
}


/******************************************************************************
 * The update function runs continuously. Use it to update states and variables
 *****************************************************************************/
void TBetaBase::update()
{
    bNewFrame = false;

	if(activeInput){

		if(camera.bcamera){
			camera.vidGrabber.grabFrame();
			bNewFrame = camera.vidGrabber.isFrameNew();
		}
		else{
			camera.vidPlayer.idleMovie();
			bNewFrame = camera.vidPlayer.isFrameNew();
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
				camera.grabFrameToGPU(gpuSourceTex);
				applyGPUImageFilters();
				contourFinder.findContours(gpuReadBackImageGS, 1, (camera.camWidth*camera.camHeight)/25, maxBlobs, false);
			}
			else{
				camera.grabFrameToCPU();
				applyImageFilters();
				contourFinder.findContours(processedImg, 1, (camera.camWidth*camera.camHeight)/25, maxBlobs, false);
			}

			//Track found contours/blobss
			tracker.track(&contourFinder);
			//get DSP time
			differenceTime = ofGetElapsedTimeMillis() - beforeTime;

            //Dynamic Background subtraction LearRate
			if(bDynamicBG){
				fLearnRate = 0.01f; //If there are no blobs, add the background faster.
				if(contourFinder.nBlobs > 0){ //If there ARE blobs, add the background slower.
					fLearnRate = 0.0003f;
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
		
		ofSetColor(180, 220, 180, 180);
		ofFill();
		ofRect(ofGetWidth() - 200,ofGetHeight() - 14, 200, 14);
		
		ofSetColor(0x000000);
		ofDrawBitmapString("|  ~  |tbeta.nuigroup.com", ofGetWidth() - 200, ofGetHeight() - 2);
		
		
    }
}

/*****************************************************************************
 * KEY EVENTS
 *****************************************************************************/
void TBetaBase::keyPressed(int key) {
	if(showConfiguration) {
		switch(key)
		{
			case 'b':
				bLearnBakground = true;
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
				bLearnBakground = true;
				break;
			case 'v':
				if(camera.bcamera)
					camera.vidGrabber.videoSettings();
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
            if(bFullscreen == false) ofToggleFullscreen(); bFullscreen = true; ofSetBackgroundAuto(true);
        }
    }

	if( key == '~' || key == '`') showConfiguration = !showConfiguration;
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
    if(showConfiguration) {
		gui->mousePressed(x, y, button); //guilistener
		if(x > ofGetWidth() - 200 && y > ofGetHeight() - 14) ofLaunchBrowser("http://tbeta.nuigroup.com/");	
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


