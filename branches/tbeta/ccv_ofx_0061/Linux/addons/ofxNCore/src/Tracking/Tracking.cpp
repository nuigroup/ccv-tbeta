/*
*  Tracking.cpp
*
*  Created by Ramsin Khoshabeh on 5/4/08.
*  Copyright 2008 risenparadigm. All rights reserved.
*
*  Changelog:
*  08/15/08 -- Fixed a major bug in the track algorithm
*
*
*/

#include "Tracking.h"

BlobTracker::BlobTracker()
{
	IDCounter = 0;
	isCalibrating = false;
}

BlobTracker::~BlobTracker(){

	delete calibrate;
}

void BlobTracker::passInCalibration(CalibrationUtils* calibrater) {

    calibrate = calibrater;
}

//assigns IDs to each blob in the contourFinder
void BlobTracker::track(ContourFinder* newBlobs)
{
	//initialize ID's of all blobs
	for(int i=0; i<newBlobs->nBlobs; i++)
		newBlobs->blobs[i].id=-1;

	//go through all tracked blobs to compute nearest new point
	for(int i=0; i<trackedBlobs.size(); i++)
	{
		/******************************************************************
		 * *****************TRACKING FUNCTION TO BE USED*******************
		 * Replace 'trackKnn(...)' with any function that will take the
		 * current track and find the corresponding track in the newBlobs
		 * 'winner' should contain the index of the found blob or '-1' if
		 * there was no corresponding blob
		 *****************************************************************/
		int winner = trackKnn(newBlobs, &(trackedBlobs[i]), 3, 0);

		if(winner==-1) //track has died, mark it for deletion
		{
			//SEND BLOB OFF EVENT
			TouchEvents.messenger = trackedBlobs[i];

			if(isCalibrating){
				TouchEvents.RAWmessenger = trackedBlobs[i];
				TouchEvents.notifyRAWTouchUp(NULL);
			}
			calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
			calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);
			//erase calibrated blob from map
			calibratedBlobs.erase(TouchEvents.messenger.id);

			TouchEvents.notifyTouchUp(NULL);
			//mark the blob for deletion
			trackedBlobs[i].id = -1;
		}
		else //still alive, have to update
		{
			//if winning new blob was labeled winner by another track\
			//then compare with this track to see which is closer
			if(newBlobs->blobs[winner].id!=-1)
			{
				//find the currently assigned blob
				int j; //j will be the index of it
				for(j=0; j<trackedBlobs.size(); j++)
				{
					if(trackedBlobs[j].id==newBlobs->blobs[winner].id)
						break;
				}

				if(j==trackedBlobs.size())//got to end without finding it
				{
					newBlobs->blobs[winner].id = trackedBlobs[i].id;
					newBlobs->blobs[winner].age = trackedBlobs[i].age;
					newBlobs->blobs[winner].sitting = trackedBlobs[i].sitting;
					newBlobs->blobs[winner].downTime = trackedBlobs[i].downTime;
					newBlobs->blobs[winner].color = trackedBlobs[i].color;
					newBlobs->blobs[winner].lastTimeTimeWasChecked = trackedBlobs[i].lastTimeTimeWasChecked;
          newBlobs->blobs[winner].lastCentroid = trackedBlobs[i].centroid;

					trackedBlobs[i] = newBlobs->blobs[winner];
				}
				else //found it, compare with current blob
				{
					double x = newBlobs->blobs[winner].centroid.x;
					double y = newBlobs->blobs[winner].centroid.y;
					double xOld = trackedBlobs[j].centroid.x;
					double yOld = trackedBlobs[j].centroid.y;
					double xNew = trackedBlobs[i].centroid.x;
					double yNew = trackedBlobs[i].centroid.y;
					double distOld = (x-xOld)*(x-xOld)+(y-yOld)*(y-yOld);
					double distNew = (x-xNew)*(x-xNew)+(y-yNew)*(y-yNew);

					//if this track is closer, update the ID of the blob
					//otherwise delete this track.. it's dead
					if(distNew<distOld) //update
					{
						newBlobs->blobs[winner].id = trackedBlobs[i].id;
						newBlobs->blobs[winner].age = trackedBlobs[i].age;
						newBlobs->blobs[winner].sitting = trackedBlobs[i].sitting;
						newBlobs->blobs[winner].downTime = trackedBlobs[i].downTime;
						newBlobs->blobs[winner].color = trackedBlobs[i].color;
						newBlobs->blobs[winner].lastTimeTimeWasChecked = trackedBlobs[i].lastTimeTimeWasChecked;
//TODO--------------------------------------------------------------------------
						//now the old winning blob has lost the win.
						//I should also probably go through all the newBlobs
						//at the end of this loop and if there are ones without
						//any winning matches, check if they are close to this
						//one. Right now I'm not doing that to prevent a
						//recursive mess. It'll just be a new track.

						//SEND BLOB OFF EVENT
						TouchEvents.messenger = trackedBlobs[j];
      			if(isCalibrating){
							TouchEvents.RAWmessenger = trackedBlobs[j];
							TouchEvents.notifyRAWTouchUp(NULL);
						}

            calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
            calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);

            //erase calibrated blob from map
						calibratedBlobs.erase(TouchEvents.messenger.id);

            TouchEvents.notifyTouchUp(NULL);
						//mark the blob for deletion
						trackedBlobs[j].id = -1;
//------------------------------------------------------------------------------
					}
					else //delete
					{
						//SEND BLOB OFF EVENT
						TouchEvents.messenger = trackedBlobs[i];
            if(isCalibrating){
							TouchEvents.RAWmessenger = trackedBlobs[i];
							TouchEvents.notifyRAWTouchUp(NULL);
						}

            calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
            calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);

            //erase calibrated blob from map
						calibratedBlobs.erase(TouchEvents.messenger.id);

						TouchEvents.notifyTouchUp(NULL);
						//mark the blob for deletion
						trackedBlobs[i].id = -1;
					}
				}
			}
			else //no conflicts, so simply update
			{
				newBlobs->blobs[winner].id = trackedBlobs[i].id;
				newBlobs->blobs[winner].age = trackedBlobs[i].age;
				newBlobs->blobs[winner].sitting = trackedBlobs[i].sitting;
				newBlobs->blobs[winner].downTime = trackedBlobs[i].downTime;
				newBlobs->blobs[winner].color = trackedBlobs[i].color;
				newBlobs->blobs[winner].lastTimeTimeWasChecked = trackedBlobs[i].lastTimeTimeWasChecked;
			}
		}
	}

	//--Update All Current Tracks
	//remove every track labeled as dead (ID='-1')
	//remove every track that is outside the calibration grid centroid.x = -1
	//find every track that's alive and copy it's data from newBlobs
	for(int i=0; i<trackedBlobs.size(); i++)
	{

		if(trackedBlobs[i].id==-1)//dead
		{
			//erase track
			trackedBlobs.erase(trackedBlobs.begin()+i,trackedBlobs.begin()+i+1);
			i--; //decrement one since we removed an element
		}
		else //living, so update it's data
		{
			for(int j=0; j<newBlobs->nBlobs; j++)
			{
			  //added to determine if new blob is within the calibration grid
			  Blob tempBlob = newBlobs->blobs[j];
			  calibrate->cameraToScreenPosition(tempBlob.centroid.x, tempBlob.centroid.y);
			  //if the id matches and eithor the newBlob is inside the grid or it is calibrating
				if(
            (trackedBlobs[i].id==newBlobs->blobs[j].id && tempBlob.centroid.x!=-1) ||
            (trackedBlobs[i].id==newBlobs->blobs[j].id &&isCalibrating)
          )
				{
					//update track
					ofPoint tempLastCentroid = trackedBlobs[i].centroid; // assign the new centroid to the old
					trackedBlobs[i]=newBlobs->blobs[j];
					trackedBlobs[i].lastCentroid = tempLastCentroid;

					//get the Differences in position
					trackedBlobs[i].D.set((trackedBlobs[i].centroid.x - trackedBlobs[i].lastCentroid.x) / (ofGetElapsedTimeMillis() - trackedBlobs[i].lastTimeTimeWasChecked),
										  (trackedBlobs[i].centroid.y - trackedBlobs[i].lastCentroid.y) / (ofGetElapsedTimeMillis() - trackedBlobs[i].lastTimeTimeWasChecked));

					//printf("D(%f, %f)\n", trackedBlobs[i].D.x, trackedBlobs[i].D.y);

					//if( abs((int)trackedBlobs[i].D.x) > 1 || abs((int)trackedBlobs[i].D.y) > 1) {
//						printf("\nUNUSUAL BLOB @ %f\n-----------------------\ntrackedBlobs[%i]\nD = (%f, %f)\nXY= (%f, %f)\nlastTimeTimeWasChecked = %f\nsitting = %f\n",
//							   ofGetElapsedTimeMillis(),
//							   i,
//							   trackedBlobs[i].D.x,  trackedBlobs[i].D.y,
//							   trackedBlobs[i].centroid.x, trackedBlobs[i].centroid.y,
//							   trackedBlobs[i].lastTimeTimeWasChecked,
//							   trackedBlobs[i].downTime,
//							   trackedBlobs[i].sitting
//						);
//					}


					//calculate the accelleration
					ofPoint tD = trackedBlobs[i].D;
					trackedBlobs[i].maccel = sqrtf((tD.x* tD.x)+(tD.y*tD.y)/(ofGetElapsedTimeMillis() - trackedBlobs[i].lastTimeTimeWasChecked));

					trackedBlobs[i].lastTimeTimeWasChecked = ofGetElapsedTimeMillis();

					//calculate the age
					trackedBlobs[i].age = ofGetElapsedTimef() - trackedBlobs[i].downTime;

					//if not moving more than min_movement_threshold then set to same position as last frame
          if(trackedBlobs[i].maccel < MIN_MOVEMENT_THRESHOLD)
					{	//this helps avoid jittery blobs
						trackedBlobs[i].centroid.x = trackedBlobs[i].lastCentroid.x;
						trackedBlobs[i].centroid.y = trackedBlobs[i].lastCentroid.y;
          }

					//set sitting (held length)
          if(trackedBlobs[i].maccel < 7)
					{	//1 more frame of sitting
						if(trackedBlobs[i].sitting != -1)
						trackedBlobs[i].sitting = ofGetElapsedTimef() - trackedBlobs[i].downTime;
					}
					else {
						trackedBlobs[i].sitting = -1;
					}

					//printf("time: %f\n", ofGetElapsedTimeMillis());
					//printf("%i age: %f, downTimed at: %f\n", i, trackedBlobs[i].age, trackedBlobs[i].downTime);

					//if blob has been 'holding/sitting' for 1 second send a held event
					if(trackedBlobs[i].sitting > 1.0f)
					{

						//SEND BLOB HELD EVENT
						TouchEvents.messenger = trackedBlobs[i];

						if(isCalibrating){
							TouchEvents.RAWmessenger = trackedBlobs[i];
							TouchEvents.notifyRAWTouchHeld(NULL);
						}

						//calibrated values
						calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
						calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);
						calibrate->cameraToScreenPosition(TouchEvents.messenger.lastCentroid.x, TouchEvents.messenger.lastCentroid.y);

						//Calibrated dx/dy
						TouchEvents.messenger.D.set((TouchEvents.messenger.centroid.x - TouchEvents.messenger.lastCentroid.x) / (ofGetElapsedTimeMillis() - TouchEvents.messenger.lastTimeTimeWasChecked),
													(TouchEvents.messenger.centroid.y - TouchEvents.messenger.lastCentroid.y) / (ofGetElapsedTimeMillis() - TouchEvents.messenger.lastTimeTimeWasChecked));

						TouchEvents.messenger.lastTimeTimeWasChecked = ofGetElapsedTimeMillis();

						//calibrated accelleration
						ofPoint tD2 = TouchEvents.messenger.D;
						TouchEvents.messenger.maccel = sqrtf((tD2.x* tD2.x)+(tD2.y*tD2.y)/(ofGetElapsedTimeMillis() - trackedBlobs[i].lastTimeTimeWasChecked));

						//add to calibration map
						calibratedBlobs[TouchEvents.messenger.id] = TouchEvents.messenger;
            //held event only happens once so set to -1
            trackedBlobs[i].sitting = -1;
            TouchEvents.notifyTouchHeld(NULL);


					}
					else
					{

						//printf("(%f, %f) -> (%f, %f) \n", trackedBlobs[i].lastCentroid.x, trackedBlobs[i].lastCentroid.y, trackedBlobs[i].centroid.x, trackedBlobs[i].centroid.y);

						//SEND BLOB MOVED EVENT
						TouchEvents.messenger = trackedBlobs[i];

						if(isCalibrating){
							TouchEvents.RAWmessenger = trackedBlobs[i];
							TouchEvents.notifyRAWTouchMoved(NULL);
						}

						//calibrated values
						calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
						calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);
						calibrate->cameraToScreenPosition(TouchEvents.messenger.lastCentroid.x, TouchEvents.messenger.lastCentroid.y);

						//Calibrated dx/dy
						TouchEvents.messenger.D.set((TouchEvents.messenger.centroid.x - TouchEvents.messenger.lastCentroid.x) / (ofGetElapsedTimeMillis() - TouchEvents.messenger.lastTimeTimeWasChecked),
													(TouchEvents.messenger.centroid.y - TouchEvents.messenger.lastCentroid.y) / (ofGetElapsedTimeMillis() - TouchEvents.messenger.lastTimeTimeWasChecked));


						TouchEvents.messenger.lastTimeTimeWasChecked = ofGetElapsedTimeMillis();


						//printf("d(%0.4f, %0.4f)\n", TouchEvents.messenger.D.x, TouchEvents.messenger.D.y);

						//calibrated accelleration
						ofPoint tD2 = TouchEvents.messenger.D;
						TouchEvents.messenger.maccel = sqrtf((tD2.x* tD2.x)+(tD2.y*tD2.y)/(ofGetElapsedTimeMillis() - trackedBlobs[i].lastTimeTimeWasChecked));

						//add to calibration map
						calibratedBlobs[TouchEvents.messenger.id] = TouchEvents.messenger;
            TouchEvents.notifyTouchMoved(NULL);
					}
				}
        else
        {
          if(trackedBlobs[i].id==newBlobs->blobs[j].id)
          {
            TouchEvents.messenger = trackedBlobs[i];
            calibrate->cameraToScreenPosition(TouchEvents.messenger.lastCentroid.x, TouchEvents.messenger.lastCentroid.y);
            TouchEvents.messenger.centroid = TouchEvents.messenger.lastCentroid;
            TouchEvents.notifyTouchUp(NULL);
            calibratedBlobs.erase(TouchEvents.messenger.id);
            trackedBlobs.erase(trackedBlobs.begin()+i,trackedBlobs.begin()+i+1);
            i--;
          }
        }
			}
		}
	}
	//--Add New Living Tracks
	//now every new blob should be either labeled with a tracked ID or\
	//have ID of -1... if the ID is -1... we need to make a new track
	for(int i=0; i<newBlobs->nBlobs; i++)
	{
	  //added to check if blob is within calibration grid
	  Blob tempBlob = newBlobs->blobs[i];
	  calibrate->cameraToScreenPosition(tempBlob.centroid.x, tempBlob.centroid.y);
    //if the id = -1 and the blob is within the calibration grid
		if(
        (newBlobs->blobs[i].id ==-1 && tempBlob.centroid.x != -1) ||
        (newBlobs->blobs[i].id ==-1 && isCalibrating)
      )
		{
			//add new track
			newBlobs->blobs[i].id=IDCounter++;
			newBlobs->blobs[i].downTime = ofGetElapsedTimef();
			//newBlobs->blobs[i].lastTimeTimeWasChecked = ofGetElapsedTimeMillis();

			//random color for blob. Could be useful?
			int r = ofRandom(0, 255);
            int g = ofRandom(0, 255);
            int b = ofRandom(0, 255);
            //Convert to hex
            int rgbNum = ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
            //Set color
            newBlobs->blobs[i].color = rgbNum;

			//Add to blob messenger
			TouchEvents.messenger = newBlobs->blobs[i];

			if(isCalibrating){
				TouchEvents.RAWmessenger = newBlobs->blobs[i];
				TouchEvents.notifyRAWTouchDown(NULL);
			}

      calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
      calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);
      //add to calibrated blob map
			//send touchDown event
			//add the blob to the trackedBlobs list
			calibratedBlobs[TouchEvents.messenger.id] = TouchEvents.messenger;
  		//Send Event
	  	TouchEvents.notifyTouchDown(NULL);
  	  trackedBlobs.push_back(newBlobs->blobs[i]);
		}
	}
}

std::map<int, Blob> BlobTracker::getTrackedBlobs(){

    return calibratedBlobs;
}

/*************************************************************************
* Finds the blob in 'newBlobs' that is closest to the trackedBlob with index
* 'ind' according to the KNN algorithm and returns the index of the winner
* newBlobs	= list of blobs detected in the latest frame
* track		= current tracked blob being tested
* k			= number of nearest neighbors to consider\
*			  1,3,or 5 are common numbers..\
*			  must always be an odd number to avoid tying
* thresh	= threshold for optimization
**************************************************************************/

int BlobTracker::trackKnn(ContourFinder *newBlobs, Blob *track, int k, double thresh = 0)
{

	int winner = -1; //initially label track as '-1'=dead
	if((k%2)==0) k++; //if k is not an odd number, add 1 to it

	//if it exists, square the threshold to use as square distance
	if(thresh>0)
		thresh *= thresh;

	//list of neighbor point index and respective distances
	std::list<std::pair<int,double> > nbors;
	std::list<std::pair<int,double> >::iterator iter;

	//find 'k' closest neighbors of testpoint
	double x, y, xT, yT, dist;
	for(int i=0; i<newBlobs->nBlobs; i++)
	{
		x = newBlobs->blobs[i].centroid.x;
		y = newBlobs->blobs[i].centroid.y;

		xT = track->centroid.x;
		yT = track->centroid.y;
		dist = (x-xT)*(x-xT)+(y-yT)*(y-yT);

		if(dist<=thresh)//it's good, apply label if no label yet and return
		{
			winner = i;
			return winner;
		}

		/****************************************************************
		* check if this blob is closer to the point than what we've seen
		*so far and add it to the index/distance list if positive
		****************************************************************/

		//search the list for the first point with a longer distance
		for(iter=nbors.begin(); iter!=nbors.end()
			&& dist>=iter->second; iter++);

		if((iter!=nbors.end())||(nbors.size()<k)) //it's valid, insert it
		{
			nbors.insert(iter, 1, std::pair<int, double>(i, dist));
			//too many items in list, get rid of farthest neighbor
			if(nbors.size()>k)
				nbors.pop_back();
		}
	}

	/********************************************************************
	* we now have k nearest neighbors who cast a vote, and the majority
	* wins. we use each class average distance to the target to break any
	* possible ties.
	*********************************************************************/

	// a mapping from labels (IDs) to count/distance
	std::map<int, std::pair<int, double> > votes;

	//remember:
	//iter->first = index of newBlob
	//iter->second = distance of newBlob to current tracked blob
	for(iter=nbors.begin(); iter!=nbors.end(); iter++)
	{
		//add up how many counts each neighbor got
		int count = ++(votes[iter->first].first);
		double dist = (votes[iter->first].second+=iter->second);

		/* check for a possible tie and break with distance */
		if(count>votes[winner].first || count==votes[winner].first
			&& dist<votes[winner].second)
		{
			winner = iter->first;
		}
	}

	return winner;
}
