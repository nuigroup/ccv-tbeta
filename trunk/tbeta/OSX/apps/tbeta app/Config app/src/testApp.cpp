#include "testApp.h"
#include "gui.h"

/******************************************************************************
 * The setup function is run once to perform initializations in the application
 *****************************************************************************/
void testApp::setup()
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
	calibrationParticle.loadImage("images/particle.png");
	calibrationParticle.setUseTexture(true);

	//Background Subtraction Learning Rate
	fLearnRate	= 0.0001f;
	//Intialize FPS variables
	frames		= 0;
	fps			= 0;
	lastFPSlog	= 0;
	//Calibration Booleans
	bCalibration= false;
	bShowTargets = true;
	bW			= false;
	bA			= false;
	bS			= false;
	bD			= false;

	differenceTime = 0;

	bDrawVideo = true;
	bFullscreen = false;
	ofSetBackgroundAuto(false);

	//Load Settings from config.xml file
	loadXMLSettings();

	//Load Calibration Settings from calibration.xml file
	calibrate.setCamRes(camWidth, camHeight);
	calibrate.loadXMLSettings();

	//Setup Window Properties
	ofSetWindowShape(winWidth,winHeight);
	ofSetFrameRate(camRate * 1.4);			//This will be based on camera fps in the future
	ofSetVerticalSync(false);	            //Set vertical sync to false for better performance

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

	//These images are used for drawing only
	grayImg.allocate(camWidth, camHeight);		//Gray Image
	grayBg.allocate(camWidth, camHeight);		//Background Image
	subtractBg.allocate(camWidth, camHeight);   //Background After subtraction
	grayDiff.allocate(camWidth, camHeight);		//Difference Image between Background and Source
	highpassImg.allocate(camWidth, camHeight);  //Highpass Image
	ampImg.allocate(camWidth, camHeight);		//Amplied Image
	fiLearn.allocate(camWidth, camHeight);		//ofxFloatImage used for simple dynamic background subtracti
//	fiLearn.setUseTexture(false);
	pressureMap.allocate(camWidth, camHeight);	//Pressure Map Image

	/********************************************************************************************************/


	/**********************************************************/
	//GPU stuff initialization
	/**********************************************************/
	glGenTextures(1, &gpuSourceTex);
	glGenTextures(1, &gpuBGTex);

	//initialize texture once with glTexImage2D so we can use gltexSubImage2D afetrwards (fastser)
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gpuSourceTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  camWidth, camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, gpuBGTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  camWidth, camHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	grabFrameToGPU(gpuBGTex);

	//so very inefficient..but only for now..until i fix the gpu blob detection and geoemtry shader for variable length output
	gpuReadBackBuffer = new unsigned char[camWidth*camHeight*3];
	gpuReadBackImage.allocate(camWidth, camHeight);
	gpuReadBackImageGS.allocate(camWidth, camHeight);

	subtractFilter = new ImageFilter("filters/absSubtract.xml", camWidth, camHeight);
	subtractFilter2 = new ImageFilter("filters/subtract.xml", camWidth, camHeight);
	contrastFilter = new ImageFilter("filters/contrast.xml", camWidth, camHeight);
	gaussVFilter = new ImageFilter("filters/gaussV.xml", camWidth, camHeight);
	gaussHFilter = new ImageFilter("filters/gauss.xml", camWidth, camHeight);
	gaussVFilter2 = new ImageFilter("filters/gaussV2.xml", camWidth, camHeight);
	gaussHFilter2 = new ImageFilter("filters/gauss2.xml", camWidth, camHeight);
	threshFilter = new ImageFilter("filters/threshold.xml", camWidth, camHeight);

	/**********************************************************/

	//Fonts - Is there a way to dynamically change font size?
	verdana.loadFont("verdana.ttf", 8, true, true);	   //Font used for small images
	sidebarTXT.loadFont("verdana.ttf", 8, true, true);
	calibrationText.loadFont("verdana.ttf", 10, true, true);
	bigvideo.loadFont("verdana.ttf", 13, true, true);  //Font used for big images.

	//Static Images
	background.loadImage("images/background.jpg"); //Main (Temp?) Background

	gui = ofxGui::Instance(this);
	setupGUI();

	printf("Touchlib application is setup!\n");
}


/******************************************************************************
 * The update function runs continuously. Use it to update states and variables
 *****************************************************************************/
void testApp::update()
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
				grabFrameToGPU(gpuSourceTex);
				applyGPUImageFilters();
				contourFinder.findContours(gpuReadBackImageGS, 1, (camWidth*camHeight)/25, maxBlobs, false);
			}
			else{
				grabFrameToCPU();
				applyImageFilters();
				contourFinder.findContours(processedImg, 1, (camWidth*camHeight)/25, maxBlobs, false);
			}

			//Track found contours/blobs
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
void testApp::draw(){
	
	gui->draw();

    //temporary. only auto draw if in fullscreen
	if(bNewFrame && bFullscreen == false){drawToScreen();}
	else{drawToScreen();}
}

/*****************************************************************************
 * KEY EVENTS
 *****************************************************************************/
void testApp::keyPressed(int key)
{

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
			bCalibration && bShowTargets ? bShowTargets = false : bShowTargets = true;
			break;
		case 'g':
			bGPUMode ? bGPUMode = false : bGPUMode = true;
			gui->update(appPtr->gpuPanel_use, kofxGui_Set_Bool, &appPtr->bGPUMode, sizeof(bool));
			bLearnBakground = true;
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
			//bShowPressure ? bShowPressure = false : bShowPressure = true;
			break;
		case ' ':
			if(bFastMode)
			{
				bFastMode = false;
				//bToggleHelp = true;
				ofSetWindowShape(950,600); //default size
				//ofSetWindowTitle("Configuration");
			}
			else
			{
				bFastMode = true;
				//bToggleHelp = false;
				ofSetWindowShape(190,155); //minimized size
				//ofSetWindowTitle("Mini");
			}
			break;
		/***********************
		* Keys for Calibration
		***********************/
		case 'c': //Enter/Exit Calibration
			if(!bCalibration){
				bCalibration = true;
				if(bFullscreen == false) ofToggleFullscreen(); bFullscreen = true; ofSetBackgroundAuto(true);
			}
			else if(bCalibration && calibrate.bCalibrating)
				calibrate.bCalibrating = false;
			else if(bCalibration)
				calibrate.beginCalibration();
			break;
		case 'x': //Begin Calibrating
			if(bCalibration){
				bCalibration = false;
				if(bFullscreen == true) ofToggleFullscreen(); bFullscreen = false; ofSetBackgroundAuto(false);
			}
			break;
		case 'r': //Revert Calibration
			if(calibrate.bCalibrating)calibrate.revertCalibrationStep();
			break;
		case 's': //Down
			bS = true;
			if(!bCalibration) saveConfiguration();
			break;
		case 'a': //Left
			bA = true;
			break;
		case 'd': //Right
			bD = true;
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
			if(bCalibration)
			calibrate.GRID_X ++;
			if(calibrate.GRID_X > 16) calibrate.GRID_X = 16; calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
			calibrate.calibrationStep = 0;
			break;
		case '-':
			if(bCalibration)
			calibrate.GRID_X --;
			if(calibrate.GRID_X < 1) calibrate.GRID_X = 1; calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
			calibrate.calibrationStep = 0;
			break;
		case '+':
			if(bCalibration)
			calibrate.GRID_Y ++;
			if(calibrate.GRID_Y > 16) calibrate.GRID_Y = 16; calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
			calibrate.calibrationStep = 0;
			break;
		case '_':
			if(bCalibration)
			calibrate.GRID_Y --;
			if(calibrate.GRID_Y < 1) calibrate.GRID_Y = 1; calibrate.setGrid(calibrate.GRID_X, calibrate.GRID_Y);
			calibrate.calibrationStep = 0;
			break;
	}
}

void testApp::keyReleased(int key){

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
	}
}


/*****************************************************************************
*	MOUSE EVENTS
*****************************************************************************/
void testApp::mouseMoved(int x, int y)
{
}

void testApp::mouseDragged(int x, int y, int button)
{
	//gui listener
	gui->mouseDragged(x, y, button);
}

void testApp::mousePressed(int x, int y, int button)
{	//gui listener
	gui->mousePressed(x, y, button);
}

void testApp::mouseReleased()
{	//guilistener
	gui->mouseReleased(mouseX, mouseY, 0);
}


/*****************************************************************************
*	TOUCH EVENTS
*****************************************************************************/
void testApp::TouchDown( ofxTBetaCvBlob b)
{
	if(bCalibration)//If calibrating change target color when a finger is down
	downColor = 0x2FB5FF;

	if(bTUIOMode)//If sending TUIO, add the blob to the map list
	{
		calibrate.cameraToScreenPosition(b.centroid.x, b.centroid.y);
		calibrate.transformDimension(b.boundingRect.width, b.boundingRect.height);
		//if blob is not otuside calibration mesh
		if(b.centroid.x != 0 && b.centroid.y != 0)
		myTUIO.blobs[b.id] = b;
	}
}

void testApp::TouchUp( ofxTBetaCvBlob b)
{
	if(bCalibration && contourFinder.nBlobs == 0)//If calibrating change target color back when a finger is up
	downColor = 0xFF0000;

	if(calibrate.bCalibrating && contourFinder.nBlobs == 0)//If Calibrating, register the calibration point on blobOff
	{
		calibrate.cameraPoints[calibrate.calibrationStep] = vector2df(b.centroid.x, b.centroid.y);
		calibrate.nextCalibrationStep();

		printf("%d (%f, %f)\n", calibrate.calibrationStep, b.centroid.x, b.centroid.y);
	}

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

void testApp::TouchMoved( ofxTBetaCvBlob b)
{
	//printf("b: %f, %f\n", b.centroid.x, b.centroid.y);
	if(bTUIOMode)//If sending TUIO, add the blob to the map list
	{
		calibrate.cameraToScreenPosition(b.centroid.x, b.centroid.y);
		calibrate.transformDimension(b.boundingRect.width, b.boundingRect.height);
		//if blob is not otuside calibration mesh
		if(b.centroid.x != 0 && b.centroid.y != 0)
		myTUIO.blobs[b.id] = b;
	}
}

/*****************************************************************************
* ON EXIT
*****************************************************************************/
void testApp::exit()
{
	// -------------------------------- SAVE STATE ON EXIT
	saveConfiguration();
	calibrate.saveCalibration();


	printf("tBeta module has exited!\n");
}


void testApp::drawToScreen(){
	
    /*********************************
	 * IF CALIBRATING
	 *********************************/
    if(bCalibration)
    {
        //Don't draw main interface
        bShowInterface = false;
        doCalibration();
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
		
		gui->draw();
		
    }
	
    if(bShowInterface && !bFastMode) //IF DRAWING BLOB OUTLINES
    {
        drawFingerOutlines();
    }
}

