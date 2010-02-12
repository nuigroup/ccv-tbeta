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

#ifndef _OFXMARKEROBJECTEVENTS
#define _OFXMARKEROBJECTEVENTS

#include "ofxMtObject.h"
#include "ofxMarkerObject.h"

static ofEventArgs voidEventArgs;

class ofxMarkerObjectEvents : public ofxMarkerObject{
	
public:
	
	//flashStyle Callbacks for Multitouch Events
	virtual void onMarkerDown(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){
		ofNotifyEvent(eMarkerDown, voidEventArgs, this);
	};
	virtual void onMarkerMoved(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){
		ofNotifyEvent(eMarkerMoved, voidEventArgs, this);
	};
	virtual void onMarkerUp(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){
		ofNotifyEvent(eMarkerUp, voidEventArgs, this);
	};
	virtual void onMarkerRollOver(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){
		ofNotifyEvent(eMarkerRollOver, voidEventArgs, this);	
	};
	virtual void onMarkerRollOut(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL){
		ofNotifyEvent(eMarkerRollOut, voidEventArgs, this);	
	};
	
	//Events
	ofEvent<ofEventArgs> eMarkerDown;
	ofEvent<ofEventArgs> eMarkerUp;
	ofEvent<ofEventArgs> eMarkerMoved;
	ofEvent<ofEventArgs> eMarkerRollOver;
	ofEvent<ofEventArgs> eMarkerRollOut;
};

#endif

