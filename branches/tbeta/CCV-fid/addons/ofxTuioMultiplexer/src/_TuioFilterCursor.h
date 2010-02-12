#ifndef INCLUDED_tuioMultiplexerCURSOR_H
#define INCLUDED_tuioMultiplexerCURSOR_H

//#include "ofxTuioMultiplexer.h"
#include "TuioCursor.h"
#include "ofxVectorMath.h"
//using namespace TUIO ;
	class tuioMultiplexerCursor {
		
	protected:
		/**
		 * The individual cursor ID number that is assigned to each TuioCursor.
		 */ 
		//int cursor_id;
		
		//struct Tuio_Source * source;
		//struct Screen * screen;
		//struct Tuio_Source* source;
		
	public:
		//using TuioContainer::update;
		//struct Tuio_Source* _source, 
		tuioMultiplexerCursor(TUIO::TuioCursor * _cursor);
		//tuioMultiplexerCursor();

		std::list<TUIO::TuioCursor *> cursorList;

		


		void addTuioCursor(TUIO::TuioCursor * cursor);
		void removeTuioCursor( TUIO::TuioCursor * cursor);
		//TUIO::TuioCursor * gethighestTuioCursor();

		//float getX()const;
		
		//float getY() const;

		 /**
		 * The destructor is doing nothing in particular. 
		 */
		~tuioMultiplexerCursor(){};

};
//};

#endif