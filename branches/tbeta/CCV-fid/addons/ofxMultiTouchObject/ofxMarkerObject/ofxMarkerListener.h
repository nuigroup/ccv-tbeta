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

#ifndef _OFXMARKERLISTENER
#define _OFXMARKERLISTENER

// contains custom data which specific implementations can override and add to
struct ofxMarkerCustomData {
};

/****** protocol, delegate, interface, whatever you want to call it ******/
class ofxMarkerListener {
public:
	virtual ~ofxMarkerListener(){};				
	
	virtual void markerMoved(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL)		= 0;
	virtual void markerDown(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL)		    = 0;
	virtual void markerUp(int sid, float x, float y, float ang, ofxMarkerCustomData *data = NULL)			= 0;
};

#endif