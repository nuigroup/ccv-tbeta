/*
 MultitouchObject for OpenFrameworks 
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

//Use this as a base class for your buttons if you want to use a callback 
//function in OF testApp.cpp

#ifndef _OFXMTOBJECTEVENTS
#define _OFXMTOBJECTEVENTS

#include "ofxMtObject.h"

static ofEventArgs voidEventArgs;

class ofxMtObjectEvents :  public ofxMtObject{
	
public:
	
	//flashStyle Callbacks for Multitouch Events
	virtual void onMultiTouchDown(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){
		ofNotifyEvent(eMultiTouchDown, voidEventArgs, this);
	};
	virtual void onMultiTouchMoved(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){
		ofNotifyEvent(eMultiTouchMoved, voidEventArgs, this);
	};
	virtual void onMultiTouchUp(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){
		ofNotifyEvent(eMultiTouchUp, voidEventArgs, this);
	};
	virtual void onMultiTouchRollOut(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){
		ofNotifyEvent(eMultiTouchRollOut, voidEventArgs, this);	
	};
	virtual void onMultiTouchRollOver(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){
		ofNotifyEvent(eMultiTouchRollOver, voidEventArgs, this);	
	};
	//not implemented yet
	virtual void onMultiTouchDoubleTap(float x, float y, int touchId, ofxMultiTouchCustomData *data = NULL){
		ofNotifyEvent(eMultiTouchDoubleTap, voidEventArgs, this);
	};
	
	//Events
	ofEvent<ofEventArgs> eMultiTouchDown;
	ofEvent<ofEventArgs> eMultiTouchUp;
	ofEvent<ofEventArgs> eMultiTouchMoved;
	ofEvent<ofEventArgs> eMultiTouchRollOver;
	ofEvent<ofEventArgs> eMultiTouchRollOut;
	ofEvent<ofEventArgs> eMultiTouchDoubleTap;
};

#endif
