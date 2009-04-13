/*
 *  Tracking.h
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

#include "ContourFinder.h"

#include "../Events/TouchMessenger.h"

#include "../Calibration/CalibrationUtils.h"

class BlobTracker : public TouchListener
{

public:

	BlobTracker();

	CalibrationUtils* calibrate;

	bool isCalibrating;

	int MIN_MOVEMENT_THRESHOLD;

	void passInCalibration(CalibrationUtils* calibrate);

	std::vector<Blob> getTrackedBlobs();


	//assigns IDs to each blob in the contourFinder
	void track(ContourFinder* newBlobs);
	int trackKnn(ContourFinder *newBlobs, Blob *track, int k, double thresh);

private:

	int						IDCounter;	  //counter of last blob
	int						fightMongrel;
	std::vector<Blob>	trackedBlobs; //tracked blobs
};

#endif
