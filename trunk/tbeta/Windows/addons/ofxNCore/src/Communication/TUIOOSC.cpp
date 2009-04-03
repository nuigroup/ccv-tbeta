#include "TUIOOSC.h"

TUIOOSC::TUIOOSC() {

	//m_tcpServer.setup(3000);
}

TUIOOSC::~TUIOOSC() {
	
	// this could be useful for whenever we get rid of an object
}

void TUIOOSC::setup(const char* host, int port) {

	localHost = host;
	TUIOPort = port;
	frameseq = 0;

	//FOR TCP
	m_tcpServer.setup(TUIOPort);
	printf("TUIOPort is: %i \n", TUIOPort);

	//FOR OSC
	//TUIOSocket.setup(localHost, TUIOPort);
}

void TUIOOSC::update() {

	frameseq += 1;
}

void TUIOOSC::sendOSC()
{	

	if(blobs.size() == 0){

		cout << blobs.size() << endl;

		m_tcpServer.sendToAll("<OSCPACKET ADDRESS=\"127.0.0.1\" PORT=\""+ofToString(TUIOPort)+"\" TIME=\"1\">" +   
						 "<MESSAGE NAME=\"/tuio/2Dcur\">"+ 
						 "<ARGUMENT TYPE=\"s\" VALUE=\"alive\"/>"+
						 "</MESSAGE>"+
						 "<MESSAGE NAME=\"/tuio/2Dcur\">"+ 
						 "<ARGUMENT TYPE=\"s\" VALUE=\"fseq\"/>"+
						 "<ARGUMENT TYPE=\"i\" VALUE=\"0\"/>" +
						 "</MESSAGE>"+
						 "</OSCPACKET>");
	}
	else //actually send the blobs
	{
		string setBlobsMsg;

		map<int, ofxTBetaCvBlob>::iterator this_blob;
		for(this_blob = blobs.begin(); this_blob != blobs.end(); this_blob++) 
		{		
			setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blob->second.id)+"\"/>"+
				"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.x)+"\"/>"+
				"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.centroid.y)+"\"/>"+
				"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.x)+"\"/>"+
				"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.D.y)+"\"/>"+
				"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.maccel)+"\"/>"+
				"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.boundingRect.width)+"\"/>"+
				"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(this_blob->second.boundingRect.height)+"\"/>"+
				"</MESSAGE>";
		}

		string aliveBeginMsg = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"alive\"/>";
		
		string aliveBlobsMsg;
		std::map<int, ofxTBetaCvBlob>::iterator this_blobID;
		for(this_blobID = blobs.begin(); this_blobID != blobs.end(); this_blobID++)
		{
			aliveBlobsMsg += "<ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blobID->second.id)+"\"/>";
		}						  
						  
		string aliveEndMsg = "</MESSAGE>";

		string fseq = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"fseq\"/><ARGUMENT TYPE=\"i\" VALUE=\"0\"/></MESSAGE>";


		m_tcpServer.sendToAll("<OSCPACKET ADDRESS=\"127.0.0.1\" PORT=\""+ofToString(TUIOPort)+"\" TIME=\"1\">" + 
								setBlobsMsg + aliveBeginMsg + aliveBlobsMsg + aliveEndMsg + fseq + "</OSCPACKET>");
	
	

/*		if(OSC_MODE){

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
*/

	}
}