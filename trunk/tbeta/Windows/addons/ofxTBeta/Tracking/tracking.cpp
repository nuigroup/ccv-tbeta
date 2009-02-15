/*
 *  tracking.cpp
 *
 *  Created by Ramsin Khoshabeh on 5/4/08.
 *  Copyright 2008 risenparadigm. All rights reserved.
 *
 * Changelog:
 * 08/15/08 -- Fixed a major bug in the track algorithm
 */

#include "tracking.h"

BlobTracker::BlobTracker()
{
	IDCounter = 0;
	TouchEvents.addListener(this);
}


void BlobTracker::passInCalibration(calibrationB* calibrater) {

    calibrate = calibrater;
 //   calibrate->setCamRes(320, 240);
 //   calibrate->loadXMLSettings();
}



//assigns IDs to each blob in the contourFinder
void BlobTracker::track(ofxTBetaCvContourFinder* newBlobs)
{
	//printf("starting time: %.3f\n", dTime.time);

	
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
			TouchEvents.RAWmessenger = trackedBlobs[i];
			TouchEvents.notifyRAWTouchUp(NULL);

			calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
			calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);

            if(TouchEvents.messenger.centroid.x != 0 && TouchEvents.messenger.centroid.y != 0)
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

//TODO--------------------------------------------------------------------------
						//now the old winning blob has lost the win.
						//I should also probably go through all the newBlobs
						//at the end of this loop and if there are ones without
						//any winning matches, check if they are close to this
						//one. Right now I'm not doing that to prevent a
						//recursive mess. It'll just be a new track.

						//SEND BLOB OFF EVENT
                        TouchEvents.messenger = trackedBlobs[i];
						TouchEvents.RAWmessenger = trackedBlobs[i];
                        TouchEvents.notifyRAWTouchUp(NULL);

                        calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
                        calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);

                        if(TouchEvents.messenger.centroid.x != 0 && TouchEvents.messenger.centroid.y != 0)
						TouchEvents.notifyTouchUp(NULL);
						//mark the blob for deletion
						trackedBlobs[j].id = -1;
//------------------------------------------------------------------------------
					}
					else //delete
					{
						//SEND BLOB OFF EVENT
                        TouchEvents.messenger = trackedBlobs[i];
						TouchEvents.RAWmessenger = trackedBlobs[i];
                        TouchEvents.notifyRAWTouchUp(NULL);

                        calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
                        calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);

                        if(TouchEvents.messenger.centroid.x != 0 && TouchEvents.messenger.centroid.y != 0)
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
			}
		}
	}

	//--Update All Current Tracks
	//remove every track labeled as dead (ID='-1')
	//find every track that's alive and copy it's data from newBlobs
	for(int i=0; i<trackedBlobs.size(); i++)
	{
		if(trackedBlobs[i].id==-1) //dead
		{
			//erase track
			trackedBlobs.erase(trackedBlobs.begin()+i,
							   trackedBlobs.begin()+i+1);

			i--; //decrement one since we removed an element
		}
		else //living, so update it's data
		{
			for(int j=0; j<newBlobs->nBlobs; j++)
			{
				if(trackedBlobs[i].id==newBlobs->blobs[j].id)
				{
					//update track
					ofPoint tempLastCentroid = trackedBlobs[i].centroid; // assign the new centroid to the old
					trackedBlobs[i]=newBlobs->blobs[j];
					trackedBlobs[i].lastCentroid = tempLastCentroid;

					trackedBlobs[i].D.set(trackedBlobs[i].centroid.x - trackedBlobs[i].lastCentroid.x, trackedBlobs[i].centroid.y - trackedBlobs[i].lastCentroid.y);

					ofPoint tD = trackedBlobs[i].D;

					trackedBlobs[i].maccel = sqrtf((tD.x* tD.x)+(tD.y*tD.y));
					
					
                    if(abs((int)trackedBlobs[i].lastCentroid.x - (int)trackedBlobs[i].centroid.x) < 3 && 
						abs((int)trackedBlobs[i].lastCentroid.y - (int)trackedBlobs[i].centroid.y) < 3) {
						
						trackedBlobs[i].centroid.x = trackedBlobs[i].lastCentroid.x;
						trackedBlobs[i].centroid.y = trackedBlobs[i].lastCentroid.y;
						trackedBlobs[i].sitting += 1; //1 more frame of sitting
						printf("%i sitting: %i\n",(int)trackedBlobs[i].id, trackedBlobs[i].sitting);
					} else {
						trackedBlobs[i].sitting = 0;
					}
					
					
					trackedBlobs[i].age += dTime.time; //add the time diff from the last dTime.countTime();
					dTime.countTime();
					
					printf("%i age: %.3f\n", i, trackedBlobs[i].age);
					
					if(trackedBlobs[i].age > 1000 && trackedBlobs[i].sitting > 10) {
						TouchEvents.messenger = trackedBlobs[i];
						TouchEvents.RAWmessenger = trackedBlobs[i];
						
						TouchEvents.notifyRAWTouchHeld(NULL);
						
						calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
						calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);

						TouchEvents.notifyTouchHeld(NULL);
					} else {
						//printf("(%f, %f) -> (%f, %f) \n", trackedBlobs[i].lastCentroid.x, trackedBlobs[i].lastCentroid.y, trackedBlobs[i].centroid.x, trackedBlobs[i].centroid.y);

						//SEND BLOB MOVED EVENT
						TouchEvents.messenger = trackedBlobs[i];
						TouchEvents.RAWmessenger = trackedBlobs[i];
						TouchEvents.notifyRAWTouchMoved(NULL);

						calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
						calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);

						if(TouchEvents.messenger.centroid.x != 0 && TouchEvents.messenger.centroid.y != 0)
						TouchEvents.notifyTouchMoved(NULL);
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
		if(newBlobs->blobs[i].id==-1)
		{
			dTime.countTime();
			
			//add new track
			newBlobs->blobs[i].id=IDCounter++;
			//printf("id!!!! : %i \n", IDCounter);
			trackedBlobs.push_back(newBlobs->blobs[i]);

			//SEND BLOB DOWN EVENT
            TouchEvents.messenger = trackedBlobs[i];
			TouchEvents.RAWmessenger = trackedBlobs[i];
            TouchEvents.notifyRAWTouchDown(NULL);

            calibrate->transformDimension(TouchEvents.messenger.boundingRect.width, TouchEvents.messenger.boundingRect.height);
            calibrate->cameraToScreenPosition(TouchEvents.messenger.centroid.x, TouchEvents.messenger.centroid.y);

            if(TouchEvents.messenger.centroid.x != 0 && TouchEvents.messenger.centroid.y != 0)
			TouchEvents.notifyTouchDown(NULL);
		}
	}
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

int BlobTracker::trackKnn(ofxTBetaCvContourFinder *newBlobs, ofxTBetaCvBlob *track, int k, double thresh = 0)
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
