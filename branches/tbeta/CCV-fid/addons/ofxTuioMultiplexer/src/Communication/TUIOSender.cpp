/*
*  TUIO.h
*  
*
*  Created on 2/2/09.
*  Copyright 2009 NUI Group. All rights reserved.
*
*/

#include "TUIOSender.h"
#include "Poco/Environment.h"
#include "Poco/Net/NetworkInterface.h"
using Poco::Environment;
using namespace Poco::Net;


TUIOSender::TUIOSender() {
	
	 host_name = new char[64];
	 tuio_source_String = new char[256];
	 bOSCMode = 1;

	 setNetworkInterface(1);//default
}

TUIOSender::~TUIOSender() {

	if (tuio_source_String) delete[] tuio_source_String;
	if (host_name) delete[] host_name;
						
	// this could be useful for whenever we get rid of an object
}

void TUIOSender::setup( const char* host, int port, int flashport,int netinterfaceID, string tuio_application_name) {

	localHost = host;
	TUIOPort = port;
	TUIOFlashPort = flashport;
	frameseq = 0;

	setup_tuio_source_String(netinterfaceID, tuio_application_name);

	//FOR TCP
	bIsConnected = m_tcpServer.setup(TUIOFlashPort);
	//FOR OSC
	TUIOSocket.setup(localHost, TUIOPort);

	
	//camWidth= _camWidth;
	//camHeight= _camHeight;
	 
	 
}

void TUIOSender::setNetworkInterface(int id){
	 //copy the IPadress from POCO.Net.NetworkInterface to a char*
	int index = 0;

	if(NetworkInterface::list().size() > id ){ 
		index = id;
	}else if(NetworkInterface::list().size() >1 ){
		index = 1;
	}
	
	 strcpy(host_name,NetworkInterface::list()[index].address().toString().c_str( ));
}

void TUIOSender::setup_tuio_source_String(int id, string applicationName){

	setNetworkInterface( id);
	tuio_source_application = applicationName.c_str();
	//cout << "setup_tuio_source_String " << inet_ntoa(addr) << endl;
	// source message from TUIO 1.1 /tuio/[profileName] source [application@address]
	
	 sprintf(tuio_source_String,"%s@%s",tuio_source_application, host_name);
	
	 cout << "TUIO.SOURCE: "<< tuio_source_String << endl ;
}

void TUIOSender::sendTUIO(std::vector<Blob> &blobs, std::list<TUIO::TuioObject> &tobjList)
{
	
	frameseq += 1;

	if(bOSCMode){

		ofxOscBundle b;

		//added by Stefan Schlupek
		// source message from TUIO 1.1 /tuio/[profileName] source [application@address]

		ofxOscMessage source;
			source.setAddress( "/tuio/2Dcur" );
			source.addStringArg( "source" );	
			source.addStringArg(tuio_source_String);

		ofxOscMessage alive;
			alive.setAddress("/tuio/2Dcur");
			alive.addStringArg("alive");


		ofxOscMessage fseq;
			fseq.setAddress( "/tuio/2Dcur" );
			fseq.addStringArg( "fseq" );
			fseq.addIntArg(frameseq);

		if(blobs.size() == 0 )
		{
			//Sends alive message - saying 'Hey, there's no alive blobs'

			//Send fseq message
			

			b.addMessage( source ); //add message to bundle	
			b.addMessage( alive ); //add message to bundle
			b.addMessage( fseq ); //add message to bundle
			//TUIOSocket.sendBundle( b ); //send bundle
		}
		else //actually send the blobs
		{
				b.addMessage( source ); //add message to bundle			
				//cout << " CCV.source_name: " << tuio_source_String << "::::" << endl;
				//cout << " CCV.source_name: " << Environment::nodeId() << "::::" << endl;//physical Adress of the first networkadapter
				//cout << " CCV.IP: " << NetworkInterface::list()[1].address().toString() << "::::" << endl;
				//cout << " CCV.name: " << NetworkInterface::list()[1].name() << "::::" << endl;
				//cout << " CCV.displayName: " << NetworkInterface::list()[1].displayName() << "::::" << endl;							
				//----------------------

				std::vector<Blob>::iterator this_blob;
				for(this_blob = blobs.begin(); this_blob != blobs.end(); this_blob++)
				{
					//Set Message
					ofxOscMessage set;
					set.setAddress("/tuio/2Dcur");
					set.addStringArg("set");
					Blob currBlob = *this_blob;
					set.addIntArg(currBlob.id); //id
					set.addFloatArg(currBlob.centroid.x);  // x
					//printf("ofxOscMessage:centroid= %f:%f\n", currBlob.centroid.x, currBlob.centroid.y );
					//printf("Starting in Mini Mode...\n\n");
					set.addFloatArg(currBlob.centroid.y); // y
					set.addFloatArg(currBlob.D.x); //dX
					set.addFloatArg(currBlob.D.y); //dY
					set.addFloatArg(currBlob.maccel); //m
					printf("ofxOscMessage:currBlob.D= %f:%f:f%\n", currBlob.D.x, currBlob.D.y ,currBlob.maccel);
					b.addMessage( set ); //add message to bundle
					//Send alive message of all alive IDs
					alive.addIntArg(currBlob.id); //Send alive message of all alive IDs

				}//for			
				
				b.addMessage( alive ); //add message to bundle
				b.addMessage( fseq ); //add message to bundle		

			}//if


			TUIOSocket.sendBundle( b ); //send bundle
// BLOB-END----------------

			
//Fiducial: //----added by Stefan Schlupek

			ofxOscBundle b_obj;

			ofxOscMessage source_obj;
				source_obj.setAddress( "/tuio/2Dobj" );
				source_obj.addStringArg( "source" );
				source_obj.addStringArg(tuio_source_String);

			ofxOscMessage alive_obj;
				alive_obj.setAddress("/tuio/2Dobj");
				alive_obj.addStringArg("alive");

			ofxOscMessage fseq_obj;
				fseq_obj.setAddress( "/tuio/2Dobj" );
				fseq_obj.addStringArg( "fseq" );
				fseq_obj.addIntArg(frameseq);

			if( tobjList.size() == 0){

				//Sends alive message - saying 'Hey, there's no alive blobs'
				b_obj.addMessage( source_obj ); //add message to bundle	
				b_obj.addMessage( alive_obj ); //add message to bundle
				b_obj.addMessage( fseq_obj ); //add message to bundle
			
			}else		
				{				

				//printf("FIDUCIALS->SIZE: %i\n", fiducialsList->size() );
					b_obj.addMessage( source_obj ); //add message to bundle	
				
				list <TUIO::TuioObject>::iterator this_fiducial ;
				for(this_fiducial = tobjList.begin();this_fiducial != tobjList.end(); this_fiducial++){
					//printf("FIDUCIAL: %f\n", this_fiducial. );
					
					printf("FIDUCIAL: %i\n", this_fiducial->getSymbolID() );
					//printf("FIDUCIAL.X: %f\n", this_fiducial->getX() );
					//Set Message /tuio/2Dobj set s i x y a X Y A m r
									
					ofxOscMessage set;
					set.setAddress("/tuio/2Dobj");
					set.addStringArg("set");
					set.addIntArg(this_fiducial->getSessionID()); //s !!Where from????
					set.addIntArg(this_fiducial->getSymbolID()); //i
					set.addFloatArg(this_fiducial->getX());  // x Normalized
					set.addFloatArg(this_fiducial->getY()); // y Normalized
					set.addFloatArg(this_fiducial->getAngle()); //a
					set.addFloatArg(this_fiducial->getXSpeed()); //X
					set.addFloatArg(this_fiducial->getYSpeed()); //Y
					set.addFloatArg(this_fiducial->getRotationSpeed()); //A
					set.addFloatArg(this_fiducial->getMotionAccel()); //m
					set.addFloatArg(this_fiducial->getRotationAccel()); //r
					

					b_obj.addMessage( set ); //add message to bundle
					//Send alive message of all alive IDs : the Spezifacation  is getSessionID, but i look if the getSymbolID works
					alive_obj.addIntArg(this_fiducial->getSessionID());//??getSessionID?? //getSymbolID
				}//for

				b_obj.addMessage( alive_obj ); //add message to bundle
				b_obj.addMessage( fseq_obj ); //add message to bundle

				
			}//if				

		TUIOSocket.sendBundle( b_obj ); //send bundle

//--------Fiducial END--------------------------------


//---------End OSC Mode-------------------------------


	}else if(bTCPMode) //else, if TCP (flash) mode
	{
		if(blobs.size() == 0){

			m_tcpServer.sendToAll("<OSCPACKET ADDRESS=\"127.0.0.1\" PORT=\""+ofToString(TUIOPort)+"\" TIME=\""+ofToString(ofGetElapsedTimef())+"\">" +
							 "<MESSAGE NAME=\"/tuio/2Dcur\">"+
							 "<ARGUMENT TYPE=\"s\" VALUE=\"alive\"/>"+
							 "</MESSAGE>"+
							 "<MESSAGE NAME=\"/tuio/2Dcur\">"+
							 "<ARGUMENT TYPE=\"s\" VALUE=\"fseq\"/>"+
							 "<ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(frameseq)+"\"/>" +
							 "</MESSAGE>"+
							 "</OSCPACKET>");
		}
		else
		{
			string setBlobsMsg;

			std::vector<Blob>::iterator this_blob;
			for(this_blob = blobs.begin(); this_blob != blobs.end(); this_blob++)
			{
				//if sending height and width
				Blob currBlob = *this_blob;
				if(bHeightWidth){
					setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(currBlob.id)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.centroid.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.centroid.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.D.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.D.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.maccel)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.boundingRect.width)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.boundingRect.height)+"\"/>"+
					"</MESSAGE>";
				}
				else{
					setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(currBlob.id)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.centroid.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.centroid.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.D.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.D.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(currBlob.maccel)+"\"/>"+
					"</MESSAGE>";
				}
			}

			string aliveBeginMsg = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"alive\"/>";

			string aliveBlobsMsg;
			std::vector<Blob>::iterator this_blobID;
			for(this_blobID = blobs.begin(); this_blobID != blobs.end(); this_blobID++)
			{
				aliveBlobsMsg += "<ARGUMENT TYPE=\"i\" VALUE=\""+ofToString((*this_blobID).id)+"\"/>";
			}

			string aliveEndMsg = "</MESSAGE>";

			string fseq = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"fseq\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(frameseq)+"\"/></MESSAGE>";

			m_tcpServer.sendToAll("<OSCPACKET ADDRESS=\"127.0.0.1\" PORT=\""+ofToString(TUIOPort)+"\" TIME=\""+ofToString(ofGetElapsedTimef())+"\">" +
									setBlobsMsg + aliveBeginMsg + aliveBlobsMsg + aliveEndMsg + fseq + "</OSCPACKET>");
		}
	}
}
