/*
 *  Tracking.h
 *
 *  Created by Ramsin Khoshabeh on 5/4/08.
 *  Copyright 2008 risenparadigm. All rights reserved.
 *
 * Changelog:
 * 08/15/08 -- Fixed a major bug in the track algorithm
 */

#ifndef _TRACKING_H
#define _TRACKING_H

#include <list>
#include <map>

#include "ContourFinder.h"
#include "../Events/TouchMessenger.h"
#include "../Calibration/CalibrationUtils.h"

class BlobTracker : public TouchListener
{

public:

	BlobTracker();
	~BlobTracker();
	//assigns IDs to each blob in the contourFinder
	void track(ContourFinder* newBlobs);
	void passInCalibration(CalibrationUtils* calibrate);

	CalibrationUtils* calibrate;
	bool isCalibrating;
	int MIN_MOVEMENT_THRESHOLD;
	std::map<int, Blob> getTrackedBlobs();

private:

	int trackKnn(ContourFinder *newBlobs, Blob *track, int k, double thresh);
	int						IDCounter;	  //counter of last blob
	int						fightMongrel;
	std::vector<Blob>		trackedBlobs; //tracked blobs
	std::map<int, Blob>     calibratedBlobs;
	//std::vector<pair<int,Blob>>		finalBlobs; //tracked blobs
};

#endif
