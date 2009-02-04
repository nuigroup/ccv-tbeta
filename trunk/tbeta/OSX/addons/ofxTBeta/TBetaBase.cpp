/*
 *  TBetaBase.cpp
 *  tbeta
 *
 *  Created by Artem Titoulenko on 2/1/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */

#include "TBetaBase.h"

TBetaBase::TBetaBase() {
	TouchEvents.addListener(this);
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
			
			//           ofSetColor(0x0099FF); //Make Plus Sign
			//           ofRect((drawBlob.centroid.x * (MAIN_WINDOW_WIDTH/camWidth)) + 40, (drawBlob.centroid.y * (MAIN_WINDOW_HEIGHT/camHeight)) - drawBlob.boundingRect.height + 30, .5, drawBlob.boundingRect.height * 2); //Horizontal Plus
			//           ofRect((drawBlob.centroid.x * (MAIN_WINDOW_WIDTH/camWidth)) - drawBlob.boundingRect.width + 40, (drawBlob.centroid.y  * (MAIN_WINDOW_HEIGHT/camHeight)) + 30, drawBlob.boundingRect.width * 2, .5); //Vertical Plus
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


/************************************************
 *				FILTERS
 ************************************************/
void TBetaBase::applyGPUImageFilters(){
	
	if (bLearnBakground == true){
		grabFrameToGPU(gpuBGTex);
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
	glReadPixels(0,0,camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, gpuReadBackBuffer);
	gpuReadBackImage.setFromPixels(gpuReadBackBuffer, camWidth, camHeight);
	gpuReadBackImageGS = gpuReadBackImage;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
}


void TBetaBase::applyImageFilters(){
	
	processedImg = sourceImg;
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
	gpuReadBackBuffer = new unsigned char[camWidth*camHeight*3];
	gpuReadBackImage.allocate(camWidth, camHeight);
	gpuReadBackImageGS.allocate(camWidth, camHeight);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gpuSourceTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  camWidth, camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, gpuBGTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  camWidth, camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	
	subtractFilter = new ImageFilter("filters/absSubtract.xml", camWidth, camHeight);
	subtractFilter2 = new ImageFilter("filters/subtract.xml", camWidth, camHeight);
	contrastFilter = new ImageFilter("filters/contrast.xml", camWidth, camHeight);
	gaussVFilter = new ImageFilter("filters/gaussV.xml", camWidth, camHeight);
	gaussHFilter = new ImageFilter("filters/gauss.xml", camWidth, camHeight);
	gaussVFilter2 = new ImageFilter("filters/gaussV2.xml", camWidth, camHeight);
	gaussHFilter2 = new ImageFilter("filters/gauss2.xml", camWidth, camHeight);
	threshFilter = new ImageFilter("filters/threshold.xml", camWidth, camHeight);
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

/******************************
 *		  CALIBRATION
 *******************************/
void TBetaBase::doCalibration(){
	
	//Change the background color to black
	ofSetColor(0x000000);
	ofFill();
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
	
    //Draw Calibration Screen
    drawCalibrationPointsAndBox();
    //Draw blobs in calibration to see where you are touching
	if(!calibrate.bCalibrating) drawCalibrationBlobs();
	
	/******************************
	 * Onscreen Calibration Instructions
	 ******************************/
	ofSetColor(0xFF00FF);
	//ofSetWindowTitle("Calibration");
	char reportStr[10240];
	calibrationText.setLineHeight(20.0f);
	
	if(calibrate.bCalibrating){
		sprintf(reportStr,
				"CALIBRATING: \n\n-Touch current circle target and lift up to calibrate point \n-Press [b] to recapture background (if there's false blobs) \n-Press [r] to go back to previous point(s) \n");
		calibrationText.drawString(reportStr, ofGetWidth()/2 - calibrationText.stringWidth(reportStr)/2, ofGetHeight()/2 - calibrationText.stringHeight(reportStr)/2);
	}else
	{
		sprintf(reportStr,
				"CALIBRATION \n\n-Press [c] to start calibrating \n-Press [x] to return main screen \n-Press [b] to recapture background \n-Press [t] to toggle blob targets \n\nCHANGING GRID SIZE (number of points): \n\n-Current Grid Size is %i x %i \n-Press [+]/[-] to add/remove points on X axis \n-Press [shift][+]/[-] to add/remove points on Y axis \n\nALINGING BOUNDING BOX TO PROJECTION SCREEN: \n\n-Use arrow keys to move bounding box\n-Press and hold [w],[a],[s],[d] (top, left, bottom, right) and arrow keys to adjust each side\n", calibrate.GRID_X + 1, calibrate.GRID_Y + 1);
		calibrationText.drawString(reportStr, ofGetWidth()/2 - calibrationText.stringWidth(reportStr)/2, ofGetHeight()/2 - calibrationText.stringHeight(reportStr)/2);
	}
}

void TBetaBase::drawCalibrationPointsAndBox(){
	
    //Get the screen points so we can make a grid
	vector2df *screenpts = calibrate.getScreenPoints();
	
	int i;
	//For each grid point
	for(i=0; i<calibrate.GRID_POINTS; i++)
	{
		//If calibrating, draw a red circle around the active point
		if(calibrate.calibrationStep == i && calibrate.bCalibrating)
		{
			glPushMatrix();
			glTranslatef(screenpts[i].X * ofGetWidth(), screenpts[i].Y * ofGetHeight(), 0.0f);
			//draw Circle
			ofFill();
			ofSetColor(downColor);
			ofCircle(0.f, 0.f, 40);
			//Cutout Middle of circle
			ofSetColor(0x000000);
			ofCircle(0.f, 0.f, 25);
			glPopMatrix();
		}
		ofSetColor(0x00FF00); //Make Plus Sign
		ofRect(screenpts[i].X * ofGetWidth() - 2, screenpts[i].Y * ofGetHeight() - 10, 4, 20); //Horizontal Plus
		ofRect(screenpts[i].X * ofGetWidth() - 10, screenpts[i].Y * ofGetHeight() - 2, 20, 4); //Vertical Plus
	}
	//Draw Bounding Box
	ofSetColor(0xFFFFFF);
	ofNoFill();
	ofRect(calibrate.screenBB.upperLeftCorner.X * ofGetWidth(), calibrate.screenBB.upperLeftCorner.Y * ofGetHeight(),
		   calibrate.screenBB.getWidth() * ofGetWidth(), calibrate.screenBB.getHeight() * ofGetHeight());
}

void TBetaBase::drawCalibrationBlobs(){
	
    //Find the blobs
    for(int i=0; i<contourFinder.nBlobs; i++)
    {
        //temp blob to rescale and draw on screen
        ofxTBetaCvBlob drawBlob2;
        drawBlob2 = contourFinder.blobs[i];
		
        //transform height/width to calibrated space
        calibrate.transformDimension(drawBlob2.boundingRect.width, drawBlob2.boundingRect.height);
        drawBlob2.boundingRect.width *= calibrate.screenBB.getWidth() * ofGetWidth() * 4;
        drawBlob2.boundingRect.height *= calibrate.screenBB.getHeight() * ofGetHeight() * 4 ;
		
        //transform x/y position to calibrated space
        calibrate.cameraToScreenPosition(drawBlob2.centroid.x, drawBlob2.centroid.y);
		
        //Get a random color for each blob
        if(blobcolor[drawBlob2.id] == 0)
        {
            int r = ofRandom(0, 255);
            int g = ofRandom(0, 255);
            int b = ofRandom(0, 255);
            //Convert to hex
            int rgbNum = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
            //Set hex into map position
            blobcolor[drawBlob2.id] = rgbNum;
        }
		
        //Draw Fuzzy Circles
        ofEnableAlphaBlending();
        ofImage tempCalibrationParticle;
        tempCalibrationParticle.clone(calibrationParticle);
        ofSetColor(blobcolor[drawBlob2.id]);
        tempCalibrationParticle.draw(drawBlob2.centroid.x * ofGetWidth() - drawBlob2.boundingRect.width * .625, drawBlob2.centroid.y * ofGetHeight() - drawBlob2.boundingRect.height * .625,
									 drawBlob2.boundingRect.width * 1.25, drawBlob2.boundingRect.height * 1.25);
        ofDisableAlphaBlending();
		
        //Draw Blob Targets
        if(bShowTargets)
        {
            ofSetColor(0xFFFFFF);
            glLineWidth(5);
            glPushMatrix();
			//	glLoadIdentity();
            glTranslatef(drawBlob2.centroid.x * ofGetWidth(), ((drawBlob2.centroid.y * ofGetHeight())), 0);
			//  ofEllipse(0, 0, drawBlob2.boundingRect.width/2, drawBlob2.boundingRect.height/2);
            ofLine(0, -drawBlob2.boundingRect.height/2, 0, drawBlob2.boundingRect.height/2);
            ofLine(-drawBlob2.boundingRect.width/2, 0, drawBlob2.boundingRect.width/2, 0);
            glPopMatrix();
        }
        //set line width back to normal
        glLineWidth(1);
		
        //Displat Text of blob information
		/*      NOT SURE I WANT TO KEEP THIS. DON'T THINK IT'S USEFUL
		 
		 ofSetColor(0xFFFFFF);
		 glLineWidth(1);
		 char idStr[1024];
		 sprintf(idStr, "id: %i \n x: %f \n y: %f",drawBlob2.id, ceil(drawBlob2.centroid.x * ofGetWidth()),
		 ceil(drawBlob2.centroid.y * ofGetHeight()));
		 verdana.drawString(idStr, drawBlob2.centroid.x * ofGetWidth() - drawBlob2.boundingRect.width/2 + 40,
		 drawBlob2.centroid.y * ofGetHeight() - drawBlob2.boundingRect.height/2 + 40);
		 */
    }
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
	
	bShowPressure		= XML.getValue("CONFIG:BOOLEAN:PRESSURE",0);
	
	bShowLabels			= XML.getValue("CONFIG:BOOLEAN:LABELS",0);
	bSnapshot			= XML.getValue("CONFIG:BOOLEAN:SNAPSHOT",0);
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
	
	XML.setValue("CONFIG:CAMERA_0:USECAMERA", bcamera);
	XML.setValue("CONFIG:CAMERA_0:DEVICE", deviceID);
	XML.setValue("CONFIG:CAMERA_0:WIDTH", camWidth);
	XML.setValue("CONFIG:CAMERA_0:HEIGHT", camHeight);
	XML.setValue("CONFIG:CAMERA_0:FRAMERATE", camRate);
	
	XML.setValue("CONFIG:BOOLEAN:PRESSURE",bShowPressure);
	XML.setValue("CONFIG:BOOLEAN:LABELS",bShowLabels);
	XML.setValue("CONFIG:BOOLEAN:SNAPSHOT",bSnapshot);
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

