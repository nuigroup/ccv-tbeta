#include "tuioMultiplexerCursor.h"
//using namespace TUIO;
//struct ofxTuioMultiplexer::Tuio_Source * source,
//tuioMultiplexerCursor::tuioMultiplexerCursor(){}

tuioMultiplexerCursor::tuioMultiplexerCursor(TUIO::TuioCursor * _cursor){
//cursor = _cursor;
//source = _source;
	cout << "tuioMultiplexerCursor---------" << endl;
	addTuioCursor(_cursor);
//screen = _screen;
}

/*
TUIO::TuioCursor *tuioMultiplexerCursor:: gethighestTuioCursor(){
TuioCursor tcur;
int priority;//woher???
std::list<TuioCourser *>::iterator = it;
for(it= cursorList.begin();it!= cursorList.end();it++){

}
return highestCursor
}
*/
/*
float tuioMultiplexerCursor::getX() const{

			
			ofxVec3f vec;
			vec.x = (cursor->getX()* source->screen.width)-(source->screen.width/2.0);
			vec.y = (cursor->getY()* source->screen.height)-(source->screen.height/2.0);
			vec.rotate( source->rotation.z,  ofxVec3f(0,0,1) );
			vec.x += source->translation.x;
			
			return vec.x;//xpos;


}
float tuioMultiplexerCursor::getY() const{
			ofxVec3f vec;
			vec.x = (cursor->getX()* source->screen.width)-(source->screen.width/2.0);
			vec.y = (cursor->getY()* source->screen.height)-(source->screen.height/2.0);
			vec.rotate( source->rotation.z,  ofxVec3f(0,0,1) );
			vec.y += source->translation.y;
			//vec.y = vec.y / screen->height;
			
			return vec.y;//xpos;


}
*/

void tuioMultiplexerCursor::addTuioCursor( TUIO::TuioCursor * cursor){

	cout << "tuioMultiplexerCursor.addTuioCursor()---------:"<<cursor->getCursorID() << endl;
	cursorList.push_back(cursor);
}

void tuioMultiplexerCursor::removeTuioCursor( TUIO::TuioCursor * cursor){

	cursorList.remove(cursor);
}

