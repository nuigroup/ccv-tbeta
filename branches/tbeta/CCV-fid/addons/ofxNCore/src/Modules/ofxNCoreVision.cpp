/*
*  ofxNCoreVision.cpp
*  NUI Group Community Core Vision
*
*  Created by NUI Group Dev Team A on 2/1/09.
*  Copyright 2009 NUI Group. All rights reserved.
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
	
	//create filter
	if ( filter == NULL ){filter = new ProcessFilters();}
	//fiducial Filters------------------------------------------------------------------------
	if ( filter_fiducial == NULL ){filter_fiducial = new ProcessFilters();}
	
	//---------added by Stefan Schlupek-----------
	
	//Load Settings from config.xml file
	loadXMLSettings();

	//GLUT Bug: http://www.mathies.com/glfaq/GLToolkitFAQ.html#II 
    //removes the 'x' button on windows which causes a crash due to a GLUT bug
	#ifdef TARGET_WIN32
		//Get rid of 'x' button
		HWND hwndConsole = FindWindowA(NULL, " Community Core Vision ");
		HMENU hMnu = ::GetSystemMenu(hwndConsole, FALSE);
		RemoveMenu(hMnu, SC_CLOSE, MF_BYCOMMAND);
	#endif

	
	if(printfToFile) {
		/*****************************************************************************************************
		* LOGGING
		******************************************************************************************************/
		/* alright first we need to get time and date so our logs can be ordered */
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		strftime (fileName,80,"../logs/log_%B_%d_%y_%H_%M_%S.txt",timeinfo);
		FILE *stream ;
		sprintf(fileName, ofToDataPath(fileName).c_str());
		if((stream = freopen(fileName, "w", stdout)) == NULL){}
		/******************************************************************************************************/
	}
	
	printf("printfToFile %i!\n", printfToFile);
	printf("Undistort %i!\n", bUndistort);

	

	//Setup Window Properties
	ofSetWindowShape(winWidth,winHeight);
	ofSetVerticalSync(false);	            //Set vertical sync to false for better performance?
	
	printf("freedom?\n");

	//load camera/video
	initDevice();
	printf("freedom2?\n");

	//set framerate
	ofSetFrameRate(camRate * 1.3);			//This will be based on camera fps in the future

	/*****************************************************************************************************
	* Allocate images (needed for drawing/processing images)
	******************************************************************************************************/
	processedImg.allocate(camWidth, camHeight); //main Image that'll be processed.
	processedImg.setUseTexture(false);			//We don't need to draw this so don't create a texture
	sourceImg.allocate(camWidth, camHeight);    //Source Image
	sourceImg.setUseTexture(false);				//We don't need to draw this so don't create a texture
	/******************************************************************************************************/

	/*****************************************************************************************************
	* Allocate Fiducial_images 
	******************************************************************************************************/
	//---------added by Stefan Schlupek-----------
	processedImg_fiducial.allocate(camWidth, camHeight); //main Image that'll be processed.
	processedImg_fiducial.setUseTexture(false);			//We don't need to draw this so don't create a texture
	printf("camWidth:%f camHeight:%f \n",camWidth ,camHeight);
	undistortedImg.allocate(camWidth, camHeight);

	//
	processedImg.allocate(camWidth, camHeight); //main Image that'll be processed.
	//---------------------------

	//Fonts - Is there a way to dynamically change font size?
	verdana.loadFont("verdana.ttf", 8, true, true);	   //Font used for small images
	bigvideo.loadFont("verdana.ttf", 13, true, true);  //Font used for big images.

	//Static Images
	background.loadImage("images/background.jpg"); //Main (Temp?) Background

	printf("freedom3?\n");
	//GUI Controls
	controls = ofxGui::Instance(this);
	setupControls();
	
	printf("freedom4?\n");

	//Setup Calibration
	calib.setup(camWidth, camHeight, &tracker);

	//added by Stefan Schlupek
	calib.calibrate.init_UDP_calibrate_connection(calibration_udp_host,calibration_udp_port);
	//-----

	//Allocate Filters
	filter->allocate( camWidth, camHeight );
	//Allocate Fiducial_Filters
	filter_fiducial->allocate( camWidth, camHeight );
	//the filters need this Information for right backround Initialization at the different modes
	filter->backgroundMode= backgroundMode;
	filter_fiducial->backgroundMode= backgroundMode;
	//---------added by Stefan Schlupek-----------

	/*****************************************************************************************************
	* Startup Modes
	******************************************************************************************************/
	//If Standalone Mode (not an addon)
	if (bStandaloneMode)
	{
		printf("Starting in standalone mode...\n\n");
		showConfiguration = true;
	}
	if (bMiniMode)
	{
		showConfiguration = true;
		bShowInterface = false;
		printf("Starting in Mini Mode...\n\n");
		ofSetWindowShape(190, 200); //minimized size
		filter->bMiniMode = bMiniMode;
		//------------------------------------------
		filter_fiducial->bMiniMode = bMiniMode;
		//---------added by Stefan Schlupek-----------
	}
	else{
		bShowInterface = true;
		printf("Starting in full mode...\n\n");
	}
	
	#ifdef TARGET_WIN32
	    //get rid of the console window
        //FreeConsole();
	#endif

	printf("Community Core Vision is setup!\n\n");


//----added by Stefan Schlupek
//load a background Image as reference for Image Processing
if(backgroundMode == "LOAD" )loadBackground();


	/****************************************************************
	*	FiducialFinder Settings
	****************************************************************/
	//detect finger is off by default
	//fidfinder.detectFinger= true;
	fidfinder.maxFingerSize = 25;
	fidfinder.minFingerSize = 5;
	fidfinder.fingerSensitivity = 0.05f;//from 0 to 2.0f
	

	// factor for Fiducial Drawing. The ImageSize is hardcoded 320x240 Pixel!(Look at ProcessFilters.h at the draw() Method
	fiducialDrawFactor_Width = 320 / static_cast<float>(filter->camWidth);//camWidth;
	fiducialDrawFactor_Height = 240 / static_cast<float>(filter->camHeight);//camHeight;

	if(fiducialTreePath != ""){
		fidfinder.initTree( fiducialTreePath.c_str() );
		}

	myTUIO.setFiducialModus(bTrackFiducials);//TUIO Sender flag to avoid udp traffic if not needed
	//--------------------------

	//UDP Alive Connections
	ofAddListener(udp_alive_connection.onUDP_makeAndSaveBackground,this,&ofxNCoreVision::makeAndSaveBackground);
	udp_alive_connection.init_Connections();// = new UDP_Alive(myTUIO.host_name,);//(string _host, int port_in,int port_out,string host_out);
	printf("udp_alive_connection host %s, in port %i, out port %i, outHost %s\n",udp_alive_connection.host.c_str(),udp_alive_connection.udp_in_port,udp_alive_connection.udp_out_port,udp_alive_connection.udp_out_host.c_str());
	//Sleep(2000);
}
				
/****************************************************************
*	Load/Save config.xml file Settings
****************************************************************/
void ofxNCoreVision::loadXMLSettings()
{
	// TODO: a seperate XML to map keyboard commands to action
	message = "Loading config.xml...";
	// Can this load via http?
	if ( XML.loadFile("data/config.xml")){
		message = "Settings Loaded!\n\n";}
	else{
		message = "No Settings Found...\n\n"; //FAIL
	}
	cout << message << endl;
	
	//--------------------------------------------------------------
	//  START BINDING XML TO VARS
	//--------------------------------------------------------------
	
	bcamera						= XML.getValue("CONFIG:CAMERA_0:USECAMERA", 1);
	deviceModel					= XML.getValue("CONFIG:CAMERA_0:DEVICE_MODEL", "");			
	deviceID					= XML.getValue("CONFIG:CAMERA_0:DEVICE", 0);
	camWidth					= XML.getValue("CONFIG:CAMERA_0:WIDTH", 320);
	camHeight					= XML.getValue("CONFIG:CAMERA_0:HEIGHT", 240);
	camRate						= XML.getValue("CONFIG:CAMERA_0:FRAMERATE", 0);
	videoFileName				= XML.getValue("CONFIG:VIDEO:FILENAME", "test_videos/RearDI.m4v");
	maxBlobs					= XML.getValue("CONFIG:BLOBS:MAXNUMBER", 20);
	bShowLabels					= XML.getValue("CONFIG:BOOLEAN:LABELS",0);
	bDrawOutlines				= XML.getValue("CONFIG:BOOLEAN:OUTLINES",0);
	//bAutoBackground				= XML.getValue("CONFIG:BOOLEAN:LEARNBG",0);
	backgroundMode				= XML.getValue("CONFIG:BACKGROUND:STARTUPMODE","AUTO");
	backgroundImagePath			= XML.getValue("CONFIG:BACKGROUND:IMAGE","");
	filter->bLearnBackground	=	backgroundMode == "AUTO";//= XML.getValue("CONFIG:BOOLEAN:LEARNBG",0);
	filter->bVerticalMirror		= XML.getValue("CONFIG:BOOLEAN:VMIRROR",0);
	filter->bHorizontalMirror	= XML.getValue("CONFIG:BOOLEAN:HMIRROR",0);

	//fmv settings
	ffmvMemoryChannel			= XML.getValue("CONFIG:FFMV:MEMORY_CHANNEL",0);
	
	//Logging
	printfToFile				= XML.getValue("CONFIG:BOOLEAN:PRINTFTOFILE",0);
	
	//Filters
	filter->bTrackDark			= XML.getValue("CONFIG:BOOLEAN:TRACKDARK", 0);
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
	//NETWORK SETTINGS
	bTUIOMode					= XML.getValue("CONFIG:BOOLEAN:TUIO",0);
	myTUIO.bOSCMode				= XML.getValue("CONFIG:BOOLEAN:OSCMODE",1);
	myTUIO.bTCPMode				= XML.getValue("CONFIG:BOOLEAN:TCPMODE",1);
	myTUIO.bHeightWidth			= XML.getValue("CONFIG:BOOLEAN:HEIGHTWIDTH",0);
	tmpLocalHost				= XML.getValue("CONFIG:NETWORK:LOCALHOST", "localhost");
	tmpPort						= XML.getValue("CONFIG:NETWORK:TUIOPORT_OUT", 3333); 
	tmpFlashPort				= XML.getValue("CONFIG:NETWORK:TUIOFLASHPORT_OUT", 3000);				
	networkInterface			= XML.getValue("CONFIG:NETWORK:NETWORK_INTERFACE_NR", 0);

	myTUIO.setup(tmpLocalHost.c_str(), tmpPort, tmpFlashPort,camWidth,camHeight); //have to convert tmpLocalHost to a const char*	
	myTUIO.setNetworkInterface	(networkInterface);

	udp_alive_connection.udp_in_port = XML.getValue("CONFIG:NETWORK:UDP_ALIVE:IN_PORT", 100000);
	udp_alive_connection.udp_out_port = XML.getValue("CONFIG:NETWORK:UDP_ALIVE:OUT_PORT", 100001);
	udp_alive_connection.udp_out_host = XML.getValue("CONFIG:NETWORK:UDP_ALIVE:OUT_HOST", "127.0.0.2");
	
	udp_alive_connection.host = myTUIO.host_name;
	

	bUndistort					= XML.getValue("CONFIG:BOOLEAN:UNDISTORT", 0);

	//--------------------------------------------------------------
	bTrackFiducials						= XML.getValue("CONFIG:FIDUCIAL:TRACKING",0);
	fiducialTreePath					= XML.getValue("CONFIG:FIDUCIAL:TREE_FILE","");

	//Fiducial_Filters
	filter_fiducial->bLearnBackground	= filter->bLearnBackground;//XML.getValue("LEARNBG",0);
	filter_fiducial->bVerticalMirror	= filter->bVerticalMirror;//XML.getValue("VMIRROR",0);
	filter_fiducial->bHorizontalMirror	= filter->bHorizontalMirror;//XML.getValue("HMIRROR",0);


	//XML.pushTag("CONFIG:FIDUCIAL"  , 0);
	filter_fiducial->bTrackDark			= XML.getValue("CONFIG:FIDUCIAL:BOOLEAN:TRACKDARK", 0);
	filter_fiducial->bHighpass			= XML.getValue("CONFIG:FIDUCIAL:BOOLEAN:HIGHPASS",0);
	filter_fiducial->bAmplify			= XML.getValue("CONFIG:FIDUCIAL:BOOLEAN:AMPLIFY", 1);
	filter_fiducial->bSmooth			= XML.getValue("CONFIG:FIDUCIAL:BOOLEAN:SMOOTH", 1);
	filter_fiducial->bDynamicBG			= XML.getValue("CONFIG:BOOLEAN:DYNAMICBG", 1);//!!! synchron with normal filter
	//Filter Settings
	filter_fiducial->threshold			= XML.getValue("CONFIG:FIDUCIAL:INT:THRESHOLD",0);
	filter_fiducial->highpassBlur		= XML.getValue("CONFIG:FIDUCIAL:INT:HIGHPASSBLUR",0);
	filter_fiducial->highpassNoise		= XML.getValue("CONFIG:FIDUCIAL:INT:HIGHPASSNOISE",0);
	filter_fiducial->highpassAmp		= XML.getValue("CONFIG:FIDUCIAL:INT:HIGHPASSAMP",0);
	filter_fiducial->smooth				= XML.getValue("CONFIG:FIDUCIAL:INT:SMOOTH",0);

	//XML.popTag();

	char* temp = new char[255];
	strcpy  (temp,const_cast<char*>(XML.getValue("CONFIG:NETWORK:TUIO_SOURCE_APPLICATION","DEFAULT").c_str() ) );
	
	myTUIO.tuio_source_application =  temp;
	myTUIO.setup_tuio_source_String();//

	calibration_udp_port = XML.getValue("CONFIG:NETWORK:CALIBRATION_UDP_PORT", 0);
	calibration_udp_host = XML.getValue("CONFIG:NETWORK:CALIBRATION_HOST", "localhost");

	// position of the propertiesPanel 
	string  xmlPath;
	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:SOURCE_PROPERTIES:X": "CONFIG:PROPERTIESPANEL:SOURCE_PROPERTIES:X" ;
	source_properties_panel_pos.x = XML.getValue(xmlPath,0);
	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:SOURCE_PROPERTIES:Y": "CONFIG:PROPERTIESPANEL:SOURCE_PROPERTIES:Y" ;
	source_properties_panel_pos.y = XML.getValue(xmlPath,0);

	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:GPU_PROPERTIES:X": "CONFIG:PROPERTIESPANEL:GPU_PROPERTIES:X" ;
	gpu_properties_panel_pos.x = XML.getValue(xmlPath,0);
	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:GPU_PROPERTIES:Y": "CONFIG:PROPERTIESPANEL:GPU_PROPERTIES:Y" ;
	gpu_properties_panel_pos.y = XML.getValue(xmlPath,0);

	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:COMMUNICATION:X": "CONFIG:PROPERTIESPANEL:COMMUNICATION:X" ;
	communication_properties_panel_pos.x = XML.getValue(xmlPath,0);
	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:COMMUNICATION:Y": "CONFIG:PROPERTIESPANEL:COMMUNICATION:Y" ;
	communication_properties_panel_pos.y = XML.getValue(xmlPath,0);

	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:CALIBRATION:X": "CONFIG:PROPERTIESPANEL:CALIBRATION:X" ;
	calibration_properties_panel_pos.x = XML.getValue(xmlPath,0);
	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:CALIBRATION:Y": "CONFIG:PROPERTIESPANEL:CALIBRATION:Y" ;
	calibration_properties_panel_pos.y = XML.getValue(xmlPath,0);

	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:FILES:X": "CONFIG:PROPERTIESPANEL:FILES:X" ;
	files_properties_panel_pos.x = XML.getValue(xmlPath,0);
	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:PROPERTIESPANEL:FILES:Y": "CONFIG:PROPERTIESPANEL:FILES:Y" ;
	files_properties_panel_pos.y = XML.getValue(xmlPath,0);

	
	//----added by Stefan Schlupek
	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:WINDOW:WIDTH": "CONFIG:WINDOW:WIDTH" ;
	winWidth					= XML.getValue(xmlPath, 950);
	xmlPath  = bTrackFiducials ? "CONFIG:FIDUCIAL:WINDOW:HEIGHT": "CONFIG:WINDOW:HEIGHT" ;
	winHeight					= XML.getValue(xmlPath, 600);
	
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
	//XML.setValue("CONFIG:BOOLEAN:LEARNBG", filter->bLearnBackground );
	XML.setValue("CONFIG:BOOLEAN:VMIRROR", filter->bVerticalMirror);
	XML.setValue("CONFIG:BOOLEAN:HMIRROR", filter->bHorizontalMirror);
	XML.setValue("CONFIG:BOOLEAN:PRINTFTOFILE", printfToFile);
	XML.setValue("CONFIG:BOOLEAN:TRACKDARK", filter->bTrackDark);
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
//	XML.setValue("CONFIG:NETWORK:LOCALHOST", myTUIO.localHost);
//	XML.setValue("CONFIG:NETWORK:TUIO_PORT_OUT",myTUIO.TUIOPort);
	//fiducial settings
	XML.setValue("CONFIG:FIDUCIAL:INT:THRESHOLD", filter_fiducial->threshold);
	XML.setValue("CONFIG:FIDUCIAL:INT:SMOOTH", filter_fiducial->smooth	);
	XML.setValue("CONFIG:FIDUCIAL:INT:HIGHPASSAMP", filter_fiducial->highpassAmp);
	XML.setValue("CONFIG:FIDUCIAL:BOOLEAN:SMOOTH", filter_fiducial->bSmooth);
	XML.setValue("CONFIG:FIDUCIAL:BOOLEAN:AMPLIFY", filter_fiducial->bAmplify);
	
	//calibration_udp_port = XML.getValue("CONFIG:NETWORK:CALIBRATION_UDP_PORT", 0);
	//calibration_udp_host = XML.getValue("CONFIG:NETWORK:CALIBRATION_HOST", "localhost");

	
	//--
	XML.saveFile("config.xml");
}



/************************************************
*				Init Device
************************************************/
//Init Device (camera/video)
void ofxNCoreVision::initDevice(){

	//save/update log file
	if(printfToFile) if((stream = freopen(fileName, "a", stdout)) == NULL){}

	//Pick the Source - camera or video
	if (bcamera)
	{
		//check if a firefly, ps3 camera, or other is plugged in
		#ifdef TARGET_WIN32
			/****PS3 - PS3 camera only****/
		    if(ofxPS3::getDeviceCount() > 0 && PS3 == NULL && deviceModel=="PS3"){
				PS3 = new ofxPS3();
				PS3->listDevices();
				PS3->initPS3(camWidth, camHeight, camRate);
				camWidth = PS3->getCamWidth();
			    camHeight = PS3->getCamHeight();
				printf("PS3 Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, PS3->getCamWidth(), PS3->getCamHeight());
				return;
			}
			/****ffmv - firefly camera only****/
			else if(ofxffmv::getDeviceCount() > 0 && ffmv == NULL && deviceModel=="FFMV"){
			   ffmv = new ofxffmv();
			   ffmv->setDeviceID(deviceID);//Here we could change the Camera
			   ffmv->listDevices();
			   //here we set the Memeory Chanel to restore camera settings from the driver
			   ffmv->setMemoryChannel(ffmvMemoryChannel);
			   ffmv->initFFMV(camWidth,camHeight,camRate);
			   printf("FFMV Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, ffmv->getCamWidth(), ffmv->getCamHeight());
			   camWidth = ffmv->getCamWidth();
			   camHeight = ffmv->getCamHeight();
			   return;
			}
			else if( vidGrabber == NULL ) {
				vidGrabber = new ofVideoGrabber();
				vidGrabber->listDevices();
				vidGrabber->setDeviceID(deviceID);
				vidGrabber->setVerbose(true);
				vidGrabber->initGrabber(camWidth,camHeight);
				printf("vidGrabber Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, vidGrabber->width, vidGrabber->height);
				camWidth = vidGrabber->width;
				camHeight = vidGrabber->height;
				return;
			}
			 else if( dsvl == NULL) {
				dsvl = new ofxDSVL();
				dsvl->initDSVL();
				printf("DSVL Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, dsvl->getCamWidth(), dsvl->getCamHeight());
				camWidth = dsvl->getCamWidth();
				camHeight = dsvl->getCamHeight();
				return;
			}
		#else 
			if( vidGrabber == NULL ) {
			vidGrabber = new ofVideoGrabber();
			vidGrabber->listDevices();
			vidGrabber->setDeviceID(deviceID);
			vidGrabber->setVerbose(true);
			vidGrabber->initGrabber(camWidth,camHeight);
			printf("Camera Mode\nAsked for %i by %i - actual size is %i by %i \n\n", camWidth, camHeight, vidGrabber->width, vidGrabber->height);
			camWidth = vidGrabber->width;
			camHeight = vidGrabber->height;
			return;
        }
		#endif
	}else{
		if( vidPlayer == NULL ) {
            vidPlayer = new ofVideoPlayer();
            vidPlayer->loadMovie( videoFileName );
            vidPlayer->play();
            vidPlayer->setLoopState(OF_LOOP_NORMAL);
			printf("Video Mode\n\n");
			camHeight = vidPlayer->height;
			camWidth = vidPlayer->width;
			return;
        }
	}
}

/******************************************************************************
* The update function runs continuously. Use it to update states and variables
*****************************************************************************/
void ofxNCoreVision::_update(ofEventArgs &e)
{
	//save/update log file
	if(printfToFile) if((stream = freopen(fileName, "a", stdout)) == NULL){}

	if(udp_alive_connection.isOUTConnected){
	udp_alive_connection.readMessage();
	}


	if(exited) return;

	bNewFrame = false;

	if (bcamera) //if camera
	{
		#ifdef TARGET_WIN32
			if(PS3!=NULL)//ps3 camera
			{
				bNewFrame = PS3->isFrameNew();
			}
			else if(ffmv!=NULL)
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
		
			//---added by Stefan Schlupek
			
			if(!bCalibration){
				deleteOutsideBlobs();
				
			}

			if(bTrackFiducials){
				grabFrameToGPU(filter_fiducial->gpuSourceTex);
				filter_fiducial->applyGPUFilters();
				fidfinder.findFiducials( filter_fiducial->gpuReadBackImageGS );

				prepareFiducialPositions();
			}
			//----------

		}
		else
		{
			grabFrameToCPU();
			filter->applyCPUFilters( processedImg );
			contourFinder.findContours(processedImg,  (MIN_BLOB_SIZE * 2) + 1, ((camWidth * camHeight) * .4) * (MAX_BLOB_SIZE * .001), maxBlobs, false);
		
			//---added by Stefan Schlupek
			//Map points to undistortion map?

			if(!bCalibration){
				deleteOutsideBlobs();
			}

			

			if(bTrackFiducials){
				filter_fiducial->applyCPUFilters( processedImg_fiducial );
				
				fidfinder.findFiducials( processedImg_fiducial );
				// printf("fiducialsList.preerase!!!!%d\n\n", fidfinder.fiducialsList.size());
				prepareFiducialPositions();
			}
			//----------

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
			//myTUIO.sendTUIO(&getBlobs());
			myTUIO.sendTUIO(&getBlobs(), &fidfinder.fiducialsList);
			
		}
	}
}


/*******************************************
* added by Stefan Schlupek
* Delete blobs that outside of the calibration area.
* should be in the ContourFinder.findContours() Method but i came into big trouble
* with the Compiler when i'm  trying to implement it ( Calibration instance as parameter for the findcontours() Method doesn't work
* don't know what problems the compiler has , i think it's an include problem
********************************************/
void ofxNCoreVision::deleteOutsideBlobs(){
	//to delete the blobs outside the tracking area
	vector<Blob>::iterator it = contourFinder.blobs.begin();
	int t ;
	while( it != contourFinder.blobs.end() )
				 {
		
		Blob b = *it;						
		vector2df pt= vector2df(b.centroid.x, b.centroid.y);
	//	printf("Blob.pt:%f:%f\n", pt.X,pt.Y);
		 t = calib.calibrate.findTriangleWithin(pt);
		
		if(t == -1)
			{
			 contourFinder.blobs.erase(it);
			}else{
				++it;
			}
	}//while
	contourFinder.nBlobs = contourFinder.blobs.size();
}


/**
Delete  Fiducials who are  outside the calibration area and map the position from the FiducalTracker
**/
void ofxNCoreVision::prepareFiducialPositions(){
//mapFiducialsToScreenPosition();// calib.calibrate.cameraToScreenPosition(x,y);
				//fidfinder.fiducialsList
	int t ;

	//printf("fiducialsList.prerase:%d\n", fidfinder.fiducialsList.size());
	for (std::list<ofxFiducial>::iterator fiducial = fidfinder.fiducialsList.begin(); fiducial != fidfinder.fiducialsList.end(); ) {
		//getCameraToScreenX()
		float x = fiducial->getX();
		float y = fiducial->getY();
		 vector2df pt = vector2df(x, y);
		 //printf("-fiducial:%p:\n", &fiducial);
		 // printf("-fiducial.back:%p:\n", & fidfinder.fiducialsList.back());
		
		 	//printf("fiducial.pt:%f:%f\n", pt.X,pt.Y);
		  t = calib.calibrate.findTriangleWithin(pt);

		  if(t == -1)
			{
			//printf("fiducialsList.erase!!!!\n");
			fiducial = fidfinder.fiducialsList.erase(fiducial);
			//++it;
			// printf("fiducialsList.erase2!!!!%d\n", fidfinder.fiducialsList.size());
			}else{

				//weired bug: there could be the case that we findTriangleWithin, but the vector values  in  the cameraToScreenMap are 0.00000000
				// as at creation time  of the map, the point seemed not to be in a triangle
				//is this a rounding error? so we have to check this (perhaps in vector2df::isOnSameSide() )
				
				calib.calibrate.cameraToScreenPosition(x,y);
				if(x== NULL && y == NULL){
					fiducial = fidfinder.fiducialsList.erase(fiducial);
				}else{
					fiducial->setCameraToScreenPosition(x,y);
					++fiducial;
				}
			}


		//calib.calibrate.cameraToScreenPosition(x,y);


	
	}


}




/************************************************
*				Input Device Stuff
************************************************/
//get pixels from camera
void ofxNCoreVision::getPixels(){
//printf("getPixels\n");
#ifdef TARGET_WIN32
	if(PS3!=NULL){
		sourceImg.setFromPixels(PS3->getPixels(), camWidth, camHeight);
		//convert to grayscale
		processedImg = sourceImg;
		//if(bUndistort) {undistortImage( processedImg);}
		if(bTrackFiducials){processedImg_fiducial = sourceImg;}
		if(bUndistort){undistortedImg = sourceImg;}
		 
	}
	else if(ffmv != NULL){
		processedImg.setFromPixels(ffmv->fcImage[0].pData, camWidth, camHeight);
		//if(bUndistort) {undistortImage( processedImg);}
		if(bTrackFiducials){processedImg_fiducial.setFromPixels(ffmv->fcImage[0].pData, camWidth, camHeight);}
		if(bUndistort){undistortedImg = processedImg;}
	}
	else if(vidGrabber != NULL ) {
		sourceImg.setFromPixels(vidGrabber->getPixels(), camWidth, camHeight);
		//convert to grayscale
		processedImg = sourceImg;
		//if(bUndistort) {undistortImage( processedImg);}
		if(bTrackFiducials){processedImg_fiducial = sourceImg;}
		if(bUndistort){undistortedImg = sourceImg;}
	}
	else if(dsvl!=NULL)
	{
		if(dsvl->getNumByes() != 1){ //if not grayscale
			sourceImg.setFromPixels(dsvl->getPixels(), camWidth, camHeight);
			//convert to grayscale
			processedImg = sourceImg;
			//if(bUndistort) {undistortImage( processedImg);}
			if(bTrackFiducials){processedImg_fiducial = sourceImg;}
			if(bUndistort){undistortedImg = sourceImg;}
		}
		else{//if grayscale
			processedImg.setFromPixels(dsvl->getPixels(), camWidth, camHeight);
			//if(bUndistort) {undistortImage( processedImg);}
			if(bTrackFiducials){processedImg_fiducial.setFromPixels(dsvl->getPixels(), camWidth, camHeight);}
			if(bUndistort){undistortedImg = processedImg;}
		}
	}
#endif	
}


//Grab frame from CPU
void ofxNCoreVision::grabFrameToCPU()
{
	//printf("grabFrameToCPU\n");
	//Set sourceImg as new camera/video frame
	if (bcamera)
	{
	    #ifdef TARGET_WIN32
			getPixels();
 		#else
            sourceImg.setFromPixels(vidGrabber->getPixels(), camWidth, camHeight);
 			//convert to grayscale
 			processedImg = sourceImg;
			if(bTrackFiducials){processedImg_fiducial = sourceImg;}
 		#endif
	}
	else
	{
		sourceImg.setFromPixels(vidPlayer->getPixels(), camWidth, camHeight);
		//convert to grayscale
		processedImg = sourceImg;
		//if(bUndistort) {undistortImage( processedImg);}
		if(bTrackFiducials){processedImg_fiducial = sourceImg;}
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
			if(PS3!=NULL)
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, PS3->getPixels());
			}
			else if(ffmv != NULL){
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, camWidth, camHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, ffmv->fcImage[0].pData);
			}
			else if(vidGrabber!=NULL)
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

			//---Added by Stefan Schlupek
			//here we could triger the UDP message for highlighting the selected Finger. calib.calibrate.calibrationStep
			if(currCalibrationStep != calib.calibrate.calibrationStep){
				currCalibrationStep = calib.calibrate.calibrationStep;
				char*  message = new char[256];
				sprintf(message,"calibrate|POINT|%s|%d",myTUIO.tuio_source_String,calib.calibrate.calibrationStep);
				int sent = calib.calibrate.udpConnection.Send(message,strlen(message)+1);
				delete message;		
			}
			//-----------------

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
			//----Added by Stefan Schlupek

			if(bTrackFiducials){drawFiducials();}

			//------------------------------
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
	if (bGPUMode) {
		filter->drawGPU();
		if(bTrackFiducials){filter_fiducial->drawGPU_fiducial();}
	}
	else {
		filter->draw();
		//filter_fiducial->draw(720,30,320,240);
		if(bTrackFiducials){filter_fiducial->draw_fiducial();}
		//cout << "<<<<<<<<<<<<" <<calib.getCalibrationUtils()->camCalib.distortionCoeffs[0] << endl;

		if(bUndistort){
			if(filter->bVerticalMirror || filter->bHorizontalMirror) undistortedImg.mirror(filter->bVerticalMirror, filter->bHorizontalMirror);
			
			undistortImage( undistortedImg);
			/*
				undistortedImg.undistort( calib.getCalibrationUtils()->camCalib.distortionCoeffs[0], calib.getCalibrationUtils()->camCalib.distortionCoeffs[1],
										  calib.getCalibrationUtils()->camCalib.distortionCoeffs[2], calib.getCalibrationUtils()->camCalib.distortionCoeffs[3],
										  calib.getCalibrationUtils()->camCalib.camIntrinsics[0], calib.getCalibrationUtils()->camCalib.camIntrinsics[4],
										  calib.getCalibrationUtils()->camCalib.camIntrinsics[2], calib.getCalibrationUtils()->camCalib.camIntrinsics[5] ); 
										  */
									  
			undistortedImg.draw(winWidth-180,winHeight-140,160,120);//winWidth,winHeight
		}//Undistort


	}

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

	if(bTrackFiducials){bigvideo.drawString("Fiducial_Tracked Image", 830, 20);}

/*
	//draw link to tbeta website
	ofSetColor(79, 79, 79);
	ofFill();
	ofRect(721, 586, 228, 14);
	ofSetColor(0xFFFFFF);
	ofDrawBitmapString("|  ~  |tbeta.nuigroup.com", 725, 596);
*/
	//Display Application information in bottom right
	string str = "Calc. Time [ms]:  ";
	str+= ofToString(differenceTime, 0)+"\n\n";

	if (bcamera)
	{
		string str2 = "Camera [Res]:     ";
        str2+= ofToString(camWidth, 0) + " x " + ofToString(camHeight, 0)  + "\n";
		string str4 = "Camera [fps]:     ";
		str4+= ofToString(fps, 0)+"\n";
		ofSetColor(0xFFFFFF);
		verdana.drawString(str + str2 + str4, 740, 410);
	}
	else
	{
		string str2 = "Video [Res]:       ";
		str2+= ofToString(vidPlayer->width, 0) + " x " + ofToString(vidPlayer->height, 0)  + "\n";
		string str4 = "Video [fps]:        ";
		str4+= ofToString(fps, 0)+"\n";
		ofSetColor(0xFFFFFF);
		verdana.drawString(str + str2 + str4, 740, 410);
	}

	if (bTUIOMode)
	{
		//Draw Port and IP to screen
		ofSetColor(0xffffff);
		char buf[256];
		if(myTUIO.bOSCMode)
			sprintf(buf, "Sending OSC messages to:\nHost: %s\nPort: %i", myTUIO.localHost, myTUIO.TUIOPort);
		else{
			if(myTUIO.bIsConnected)
			sprintf(buf, "Sending TCP messages to:\nPort: %i", myTUIO.TUIOFlashPort);
			else
			sprintf(buf, "Could not bind or send TCP to:\nPort: %i", myTUIO.TUIOFlashPort);
		}		
		
		verdana.drawString(buf, 740, 480);
	}

	ofSetColor(0xFF0000);
	verdana.drawString("Press spacebar to toggle fast mode", 730, 472);
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
	verdana.drawString("Calc. Time  [ms]:        " + ofToString(differenceTime,0),10, ofGetHeight() - 70 );
	if (bcamera){
		verdana.drawString("Camera [fps]:            " + ofToString(fps,0),10, ofGetHeight() - 50 );
	}
	else {
		verdana.drawString("Video [fps]:              " + ofToString(fps,0),10, ofGetHeight() - 50 );
	}
	verdana.drawString("Blob Count:               " + ofToString(contourFinder.nBlobs,0),10, ofGetHeight() - 29 );
	verdana.drawString("Sending TUIO:  " ,10, ofGetHeight() - 9 );

	//draw green tuio circle
	if((myTUIO.bIsConnected || myTUIO.bOSCMode) && bTUIOMode)//green = connected
	ofSetColor(0x00FF00);
	else
	ofSetColor(0xFF0000); //red = not connected
	ofFill();
	ofCircle(ofGetWidth() - 17 , ofGetHeight() - 10, 5);
	ofNoFill();
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


/*-------------------------------------------------------------------------
added by Stefan Schlupek
*/

void ofxNCoreVision::drawFiducials(){

	//you can use this method for the FidicialFinder
	/*for(int f=0; f<fidfinder._fiducials.size() ;f++) {
		fidfinder._fiducials[f].draw( 20, 20 );
	}*/
	
	//use this method for the FiducialTracker
	//to get fiducial info you can use the fiducial getter methods
	char idStr[1024];
	//sprintf(idStr, "scaleWidth: %f", fiducialDrawFactor_Width);
	//verdana.drawString(idStr, 730, 562);
	for (list<ofxFiducial>::iterator fiducial = fidfinder.fiducialsList.begin(); fiducial != fidfinder.fiducialsList.end(); fiducial++) {
		
		//Nasty hack: hardcoded Position of the offset to draw into the right position
		fiducial->drawScaled(40,30,fiducialDrawFactor_Width,fiducialDrawFactor_Height);
		//fiducial->drawCorners( 40, 30 );//draw corners
		fiducial->drawCornersScaled( 40, 30 ,fiducialDrawFactor_Width,fiducialDrawFactor_Height);//draw corners
		/*
		ofSetColor(0,0,255);//set color to blue
		//if mouse (- 20 to compensate for drawing at 20) is inside fiducial then fill
		if (fiducial->isPointInside(mouseX - 20, mouseY - 20)) ofFill();
			else ofNoFill();// else dont
		ofCircle(mouseX, mouseY, 10);//draw mouse position
		*/
		ofSetColor(255,255,255);//reset color to white
		//like this one below
		//cout << "fiducial " << fiducial->getId() << " found at ( " << fiducial->getX() << "," << fiducial->getY() << " )" << endl;
		//take a good look at the fiducial class to get all the info and a few helper functions
		
	 }
	
	
	//draw the fingers
	for (list<ofxFinger>::iterator finger = fidfinder.fingersList.begin(); finger != fidfinder.fingersList.end(); finger++) {
		finger->draw(20, 20);
	}
	

}

//----------------------------------------------

void ofxNCoreVision::saveBackground(){
	ofImage bgImg;
	bgImg.allocate(camWidth,camHeight,OF_IMAGE_GRAYSCALE);
	bgImg.setFromPixels(filter->floatBgImg.getPixels(),camWidth, camHeight,OF_IMAGE_GRAYSCALE);
	
	bgImg.saveImage(backgroundImagePath);
}

//Method for POCO Event system. didn't get a version realized with no arguments  02.09.10 by stefan Schlupek
void ofxNCoreVision::makeAndSaveBackground(char &args){
	grabFrameToCPU();
	if(filter->bVerticalMirror || filter->bHorizontalMirror) processedImg.mirror(filter->bVerticalMirror, filter->bHorizontalMirror);
	filter->setCPUBackgroundImage(processedImg);
	filter_fiducial->setCPUBackgroundImage(processedImg);

saveBackground();
}

void ofxNCoreVision::loadBackground(){
	ofImage bgImg;
	ofxCvGrayscaleImage bgImgBW;

	if (bGPUMode)
	{
	//TODO !!!!
	}else{
		
		
		bgImg.loadImage(backgroundImagePath);
		bgImg.setImageType(OF_IMAGE_GRAYSCALE); 
		bgImgBW.allocate(camWidth, camHeight);
		bgImgBW.setFromPixels( bgImg.getPixels(),camWidth, camHeight);
		filter->setCPUBackgroundImage(bgImgBW);
		filter_fiducial->setCPUBackgroundImage(bgImgBW);
	}

}
//ofxCvGrayscaleImage
void ofxNCoreVision::undistortImage( ofxCvImage &img){

	
			

				img.undistort( calib.getCalibrationUtils()->camCalib.distortionCoeffs[0], calib.getCalibrationUtils()->camCalib.distortionCoeffs[1],
										  calib.getCalibrationUtils()->camCalib.distortionCoeffs[2], calib.getCalibrationUtils()->camCalib.distortionCoeffs[3],
										  calib.getCalibrationUtils()->camCalib.camIntrinsics[0], calib.getCalibrationUtils()->camCalib.camIntrinsics[4],
										  calib.getCalibrationUtils()->camCalib.camIntrinsics[2], calib.getCalibrationUtils()->camCalib.camIntrinsics[5] ); 
									  
		


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
		case 'a':
			filter->threshold++;
			controls->update(appPtr->trackedPanel_threshold, kofxGui_Set_Int, &appPtr->filter->threshold, sizeof(int));
			break;
		case 'z':
			filter->threshold--;
			controls->update(appPtr->trackedPanel_threshold, kofxGui_Set_Int, &appPtr->filter->threshold, sizeof(int));
			break;
		case 'b':
			filter->bLearnBackground = true;
			filter_fiducial->bLearnBackground = true;
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
			filter_fiducial->bVerticalMirror ? filter_fiducial->bVerticalMirror = false : filter_fiducial->bVerticalMirror = true;
			controls->update(appPtr->propertiesPanel_flipV, kofxGui_Set_Bool, &appPtr->filter->bVerticalMirror, sizeof(bool));


			break;
		case 't':
			myTUIO.bOSCMode = !myTUIO.bOSCMode;
			myTUIO.bTCPMode = false;
			bTUIOMode = myTUIO.bOSCMode;
			controls->update(appPtr->optionPanel_tuio_tcp, kofxGui_Set_Bool, &appPtr->myTUIO.bTCPMode, sizeof(bool));
			controls->update(appPtr->optionPanel_tuio_osc, kofxGui_Set_Bool, &appPtr->myTUIO.bOSCMode, sizeof(bool));
			//clear blobs
//			myTUIO.blobs.clear();
			break;
		case 'f':
			myTUIO.bOSCMode = false;
			myTUIO.bTCPMode = !myTUIO.bTCPMode;
			bTUIOMode = myTUIO.bTCPMode;
			controls->update(appPtr->optionPanel_tuio_tcp, kofxGui_Set_Bool, &appPtr->myTUIO.bTCPMode, sizeof(bool));
			controls->update(appPtr->optionPanel_tuio_osc, kofxGui_Set_Bool, &appPtr->myTUIO.bOSCMode, sizeof(bool));
			//clear blobs
//			myTUIO.blobs.clear();
			break;
		case 'g':
			bGPUMode ? bGPUMode = false : bGPUMode = true;
			controls->update(appPtr->gpuPanel_use, kofxGui_Set_Bool, &appPtr->bGPUMode, sizeof(bool));
			filter->bLearnBackground = true;
			filter_fiducial->bLearnBackground = true;
			break;
		case 'v':
			if (bcamera && vidGrabber != NULL)
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
				filter_fiducial->bMiniMode = bMiniMode;
				//ofSetWindowShape(950,600); //default size
				ofSetWindowShape(winWidth,winHeight);
			}
			else if(!bCalibration)
			{
				bMiniMode = true;
				bShowInterface = false;
				filter->bMiniMode = bMiniMode;
				filter_fiducial->bMiniMode = bMiniMode;
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

				//added by stefan Schlupek--------
				char*  message = new char[512];
				sprintf(message,"%s|%s","calibrate|OFF",myTUIO.tuio_source_String);
				int sent = calib.calibrate.udpConnection.Send(message,strlen(message)+1);
				currCalibrationStep = -1;
				//int sent = calib.calibrate.udpConnection.Send(message.c_str(),message.length());
				delete message;
				//--------------

			}
			break;
		case 's':
			saveBackground();
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
			//added by stefan Schlupek--------
			char*  message = new char[512];
			//calibrate|ON|TUIOSource Name@adress|X Points|Y POints)
			sprintf(message,"calibrate|ON|%s|%d|%d",
				myTUIO.tuio_source_String,
				calib.calibrate.GRID_X,
				calib.calibrate.GRID_Y);
			int sent = calib.calibrate.udpConnection.Send(message,strlen(message)+1);
			//int sent = calib.calibrate.udpConnection.Send(message.c_str(),message.length());
			cout <<"UDP.Message:" << message << endl;
			printf("UDP.Message:%s!\n",message);
			delete message;
			//--------------
		}
		
	}
	if ( e.key == '~' || e.key == '`' && !bMiniMode && !bCalibration) showConfiguration = !showConfiguration;
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
		if (e.x > 722 && e.y > 586){ofLaunchBrowser("http://tbeta.nuigroup.com");}
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

std::map<int, Blob> ofxNCoreVision::getBlobs(){

	return tracker.getTrackedBlobs();
}

/*****************************************************************************
* ON EXIT
*****************************************************************************/
void ofxNCoreVision::_exit(ofEventArgs &e)
{
	//save/update log file
	if((stream = freopen(fileName, "a", stdout)) == NULL){}

	saveSettings();

    #ifdef TARGET_WIN32
		if(PS3!=NULL) delete PS3;
		if(ffmv!=NULL) delete ffmv;
		if(dsvl!=NULL) delete dsvl;	
	#endif

	if(vidGrabber!=NULL) delete vidGrabber;
	if(vidPlayer !=NULL) delete vidPlayer;
	// -------------------------------- SAVE STATE ON EXIT
	
	printf("Vision module has exited!\n");
}

