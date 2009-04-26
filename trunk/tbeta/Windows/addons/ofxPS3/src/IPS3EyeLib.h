//////////////////////////////////////////////////////////////////////////////////////
// The library allows you to integrate and use this great 
// Sony PS3Eye camera in your own applications.
//
// For updates and file downloads go:
// http://www.alexpopovich.com/blog/?cat=5
//
// Copyright 2008,2009 (c) AlexP.  All rights reserved.
//
// PS3EyeLib Usage Example:
//
//  // Query for PS3Eye cameras 
//  // Returns 0 if no cams are detected and 1 if there is a camera
//  int cnum = IPS3EyeLib::GetNumCameras();
//  if(cnum==0)
//  {
//      // No cameras installed
//      return;
//  }
// 	// Create PS3EyeLib camera object
// 	IPS3EyeLib *pCam=IPS3EyeLib::Create(0);
// 	// Query supported video formats
// 	for(int i=0; i<IPS3EyeLib::GetNumFormats(); i++)
// 	{
// 		int width, height, rate;
// 		char *description;
// 		width=IPS3EyeLib::GetFormats()[i].width;
// 		height=IPS3EyeLib::GetFormats()[i].height;
// 		rate=IPS3EyeLib::GetFormats()[i].rate;
// 		description=IPS3EyeLib::GetFormats()[i].formatTxt;
// 	}
// 	// Decide on the format to use (Example: 320x240 * 30fps)
// 	// Select this format
// 	pCam->SetFormat(IPS3EyeLib::GetFormatIndex(320,240,30));
// 	// Allocate image buffer (we are going to capture 24bit RGB images)
//	// The supported color depths are 8, 16, 24 and 32
// 	PBYTE pBuffer=new BYTE[(320*240*24)/8];
// 	// Start capturing
// 	pCam->StartCapture();
// 	// Process frames until 'done'
// 	bool done=false;
// 	while(!done)
// 	{
// 		// This function call will block until a new frame is available
// 		// It will then fill the buffer with frame image data
// 		if(pCam->GetFrame(pBuffer, 24, false))
// 		{
// 			// Process/display video frame here
// 			// …
// 			// When your program is done set ’done=true;’
// 		}
// 	}
// 	// Free the image buffer
// 	delete [] pBuffer;
// 	// Stop capturing
// 	pCam->StopCapture();
// 	// Free the 
// 	delete pCam;
//
//////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <windows.h>

#ifdef BUILD_PS3EYELIB_DLL
#define PS3EYELIB_EXPORT __declspec(dllexport)
#else
#define PS3EYELIB_EXPORT __declspec(dllimport)
#endif

// pure function macro
#ifndef PURE
#define PURE = 0
#endif

// camera resolution
typedef enum
{
	R320x240,
	R640x480,
}PS3EYE_RESOLUTION;

// camera format structure
typedef struct tFormat
{
	PS3EYE_RESOLUTION res;	// resolution
	int width;				// frame width
	int height;				// frame height
	DWORD rate;				// frames per second
	LONGLONG period;		// in 100ns units
	char formatTxt[32];		// description
}PS3EYE_FORMAT;

class PS3EYELIB_EXPORT IPS3EyeLib
{
public:
	// get number of connected PS3Eye cameras
	static int GetNumCameras();
	// create PS3Eye camera object
	static IPS3EyeLib *Create(int CameraIndex);

public:
	virtual ~IPS3EyeLib();

	// PS3EyeLib GUID
	static GUID PS3EyeLibGUID();

	// camera format
	virtual void SetFormat(int formatIdx) PURE;
	virtual UINT GetWidth() PURE;
	virtual UINT GetHeight() PURE;
	virtual PS3EYE_RESOLUTION GetRes() PURE;
	virtual DWORD GetRate() PURE;
	
	// supported formats
	static PS3EYE_FORMAT *GetFormats();
	static int GetNumFormats();
	static int GetFormatIndex(int width, int height, LONGLONG period);
	static int GetFormatIndex(int width, int height, int rate);

	// capture control
	virtual void StartCapture() PURE;
	bool GetFrameRAW(PBYTE *pBuf, bool wait);
	bool GetFrame(PBYTE buf, int bitsPerPixel, bool flip, bool wait=true);
	virtual void StopCapture() PURE;
	virtual bool IsCapturing() PURE;

	// camera hardware
	virtual void DumpEEPROM(LPCTSTR filename) PURE;
	virtual void DumpRAM(LPCTSTR filename) PURE;
	virtual void LED(bool on) PURE;

	// camera version
	virtual LPCTSTR GetCameraVersion() PURE;

	// camera video control
	// these settings are effective while camera 
	// is capturing video frames
	// the settings persist even between capture sessions
	virtual bool IsAutoAGC() PURE;
	virtual void AutoAGC(bool enable) PURE;
	// Gain value [0..79]
	virtual int GetGain() PURE;
	virtual void SetGain(int value) PURE;

	virtual bool IsAutoAEC() PURE;
	virtual void AutoAEC(bool enable) PURE;
	// Exposure value [0..511]
	virtual int GetExposure() PURE;
	virtual void SetExposure(int value) PURE;

	virtual bool IsAutoAWB() PURE;
	virtual void AutoAWB(bool enable) PURE;
	virtual void GetWhiteBalance(BYTE *r, BYTE *g, BYTE *b) PURE;
	virtual void SetWhiteBalance(BYTE r, BYTE g, BYTE b) PURE;

	virtual bool IsColorBar() PURE;
	virtual void ColorBar(bool enable) PURE;
};
