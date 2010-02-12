#include "ofxffmv.h"
ofxffmv::ofxffmv()
{
    fcCameraID=0;

    //First assume there are MAX_CAMS.Then this variable is modified to the
    //true camera number after listDevices() is called.
    camNum= _MAX_CAMS;
}
void ofxffmv::listDevices()
{
    printf("The following are FFMV\n");
    // This function enumerates all the cameras found on the machine, across
    // all 1394 buses and cards. It fills an array of FlyCaptureInfoEx
    // structures with all of the pertinent information from the attached
    // cameras. The index of a given FlyCaptureInfoEx structure in the array
    // parInfo is the device number.
   flycaptureBusEnumerateCamerasEx( arInfo, &camNum );
	for( unsigned int uiBusIndex = 0; uiBusIndex < camNum; uiBusIndex++ )
   {
      FlyCaptureInfoEx* pinfo = &arInfo[ uiBusIndex ];
      printf(
		  "Index %u: %s, SerialNumber:  %u\n",
         uiBusIndex,
         pinfo->pszModelName,
         pinfo->SerialNumber );
   }
   printf("end of listing FFMV\n\n");
}
void ofxffmv::setDeviceID(int id)
{
    fcCameraID=id;
}
int ofxffmv::getDeviceID()
{
    return fcCameraID;
}

void ofxffmv::setMemoryChannel(int nr)
{
	fcMemoryChannel=nr;
}

void ofxffmv::initFFMV(int wid,int hei, int rate,int startX,int startY )
{
	//camNum= 1;
	/*
    for(int i=0;i<camNum;i++)
    {
	*/
		std::cout <<"initFFMV.Camera:" << fcCameraID << std::endl ;
    //
	// Create the context. This call must be made before any other calls to
	// to the context are made, as a valid context is needed for all
	// camera-specific FlyCapture library calls. This call sets the context
	// to the default settings, but flycaptureInitialize() must be called
	// below to fully initialize the camera for use.
	flycaptureCreateContext( &context[0] );
	printf("initFFMV.A\n");
	//
	// Initialize the camera. This call initializes one of the cameras on the
	// bus with the FlyCaptureContext that is passed in. This should generally
	// be called right after creating the context but before doing anything
	// else.
	//
	// This call performs several functions. It sets the camera to communicate
	// at the proper bus speed, turns on color processing (if available) and
	// sets the Bayer orientation to the correct setting. Finally, it also
	// initializes the white balance tables for cameras that support that
	// function.
	//
	//flycaptureInitialize( context[i], i );
	flycaptureInitialize( context[0], fcCameraID );
	printf("VinitFFMV.B\n");



	//now we load the camera settings from the Memory Channel. With this we could manage the settings in the flycap.exe and save the data into the channel for later use

	unsigned int puiCurrentChannel;
	unsigned int puiNumChannels;

	//flycaptureGetMemoryChannel(FlyCaptureContext  context,unsigned int*     puiCurrentChannel,unsigned int*     puiNumChannels )
	flycaptureGetMemoryChannel(context[0], &puiCurrentChannel, &puiNumChannels );
	std::cout <<"puiCurrentChannel:"<<puiCurrentChannel<<"| puiNumChannels:"<<puiNumChannels << std::endl;

		//flycaptureRestoreFromMemoryChannel(FlyCaptureContext  context,unsigned long     ulChannel );
	if(fcMemoryChannel != NULL && fcMemoryChannel <= puiNumChannels){
		std::cout <<"Using XML Data for Memory Channel:"<<fcMemoryChannel << std::endl;
		flycaptureRestoreFromMemoryChannel(context[0],fcMemoryChannel);
	}else if (puiCurrentChannel != NULL) {
		std::cout <<"Using puiCurrentChannel Data for Memory Channel:"<<puiCurrentChannel << std::endl;
		flycaptureRestoreFromMemoryChannel(context[0],puiCurrentChannel);
	}
//-----------------------------------

	//
	// Start grabbing images in the current videomode and framerate. Driver
	// level image buffer allocation is performed at this point. After this
	// point, images will be constantly grabbed by the camera and stored
	// in the buffers on the PC.
	/*
	if(wid==640)
	flycaptureStartCustomImage(context[i],0,startX,startY,640,480,100,FLYCAPTURE_MONO8);
	else if(wid==320)
	flycaptureStartCustomImage(context[i],1,startX,startY,320,240,100,FLYCAPTURE_MONO8);
	flycaptureGrabImage2( context[i], &fcImage[i] );
	*/

	//http://nuigroup.com/forums/viewthread/6643/#42007

	FlyCaptureVideoMode videoMode;
    switch(wid)
    {
        case 640:
            videoMode = FLYCAPTURE_VIDEOMODE_640x480Y8;
            break;
        case 800:
            videoMode = FLYCAPTURE_VIDEOMODE_800x600Y8;
            break;
        case 1024:
            videoMode = FLYCAPTURE_VIDEOMODE_1024x768Y8;
            break;
        case 1280:
            videoMode = FLYCAPTURE_VIDEOMODE_1280x960Y8;
            break;
        case 1600:
            videoMode = FLYCAPTURE_VIDEOMODE_1600x1200Y8;
            break;
        default:
            videoMode = FLYCAPTURE_VIDEOMODE_640x480Y8;
    }

    FlyCaptureFrameRate frameRate;
    switch(rate)
    {
        case 15:
            frameRate = FLYCAPTURE_FRAMERATE_15;
            break;
        case 30:
            frameRate = FLYCAPTURE_FRAMERATE_30;
            break;
        case 60:
            frameRate = FLYCAPTURE_FRAMERATE_60;
            break;
        case 120:
            frameRate = FLYCAPTURE_FRAMERATE_120;
            break;
        case 240:
            frameRate = FLYCAPTURE_FRAMERATE_240;
            break;
        default:
            frameRate = FLYCAPTURE_FRAMERATE_30;
    }

    bool result;
    flycaptureCheckVideoMode(context[0], videoMode, frameRate, &result);
	printf("VinitFFMV.C\n");
    if(result)
    {
        flycaptureStart(context[0], videoMode, frameRate);
		printf("VinitFFMV.D\n");
       // flycaptureGrabImage2(context[i], &fcImage[i] );
		printf("VinitFFMV.E\n");
    }
    else printf("Video mode not supported\n");

	flycaptureGrabImage2( context[0], &fcImage[0] );//Für getWidth()/Hight()
	//camWidth=fcImage.iCols;
	//camHeight=fcImage.iRows;
   // }//end of for loop
	printf("VinitFFMV.F\n");
}
void ofxffmv::grabFrame()
{
    //
    // Grab an image. This obtains an pointer to the latest full
    // image captured by the camera and saved in the image buffer.
    //
    // flycaptureGrabImage2 is used instead of flycaptureGrabImage because
    // it returns a FlyCaptureImage structure, which is generally easier to
    // work with.
    //
	//std::cout <<"fcCameraID:" << fcCameraID << std::endl ;
   // flycaptureGrabImage2( context[fcCameraID], &fcImage[fcCameraID] );
	flycaptureGrabImage2( context[0], &fcImage[0] );
}
int ofxffmv::getDeviceCount()
{
	// Get number of PGRCameras
	unsigned int uiPGRCameraCount;
	flycaptureBusCameraCount(&uiPGRCameraCount);
	return uiPGRCameraCount;
}
int ofxffmv::getCamWidth()
{
    camWidth=fcImage[0].iCols;
    return camWidth;
}
int ofxffmv::getCamHeight()
{
    camHeight=fcImage[0].iRows;
    return camHeight;
}
//Clean up
ofxffmv::~ofxffmv()
{
	 camNum = 1;
	if(getDeviceCount() > 0){
		for(int i=0;i<camNum;i++)
		{
		// Stop the camera. This does not destroy the context. This simply stops
		// the grabbing of images from the camera. This should always be called
		// prior to calling flycaptureDestroyContext().
		//
		flycaptureStop( context[i] );
		//
		// Destroy the context. This should always be called before exiting
		// the application to prevent memory leaks.
		//
		flycaptureDestroyContext( context[i] );
		}//end of for loop
	}
}
