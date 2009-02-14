/*
 *  Cali.cpp
 *  tbeta
 *
 *  Created by Artem Titoulenko on 2/1/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */

#include "Calibration.h"

/******************************************************************************
 * The setup function is run once to perform initializations in the application
 *****************************************************************************/
void Calibration::setup(int _camWidth, int _camHeight, BlobTracker *trackerIn)
{
	/********************
	 * Initalize Variables
	 *********************/
	calibrationParticle.loadImage("images/particle.png");
	calibrationParticle.setUseTexture(true);

    calibrating = false;
	bShowTargets = true;
	bW			= false;
	bA			= false;
	bS			= false;
	bD			= false;

    //Fonts - Is there a way to dynamically change font size?
	verdana.loadFont("verdana.ttf", 8, true, true);	   //Font used for small images
	calibrationText.loadFont("verdana.ttf", 10, true, true);

	//Load Calibration Settings from calibration.xml file
	calibrate.setCamRes(_camWidth, _camHeight);
	calibrate.loadXMLSettings();

	tracker = trackerIn;
	tracker->passInCalibration(&calibrate);

	printf("Calibration is setup!\n");
}

void Calibration::passInContourFinder(int numBlobs, vector<ofxTBetaCvBlob> blobs) {

    contourFinder.nBlobs = numBlobs;
	contourFinder.blobs  = blobs;
}

void Calibration::passInTracker(BlobTracker *trackerIn) {
	tracker = trackerIn;
	tracker->passInCalibration(&calibrate);
}


/******************************
 *		  CALIBRATION
 *******************************/
void Calibration::doCalibration(){

	//Change the background color to black
	ofSetColor(0x000000);
	ofFill();
	ofRect(0, 0, ofGetWidth(), ofGetHeight());

    //Draw Calibration Screen
    drawCalibrationPointsAndBox();
    //Draw blobs in calibration to see where you are touching
	if(!calibrate.bCalibrating) drawCalibrationBlobs();

	/************************************
	 * Onscreen Calibration Instructions
	 ************************************/
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

void Calibration::drawCalibrationPointsAndBox(){

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

void Calibration::drawCalibrationBlobs(){

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
		//     NOT SURE I WANT TO KEEP THIS. DON'T THINK IT'S USEFUL

		/* ofSetColor(0xFFFFFF);
		 glLineWidth(1);
		 char idStr[1024];
		 sprintf(idStr, "id: %i \n x: %f \n y: %f",drawBlob2.id, ceil(drawBlob2.centroid.x * ofGetWidth()),
		 ceil(drawBlob2.centroid.y * ofGetHeight()));
		 verdana.drawString(idStr, drawBlob2.centroid.x * ofGetWidth() - drawBlob2.boundingRect.width/2 + 40,
		 drawBlob2.centroid.y * ofGetHeight() - drawBlob2.boundingRect.height/2 + 40);
        */
    }
}

/*****************************************************************************
 * TOUCH EVENTS
 *****************************************************************************/
void Calibration::RAWTouchDown( ofxTBetaCvBlob b)
{
	 downColor = 0x2FB5FF;
}

void Calibration::RAWTouchUp( ofxTBetaCvBlob b)
{
	if(b.simulated && contourFinder.nBlobs == 1) {
		calibrate.cameraPoints[calibrate.calibrationStep] = vector2df(b.centroid.x, b.centroid.y);
		calibrate.nextCalibrationStep();
	} else if(contourFinder.nBlobs == 1)//If calibrating change target color back when a finger is up
        downColor = 0xFF0000;

	 if(calibrate.bCalibrating && contourFinder.nBlobs == 1)//If Calibrating, register the calibration point on blobOff
	 {
         calibrate.cameraPoints[calibrate.calibrationStep] = vector2df(b.centroid.x, b.centroid.y);
         calibrate.nextCalibrationStep();

         if(calibrate.calibrationStep != 0)
            printf("%d (%f, %f)\n", calibrate.calibrationStep, b.centroid.x, b.centroid.y);

         if(calibrate.calibrationStep == 0){
            printf("%d (%f, %f)\n", calibrate.GRID_POINTS, b.centroid.x, b.centroid.y);
            printf("Calibration complete\n");
         }
	 }
}

void Calibration::RAWTouchMoved( ofxTBetaCvBlob b)
{
}


/*****************************************************************************
 * KEY EVENTS
 *****************************************************************************/
void Calibration::keyPressed(int key) {

	if(calibrating)
	{
        switch(key)
        {
            case 't':
                bShowTargets ? bShowTargets = false : bShowTargets = true;
                break;
                /***********************
                 * Keys for Calibration
                 ***********************/
            case 'c': //Enter/Exit Calibration
                if(calibrate.bCalibrating) {
                    calibrate.bCalibrating = false;
                    printf("Calibration Stoped\n");
				} else {
					calibrate.beginCalibration();
                    printf("Calibration Started\n");
				}
                break;
            case 'r': //Revert Calibration
                if(calibrate.bCalibrating)calibrate.revertCalibrationStep();
                break;
            case 'a': //Left
                bA = true;
                break;
            case 'd': //Right
                bD = true;
                break;
            case 'w': //Right
                bW = true;
                break;
            case 's': //Right
                bS = true;
                break;
            case OF_KEY_RIGHT: //Move bounding box right
                if(bD){
                    calibrate.screenBB.lowerRightCorner.X += .001;
                    if(calibrate.screenBB.lowerRightCorner.X > 1) calibrate.screenBB.lowerRightCorner.X = 1;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }else if(bA){
                    calibrate.screenBB.upperLeftCorner.X += .001;
                    if(calibrate.screenBB.upperLeftCorner.X > 1) calibrate.screenBB.upperLeftCorner.X = 1;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }else{
                    calibrate.screenBB.lowerRightCorner.X += .001;
                    if(calibrate.screenBB.lowerRightCorner.X > 1) calibrate.screenBB.lowerRightCorner.X = 1;
                    calibrate.screenBB.upperLeftCorner.X += .001;
                    if(calibrate.screenBB.upperLeftCorner.X > 1) calibrate.screenBB.upperLeftCorner.X = 1;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }
                break;
            case OF_KEY_LEFT: //Move bounding box left
                if(bD){
                    calibrate.screenBB.lowerRightCorner.X -= .001;
                    if(calibrate.screenBB.lowerRightCorner.X < 0) calibrate.screenBB.lowerRightCorner.X = 0;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }else if(bA){
                    calibrate.screenBB.upperLeftCorner.X -= .001;
                    if(calibrate.screenBB.upperLeftCorner.X < 0) calibrate.screenBB.upperLeftCorner.X = 0;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }else{
                    calibrate.screenBB.lowerRightCorner.X -= .001;
                    if(calibrate.screenBB.lowerRightCorner.X < 0) calibrate.screenBB.lowerRightCorner.X = 0;
                    calibrate.screenBB.upperLeftCorner.X -= .001;
                    if(calibrate.screenBB.upperLeftCorner.X < 0) calibrate.screenBB.upperLeftCorner.X = 0;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }
                break;
            case OF_KEY_DOWN: //Move bounding box down
                if(bS){
                    calibrate.screenBB.lowerRightCorner.Y += .001;
                    if(calibrate.screenBB.lowerRightCorner.Y > 1) calibrate.screenBB.lowerRightCorner.Y = 1;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }else if(bW){
                    calibrate.screenBB.upperLeftCorner.Y += .001;
                    if(calibrate.screenBB.upperLeftCorner.Y > 1) calibrate.screenBB.upperLeftCorner.Y = 1;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }else{
                    calibrate.screenBB.lowerRightCorner.Y += .001;
                    if(calibrate.screenBB.lowerRightCorner.Y > 1) calibrate.screenBB.lowerRightCorner.Y = 1;
                    calibrate.screenBB.upperLeftCorner.Y += .001;
                    if(calibrate.screenBB.upperLeftCorner.Y > 1) calibrate.screenBB.upperLeftCorner.Y = 1;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }
                break;
            case OF_KEY_UP: //Move bounding box up
                if(bS){
                    calibrate.screenBB.lowerRightCorner.Y -= .001;
                    if(calibrate.screenBB.lowerRightCorner.Y < 0) calibrate.screenBB.lowerRightCorner.Y = 0;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }else if(bW){
                    calibrate.screenBB.upperLeftCorner.Y -= .001;
                    if(calibrate.screenBB.upperLeftCorner.Y < 0) calibrate.screenBB.upperLeftCorner.Y = 0;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }else{
                    calibrate.screenBB.lowerRightCorner.Y -= .001;
                    if(calibrate.screenBB.lowerRightCorner.Y < 0) calibrate.screenBB.lowerRightCorner.Y = 0;
                    calibrate.screenBB.upperLeftCorner.Y -= .001;
                    if(calibrate.screenBB.upperLeftCorner.Y < 0) calibrate.screenBB.upperLeftCorner.Y = 0;
                    calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                    calibrate.calibrationStep = 0;
                }
                break;
                //Start Grid Point Changing
            case '=':
                calibrate.GRID_X ++;
                if(calibrate.GRID_X > 16) calibrate.GRID_X = 16; calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                calibrate.calibrationStep = 0;
                break;
            case '-':
                calibrate.GRID_X --;
                if(calibrate.GRID_X < 1) calibrate.GRID_X = 1; calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                calibrate.calibrationStep = 0;
                break;
            case '+':
                calibrate.GRID_Y ++;
                if(calibrate.GRID_Y > 16) calibrate.GRID_Y = 16; calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                calibrate.calibrationStep = 0;
                break;
            case '_':
                calibrate.GRID_Y --;
                if(calibrate.GRID_Y < 1) calibrate.GRID_Y = 1; calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
                calibrate.calibrationStep = 0;
                break;
        }
    }
}

void Calibration::keyReleased(int key){

	if(calibrating)
	{
		switch(key)
		{
			case 'w':
				bW = false;
				break;
			case 's':
				bS = false;
				break;
			case 'a':
				bA = false;
				break;
			case 'd':
				bD = false;
				break;
			case '/':
				if(calibrating) {
					ofxTBetaCvBlob tempBlob;
					tempBlob.centroid.x = mouseX;
					tempBlob.centroid.y = mouseY;
					tempBlob.simulated = true;
					RAWTouchUp(tempBlob);
				}
				break;
            case 'x': //Begin Calibrating
                tracker->passInCalibration(&calibrate);
                break;
			case OF_KEY_RIGHT: //Move bounding box right
                calibrate.computeCameraToScreenMap();
                tracker->passInCalibration(&calibrate);
                break;
            case OF_KEY_LEFT: //Move bounding box left
                calibrate.computeCameraToScreenMap();
                tracker->passInCalibration(&calibrate);
                break;
            case OF_KEY_DOWN: //Move bounding box down
                calibrate.computeCameraToScreenMap();
                tracker->passInCalibration(&calibrate);
                break;
            case OF_KEY_UP: //Move bounding box up
                calibrate.computeCameraToScreenMap();
                tracker->passInCalibration(&calibrate);
                break;
		}
	}
}

/*****************************************************************************
 *	MOUSE EVENTS
 *****************************************************************************/
void Calibration::mouseMoved(int x, int y)
{
}

void Calibration::mouseDragged(int x, int y, int button)
{
}

void Calibration::mousePressed(int x, int y, int button)
{

}

void Calibration::mouseReleased()
{

}
