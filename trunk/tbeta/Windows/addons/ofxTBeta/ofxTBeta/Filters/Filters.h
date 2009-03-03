/*
* Copyright 2008 NOR_/D <http://nortd.com>
*
*
* * *
* The base class of all filters.
*/

#ifndef FILTERS_H_
#define FILTERS_H_

#include "ofMain.h"
#include "ofxCvMain.h"
#include "CPUImageFilter.h"

#define CAMERA_EXPOSURE_TIME  2200.0f

class Filters {
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
    ofxCvGrayscaleImage gpuReadBackImageGS;
    GPUImageFilter*	contrastFilter;
    GPUImageFilter*	subtractFilter;
    GPUImageFilter*	subtractFilter2; //we need 2 because we are showing the output of each
    GPUImageFilter*	gaussVFilter;
    GPUImageFilter*	gaussVFilter2;
    GPUImageFilter*	gaussHFilter;
    GPUImageFilter*	gaussHFilter2;
    GPUImageFilter*	threshFilter;
    GPUImageFilter*	copyFilter;
    GPUImageFilter*	grayScale;

    virtual void allocate( int w, int h ) = 0;
    virtual void applyCPUFilters(CPUImageFilter& img) = 0;
    virtual void learnBackground( ofxCvGrayscaleImage& live, ofxCvGrayscaleImage& _grayBg, ofxCvFloatImage& fLearn, float learnRate ) = 0;
    virtual void draw() = 0;

    virtual void allocateGPU() = 0;
    virtual void applyGPUFilters() = 0;
    virtual void drawGPU() = 0;
};


#endif
