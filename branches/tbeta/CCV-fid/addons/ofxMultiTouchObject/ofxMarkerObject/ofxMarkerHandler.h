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

#ifndef _OFXMARKERHANDLER
#define _OFXMARKERHANDLER

#include <list>
#include "ofxMarkerListener.h"

class ofxMarkerHandler : public ofxMarkerListener {
public:
	void addListener(ofxMarkerListener * o) {
		listeners.push_back(o);
	}
	
	void removeListener(ofxMarkerListener * o) {
		listeners.remove(o);
	}
	
	// multitouch engine should call this when a 'down' is detected
	// use ofxMultiTouchCustomData *data to send any platform specific data
	void markerDown(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL) {
		for(std::list<ofxMarkerListener*>::iterator it=listeners.begin(); it!=listeners.end(); ++it) {
			ofxMarkerListener* o = *it;
			o->markerDown(sid, x, y, ang, data);
		}
	}
	
	// multitouch engine should call this when a 'move' (drag) is detected
	// use ofxMultiTouchCustomData *data to send any platform specific data
	void markerMoved(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL) {
		for(std::list<ofxMarkerListener*>::iterator it=listeners.begin(); it!=listeners.end(); ++it) {
			ofxMarkerListener* o = *it;
			o->markerMoved(sid, x, y, ang, data);
		}
	}
	
	// multitouch engine should call this when an 'up' is detected
	// use ofxMultiTouchCustomData *data to send any platform specific data
	void markerUp(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL) {
		for(std::list<ofxMarkerListener*>::iterator it=listeners.begin(); it!=listeners.end(); ++it) {
			ofxMarkerListener* o = *it;
			o->markerUp(sid, x, y, ang, data);
		}
	}
	
protected:
	std::list<ofxMarkerListener*> listeners;
	
};
extern ofxMarkerHandler ofxMarker;
#endif

