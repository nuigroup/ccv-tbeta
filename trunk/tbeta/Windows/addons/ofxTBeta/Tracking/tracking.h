/*
 *  tracking.h
 *
 *  Created by Ramsin Khoshabeh on 5/4/08.
 *  Copyright 2008 risenparadigm. All rights reserved.
 *
 * Changelog:
 * 08/15/08 -- Fixed a major bug in the track algorithm
 */

#ifndef __TRACKING
#define __TRACKING

#include <list>
#include <map>

#include "ofxTBetaCvContourFinder.h"

#include "../TouchMessenger.h"

#include "../Calibration/calibrationB.h"

class deltaTime
{
public:
	float time;
	
	deltaTime() {
		time= 0.0f;
		timeOne=0.0f;
		switcher=true;
	}
	~deltaTime() {
	}
	
	void countTime() {
		if(switcher) {
			timeOne = 0.0f;
			timeOne = ofGetElapsedTimeMillis(); // get the first value
			switcher = !switcher; //flip teh switchez!
		} else {
			time = ofGetElapsedTimeMillis() - timeOne; // diff between this and that ^
			switcher = !switcher; // more flipzz
		}
	}
	
private:
	float timeOne;
	bool switcher;
};

class BlobTracker : public TouchListener
{

public:

	BlobTracker();
	
	
	deltaTime dTime;

	calibrationB* calibrate;

	void passInCalibration(calibrationB* calibrate);

	//assigns IDs to each blob in the contourFinder
	void track(ofxTBetaCvContourFinder* newBlobs);
	int trackKnn(ofxTBetaCvContourFinder *newBlobs, ofxTBetaCvBlob *track, int k, double thresh);

private:

	std::vector<ofxTBetaCvBlob>	trackedBlobs; //tracked blobs
	int						IDCounter;	  //counter of last blob
	int						fightMongrel;
};

#endif
