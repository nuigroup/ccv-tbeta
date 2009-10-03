////////////////////////////////////////////////////////////////////////////////////////////
//
// This library allows you to use multiple PS3Eye cameras in your own applications.
//
// For updates, more information and downloads visit: 
// http://www.alexpopovich.com or
// http://alexpopovich.wordpress.com/
//
// Copyright 2008,2009 (c) AlexP.  All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <windows.h>

#ifdef PS3EYEMULTICAM_EXPORTS
#define PS3EYEMULTICAM_API extern "C" __declspec(dllexport)
#else
#define PS3EYEMULTICAM_API extern "C" __declspec(dllimport)
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// PS3EyeMulticam API
////////////////////////////////////////////////////////////////////////////////////////////

typedef enum{ QVGA, VGA }Resolution;

// Camera information
PS3EYEMULTICAM_API int PS3EyeMulticamGetCameraCount();

// Library initialization
PS3EYEMULTICAM_API bool PS3EyeMulticamOpen(int camCnt, Resolution res, int frameRate);
PS3EYEMULTICAM_API void PS3EyeMulticamClose();

// Capture control
PS3EYEMULTICAM_API bool PS3EyeMulticamStart();
PS3EYEMULTICAM_API void PS3EyeMulticamStop();

// Settings control
PS3EYEMULTICAM_API bool PS3EyeMulticamLoadSettings(char* fileName="settings.xml");
PS3EYEMULTICAM_API bool PS3EyeMulticamSaveSettings(char* fileName="settings.xml");
PS3EYEMULTICAM_API void PS3EyeMulticamShowSettings();

// Processed frame image data retrieval
PS3EYEMULTICAM_API bool PS3EyeMulticamGetFrameDimensions(int &width, int &height);
PS3EYEMULTICAM_API bool PS3EyeMulticamGetFrame(PBYTE pData, int waitTimeout=2000);

////////////////////////////////////////////////////////////////////////////////////////////
