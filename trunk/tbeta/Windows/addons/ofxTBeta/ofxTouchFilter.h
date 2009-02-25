/*
* Copyright 2008 NOR_/D <http://nortd.com>
*
* This file is part of ofxTouch.
* ofxTouch is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version. For details
* see <http://www.gnu.org/licenses/>.
*
* * *
* The base class of all filters.
*/

#ifndef OFX_TOUCHAFILTER
#define OFX_TOUCHAFILTER

#include "ofMain.h"
#include "ofxCvMain.h"
#include "Filters/CPUImageFilter.h"

#define CAMERA_EXPOSURE_TIME  2200.0f

class ofxTouchFilter {
  public:

    int camWidth;
    int camHeight;
    int exposureStartTime;

    //filter values
	int threshold;
    int smooth;
    int highpassBlur;
    int highpassNoise;
    int	highpassAmp;

    float fLearnRate;// rate to learn background

    //filter bools
	bool bLearnBackground;
	bool bVerticalMirror;
	bool bHorizontalMirror;
	bool bDynamicBG;
	bool bSmooth;
	bool bHighpass;
	bool bAmplify;
    bool bThreshold;
    bool bLearnBakground;

    //CPU
    ofxCvFloatImage  floatBgImg;
    ofxCvGrayscaleImage  grayBgImg;
    ofxCvColorImage		sourceImg;
    ofxCvGrayscaleImage grayImg;
    ofxCvGrayscaleImage grayBg;
    ofxCvGrayscaleImage subtractBg;
    ofxCvGrayscaleImage grayDiff;
    ofxCvGrayscaleImage highpassImg;
    ofxCvGrayscaleImage ampImg;
    ofxCvFloatImage		fiLearn;

    //GPU
    GLuint			gpuBGTex;
    GLuint	gpuSourceTex;
    unsigned char * gpuReadBackBuffer;
    ofxCvColorImage gpuReadBackImage;
    ofxCvGrayscaleImage gpuReadBackImageGS;
    ImageFilter*	contrastFilter;
    ImageFilter*	subtractFilter;
    ImageFilter*	subtractFilter2; //we need 2 because we are showing the output of each
    ImageFilter*	gaussVFilter;
    ImageFilter*	gaussVFilter2;
    ImageFilter*	gaussHFilter;
    ImageFilter*	gaussHFilter2;
    ImageFilter*	threshFilter;
    ImageFilter*	copyFilter;

    virtual void allocate( int w, int h ) = 0;
    virtual void applyCPUFilters(CPUImageFilter& img) = 0;
    virtual void learnBackground( ofxCvGrayscaleImage& live, ofxCvGrayscaleImage& _grayBg, ofxCvFloatImage& fLearn, float learnRate ) = 0;
    virtual void draw() = 0;

    virtual void allocateGPU() = 0;
    virtual void applyGPUFilters() = 0;
    virtual void drawGPU() = 0;
};


#endif
