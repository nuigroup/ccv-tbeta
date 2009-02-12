#ifndef Touch_MESSENGER_H
#define Touch_MESSENGER_H
/*
 *  TouchMessenger.h
 *  tbeta
 *
 *  Created by Artem Titoulenko on 2/1/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */
#include "Poco/FIFOEvent.h"
#include "Poco/Delegate.h"
#include "ofTypes.h"

#include "Tracking/ofxTBetaCvContourFinder.h"

// message code
//	copied, adapted, practiced from ofMouseEvents.h

/* THE DEAL WITH THE RAW

 RAW Events added - Uncalibrated RAW events. Their
 main objective is to get the raw camera blobs
 without calibration so as to provide calibration.
 Anyone can use them, but it's unneccesary
 because the normal TouchDown, TouchUp, and TouchMoved
 get the calibrated blobs. Enjoy!

 */

class content
	{
	public:
		int id, num;
		float x, y, movx, movy;
		content( int _num, float _x, float _y, float _movx, float _movy){
			id = -1;
			num = _num;
			x = _x;
			y = _y;
			movx = _movx;
			movy = _movy;
		}
		void set(int _num, float _x, float _y, float _movx, float _movy){
			num = _num;
			x = _x;
			y = _y;
			movx = _movx;
			movy = _movy;
		}
	};

class TouchMessage : public ofEventArgs {
public:
	vector<content*> contents;
	vector<int> alive;
};

class TouchListener {
protected:

	virtual void TouchDown(ofxTBetaCvBlob message){}
	virtual void TouchUp(ofxTBetaCvBlob message){}
	virtual void TouchMoved(ofxTBetaCvBlob message){}

	//RAW
	virtual void RAWTouchDown(ofxTBetaCvBlob message){}
	virtual void RAWTouchUp(ofxTBetaCvBlob message){}
	virtual void RAWTouchMoved(ofxTBetaCvBlob message){}

	void TouchDown(const void* sender, ofxTBetaCvBlob& eventArgs){
		TouchDown(eventArgs);
	}
	void TouchUp(const void* sender, ofxTBetaCvBlob& eventArgs){
		TouchUp(eventArgs);
	}
	void TouchMoved(const void* sender, ofxTBetaCvBlob& eventArgs){
		TouchMoved(eventArgs);
	}

	//RAW
	void RAWTouchDown(const void* sender, ofxTBetaCvBlob& eventArgs){
		RAWTouchDown(eventArgs);
	}
	void RAWTouchUp(const void* sender, ofxTBetaCvBlob& eventArgs){
		RAWTouchUp(eventArgs);
	}
	void RAWTouchMoved(const void* sender, ofxTBetaCvBlob& eventArgs){
		RAWTouchMoved(eventArgs);
	}
};

class TouchManager
	{
	public:
		//TouchMessage messenger;
		ofxTBetaCvBlob messenger;
		ofxTBetaCvBlob RAWmessenger;

		void addListener(TouchListener* listener){
			addTouchDownListener(listener);
			addTouchUpListener(listener);
			addTouchMovedListener(listener);
		}

		void addRAWListener(TouchListener* listener){
			addRAWTouchDownListener(listener);
			addRAWTouchUpListener(listener);
			addRAWTouchMovedListener(listener);
		}

		//Listeners -----------
		void addTouchDownListener(TouchListener* listener){ //TouchDown
			TouchDown += Poco::Delegate<TouchListener, ofxTBetaCvBlob>(listener, &TouchListener::TouchDown);
		}
		void addTouchUpListener(TouchListener* listener){ //TouchUp
			TouchUp += Poco::Delegate<TouchListener, ofxTBetaCvBlob>(listener, &TouchListener::TouchUp);
		}
		void addTouchMovedListener(TouchListener* listener){ //TouchMoved
			TouchMoved += Poco::Delegate<TouchListener, ofxTBetaCvBlob>(listener, &TouchListener::TouchMoved);
		}

		//raw listeners
		void addRAWTouchDownListener(TouchListener* listener){ //TouchDown
			RAWTouchDown += Poco::Delegate<TouchListener, ofxTBetaCvBlob>(listener, &TouchListener::RAWTouchDown);
		}
		void addRAWTouchUpListener(TouchListener* listener){ //TouchUp
			RAWTouchUp += Poco::Delegate<TouchListener, ofxTBetaCvBlob>(listener, &TouchListener::RAWTouchUp);
		}
		void addRAWTouchMovedListener(TouchListener* listener){ //TouchMoved
			RAWTouchMoved += Poco::Delegate<TouchListener, ofxTBetaCvBlob>(listener, &TouchListener::RAWTouchMoved);
		}




		//message recievers ---
		void notifyTouchDown(void* sender){ //TouchDown
			TouchDown.notify(sender, messenger);
		}
		void notifyTouchUp(void* sender){ //TouchUp
			TouchUp.notify(sender, messenger);
		}
		void notifyTouchMoved(void* sender){ //TouchMoved
			TouchMoved.notify(sender, messenger);
		}

		//RAW
		void notifyRAWTouchDown(void* sender){ //TouchDown
			RAWTouchDown.notify(sender, RAWmessenger);
		}
		void notifyRAWTouchUp(void* sender){ //TouchUp
			RAWTouchUp.notify(sender, RAWmessenger);
		}
		void notifyRAWTouchMoved(void* sender){ //TouchMoved
			RAWTouchMoved.notify(sender, RAWmessenger);
		}






		//events -------------- MONGREL
	private:
		Poco::FIFOEvent<ofxTBetaCvBlob> TouchDown;
		Poco::FIFOEvent<ofxTBetaCvBlob> TouchUp;
		Poco::FIFOEvent<ofxTBetaCvBlob> TouchMoved;

		//RAW
		Poco::FIFOEvent<ofxTBetaCvBlob> RAWTouchDown;
		Poco::FIFOEvent<ofxTBetaCvBlob> RAWTouchUp;
		Poco::FIFOEvent<ofxTBetaCvBlob> RAWTouchMoved;

		bool becomeSkyNet;

	};

extern TouchManager TouchEvents;


#endif
