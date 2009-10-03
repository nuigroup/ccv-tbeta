#include "ofxPS3.h"
#include <stdio.h>

// TODO: move this quiery to the PS3EyeLib driver
unsigned int ofxPS3::camNum=1;

ofxPS3::ofxPS3() 
{
}

void ofxPS3::listDevices()
{
	// PS3 Camera Testing
	pCam = IPS3EyeLib::Create(0);

	// Query supported video formats
	printf("printing formats...\n");
	for(int i=0; i<IPS3EyeLib::GetNumFormats(); i++) 
	{
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
	pCam->AutoAGC(false);
	pCam->AutoAEC(false);
	//pCam->SetExposure(511);
	//pCam->SetGain(0);
}

int ofxPS3::getDeviceCount()
{
	camNum = IPS3EyeLib::GetNumCameras();
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
	// Stop capturing
	pCam->StopCapture();
	Sleep(50);
	// Free the
	delete pCam;
	// Free the image buffer
	delete [] pBuffer;
}
