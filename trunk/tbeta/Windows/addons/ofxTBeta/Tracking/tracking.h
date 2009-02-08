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

class BlobTracker : public TouchListener
{

public: 

	BlobTracker();
	
	//assigns IDs to each blob in the contourFinder
	void track(ofxTBetaCvContourFinder* newBlobs);
	int trackKnn(ofxTBetaCvContourFinder *newBlobs, ofxTBetaCvBlob *track, int k, double thresh);

private:
	
	std::vector<ofxTBetaCvBlob>	trackedBlobs; //tracked blobs
	int						IDCounter;	  //counter of last blob
	int						fightMongrel;
};

#endif