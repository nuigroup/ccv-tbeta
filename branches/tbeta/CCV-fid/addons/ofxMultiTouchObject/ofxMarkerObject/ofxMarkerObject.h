/*
 Multitouch Marker Object for OpenFrameworks 
 Copyright (c) 2009 Matthias Dörfelt <info@mokafolio.de>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _OFXMARKEROBJECT
#define _OFXMARKEROBJECT

#include "ofxMtObject.h"
#include "ofxMarkerHandler.h"


//this class extends ofxMtObject, so you can make it react to touches and
//markers at the same time, whatever makes sense!
class ofxMarkerObject : public ofxMtObject, public ofxMarkerListener{
	
public:
	ofxMarkerObject();
	virtual ~ofxMarkerObject();
	void registerForMarkerEvents();
	
	//flashStyle Callbacks for Marker Events
	virtual void onMarkerDown(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){};
	virtual void onMarkerMoved(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){};
	virtual void onMarkerUp(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){};
	virtual void onMarkerRollOver(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){};
	virtual void onMarkerRollOut(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){};
	
protected:
	//global Callbacks, don't modify anthing unless you know what you are doing!
	void markerMoved(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL);
	void markerDown(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL);
	void markerUp(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL);
	
	//keeps track of all markers active on the object
	std::list<int> activeMarkers;

};

#endif
