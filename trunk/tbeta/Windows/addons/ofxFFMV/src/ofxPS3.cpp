#include "ofxPS3.h"
#include <stdio.h>

ofxPS3::ofxPS3() 
{

}

void ofxPS3::listDevices()
{
    //PS3 Camera Testing
	pCam = IPS3EyeLib::Create();
	
	// Query supported video formats
	printf("printing formats...\n");
	for(int i=0; i<IPS3EyeLib::GetNumFormats(); i++) {
		int width, height, rate;
		char *description;
		width=IPS3EyeLib::GetFormats()[i].width;
		height=IPS3EyeLib::GetFormats()[i].height;
		rate=IPS3EyeLib::GetFormats()[i].rate;
		description=IPS3EyeLib::GetFormats()[i].formatTxt;
		printf(description);
		printf("\n");
	 }
}

void ofxPS3::setDeviceID(int id)
{

}

int ofxPS3::getDeviceID()
{
    return 0;
}

PBYTE ofxPS3::getPixels()
{
	return pBuffer;
}

bool ofxPS3::isFrameNew()
{
	return pCam->GetFrame(pBuffer, 24, false);
}

void ofxPS3::initPS3(int width,int height, int framerate)
{
     printf("selecting format...\n");

	 //select format
	 pCam->SetFormat(IPS3EyeLib::GetFormatIndex(width, height, framerate));
   
	 // Allocate image buffer (we are going to capture 24bit RGB images)
   	 // The supported color depths are 16, 24 and 32     
	 pBuffer = new BYTE[(width*height*24)/8];
   
	 // Start capturing
	 pCam->StartCapture();
}

int ofxPS3::getDeviceCount()
{
    return camNum;
}

int ofxPS3::getCamWidth()
{
	return pCam->GetWidth();
}

int ofxPS3::getCamHeight()
{
	return pCam->GetHeight();
}

//Clean up
ofxPS3::~ofxPS3()
{
	// Free the image buffer
 	delete [] pBuffer;
	// Stop capturing
    pCam->StopCapture();
    // Free the
    delete pCam;
}
