#include "TUIOOSC.h"

TUIOOSC::TUIOOSC() {
	printf("TUIO created: \n");
}

TUIOOSC::~TUIOOSC() {
	// this could be useful for whenever we get rid of an object
	printf("tuio killed: \n");
}

void TUIOOSC::setup(const char* host, int port) {

	printf("TUIO setup: \n");

	localHost = host;
	TUIOPort = port;

	TUIOSocket.setup(localHost, TUIOPort); 
	
	frameseq = 0;
}

void TUIOOSC::update() {

	frameseq += 1;
}

void TUIOOSC::sendOSC()
{	
	ofxOscBundle b;

	if(blobs.size() == 0)
	{
		//Sends alive message - saying 'Hey, there's no alive blobs'
		ofxOscMessage alive;
		alive.setAddress("/tuio/2Dcur");		
		alive.addStringArg("alive");

		//Send fseq message
		ofxOscMessage fseq;
		fseq.setAddress( "/tuio/2Dcur" );		
		fseq.addStringArg( "fseq" );
		fseq.addIntArg(frameseq);
		
		b.addMessage( alive ); //add message to bundle
		b.addMessage( fseq ); //add message to bundle
		TUIOSocket.sendBundle( b ); //send bundle
	}
	else //actually send the blobs
	{
		map<int, ofxTBetaCvBlob>::iterator this_blob;
		for(this_blob = blobs.begin(); this_blob != blobs.end(); this_blob++) 
		{		
			//Set Message
			ofxOscMessage set;
			set.setAddress("/tuio/2Dcur");
			set.addStringArg("set");
			set.addIntArg(this_blob->second.id); //id
			set.addFloatArg(this_blob->second.centroid.x);  // x
			set.addFloatArg(this_blob->second.centroid.y); // y 
			set.addFloatArg(this_blob->second.D.x); //dX
			set.addFloatArg(this_blob->second.D.y); //dY
			set.addFloatArg(this_blob->second.maccel); //m	
			set.addFloatArg(this_blob->second.boundingRect.width); // wd
			set.addFloatArg(this_blob->second.boundingRect.height);// ht
			b.addMessage( set ); //add message to bundle
		}

		//Send alive message of all alive IDs
		ofxOscMessage alive;
		alive.setAddress("/tuio/2Dcur");		
		alive.addStringArg("alive");

		std::map<int, ofxTBetaCvBlob>::iterator this_blobID;
		for(this_blobID = blobs.begin(); this_blobID != blobs.end(); this_blobID++)
		{
			alive.addIntArg(this_blobID->second.id); //Get list of ALL active IDs
		}

		//Send fseq message
		ofxOscMessage fseq;
		fseq.setAddress( "/tuio/2Dcur" );		
		fseq.addStringArg( "fseq" );
		fseq.addIntArg(frameseq);
		
		b.addMessage( alive ); //add message to bundle
		b.addMessage( fseq ); //add message to bundle
		
		TUIOSocket.sendBundle( b ); //send bundle
		
	}
}