/*
 *  ofxNCore.h
 *  NUI Group Community Core Vision
 * 
 *  Created by NUI Group Dev Team A on 2/1/09.
 *  Copyright 2009 NUI Group/Inc. All rights reserved.
 *
 */

#ifndef OFXNCORE_H
#define OFXNCORE_H

//removes the 'x' button on windows which causes a crash due to a GLUT bug

#ifdef TARGET_WIN32
//HWND hwndConsole = FindWindowA(NULL, "tbeta");
//HMENU hMnu = ::GetSystemMenu(hwndConsole, FALSE);
//RemoveMenu(hMnu, SC_CLOSE, MF_BYCOMMAND);
#endif

#include "Events/TouchMessenger.h"

//Used for tracking algo
#include "Tracking/BlobManager.h"
#include "Tracking/tracking.h"

//GPU image filters
#include "Filters/GPUImageFilter.h"

//Used for warped image calibration
#include "Calibration/boxAlign.h"

//Used other calibration
#include "Calibration/calibrationB.h"

//Communications
#include "Communication/TUIOOSC.h"

//GUI
#include "Controls/ofxGui.h"
#include "Controls/ofxGuiTypes.h"

//#include "Camera/Camera.h"

// Modules (Audio/Visual/Physical) Custom modules that fall in each category...
#include "Modules/ofxNCoreBase.h" // Baseline Module Elements
#include "Modules/ofxNCoreVision.h" // Extended Module for Vision Processing

#endif
