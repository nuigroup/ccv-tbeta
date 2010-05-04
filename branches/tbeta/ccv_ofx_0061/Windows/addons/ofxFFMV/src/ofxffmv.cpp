#include "ofxffmv.h"

#pragma warning(disable : 4018)	// signed/unsigned mismatch

ofxffmv::ofxffmv()
{
    fcCameraID=0;

    //First assume there are MAX_CAMS.Then this variable is modified to the
    //true camera number after listDevices() is called.
    camNum=_MAX_CAMS;
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
void ofxffmv::initFFMV(int wid,int hei,int startX,int startY)
{
    for(int i=0;i<camNum;i++)
    {
    //
	// Create the context. This call must be made before any other calls to
	// to the context are made, as a valid context is needed for all
	// camera-specific FlyCapture library calls. This call sets the context
	// to the default settings, but flycaptureInitialize() must be called
	// below to fully initialize the camera for use.
	flycaptureCreateContext( &context[i] );
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
	flycaptureInitialize( context[i], i );
	//
	// Start grabbing images in the current videomode and framerate. Driver
	// level image buffer allocation is performed at this point. After this
	// point, images will be constantly grabbed by the camera and stored
	// in the buffers on the PC.
	if(wid==640)
	flycaptureStartCustomImage(context[i],0,startX,startY,640,480,100,FLYCAPTURE_MONO8);
	else if(wid==320)
	flycaptureStartCustomImage(context[i],1,startX,startY,320,240,100,FLYCAPTURE_MONO8);
	flycaptureGrabImage2( context[i], &fcImage[i] );
	//camWidth=fcImage.iCols;
	//camHeight=fcImage.iRows;
    }//end of for loop
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
    flycaptureGrabImage2( context[fcCameraID], &fcImage[fcCameraID] );
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
    camWidth=fcImage[fcCameraID].iCols;
    return camWidth;
}
int ofxffmv::getCamHeight()
{
    camHeight=fcImage[fcCameraID].iRows;
    return camHeight;
}
//Clean up
ofxffmv::~ofxffmv()
{
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
