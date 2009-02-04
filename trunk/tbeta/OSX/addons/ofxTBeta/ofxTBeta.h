#ifndef OFXTBETA_H
#define OFXTBETA_H
/*
 *  ofxTBeta.h
 *  tbeta
 *
 *  Created by Artem Titoulenko on 1/31/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */

#include "TouchMessenger.h"

//Used for tracking algo
//
#include "Tracking/BlobManager.h"
#include "Tracking/tracking.h"

//GPU image filters
#include "Filters/ImageFilter.h"

//Used for warped image calibration
#include "Calibration/boxAlign.h"

//Used other calibration
#include "Calibration/calibrationB.h"

//Communications
#include "Communication/TUIOOSC.h"

//GUI
#include "ofxGui/ofxGui.h"
#include "ofxGui/ofxGuiTypes.h"

#include "TBetaBase.h"

#endif