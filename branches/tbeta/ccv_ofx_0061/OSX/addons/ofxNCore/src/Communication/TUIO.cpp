/*
*  TUIO.h
*  
*
*  Created on 2/2/09.
*  Copyright 2009 NUI Group. All rights reserved.
*
*/

#include "TUIO.h"

TUIO::TUIO() 
{
}

TUIO::~TUIO() 
{
	// this could be useful for whenever we get rid of an object
}

void TUIO::setup(const char* host, int port, int flashport) 
{
	localHost = host;
	TUIOPort = port;
	TUIOFlashPort = flashport;
	frameseq = 0;

	//FOR TCP
	bIsConnected = m_tcpServer.setup(TUIOFlashPort);
	//FOR OSC
	TUIOSocket.setup(localHost, TUIOPort);
}

void TUIO::sendTUIO(std::map<int, Blob> * blobs)
{
	frameseq += 1;

	// if sending OSC (not TCP)
	if(bOSCMode)
	{
		ofxOscBundle b;
		ofxOscMessage alive;
		// Sends alive message - saying 'Hey, there's no alive blobs'
		alive.setAddress("/tuio/2Dcur");
		alive.addStringArg("alive");

		// Send fseq message
		ofxOscMessage fseq;
		fseq.setAddress( "/tuio/2Dcur" );
		fseq.addStringArg( "fseq" );
		fseq.addIntArg(frameseq);

		if(blobs->size() == 0)
		{
			b.addMessage( alive );		// add message to bundle
			b.addMessage( fseq );		// add message to bundle
			TUIOSocket.sendBundle( b ); // send bundle
		}
		else // actually send the blobs
		{
			map<int, Blob>::iterator blob;
			for(blob = blobs->begin(); blob != blobs->end(); blob++)
			{
				// omit point (0,0) since this means that we are outside of the range
				if(blob->second.centroid.x == 0 && blob->second.centroid.y == 0)
					continue;

				//Set Message
				ofxOscMessage set;
				set.setAddress( "/tuio/2Dcur" );
				set.addStringArg("set");
				set.addIntArg(blob->second.id);				// id
				set.addFloatArg(blob->second.centroid.x);	// x
				set.addFloatArg(blob->second.centroid.y);	// y
				set.addFloatArg(blob->second.D.x);			// dX
				set.addFloatArg(blob->second.D.y);			// dY
				set.addFloatArg(blob->second.maccel);		// m
				if(bHeightWidth)
				{
					set.addFloatArg(blob->second.boundingRect.width);	// wd
					set.addFloatArg(blob->second.boundingRect.height);	// ht
				}
				b.addMessage( set );							// add message to bundle
				alive.addIntArg(blob->second.id);				// add blob to list of ALL active IDs
			}

// 			// Send alive message of all alive IDs
// 			ofxOscMessage alive;
// 			alive.setAddress("/tuio/2Dcur");
// 			alive.addStringArg("alive");

//			std::map<int, Blob>::iterator this_blobID;
//			for(this_blobID = blobs->begin(); this_blobID != blobs->end(); this_blobID++)
//			{
//				alive.addIntArg(this_blobID->second.id); //Get list of ALL active IDs
//			}

			//Send fseq message
//			ofxOscMessage fseq;
//			fseq.setAddress( "/tuio/2Dcur" );
//			fseq.addStringArg( "fseq" );
//			fseq.addIntArg(frameseq);

			b.addMessage( alive );		//add message to bundle
			b.addMessage( fseq );		//add message to bundle
			TUIOSocket.sendBundle( b ); //send bundle
		}
	}
	else if(bTCPMode) // else, if TCP (flash) mode
	{
		if(blobs->size() == 0)
		{
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
			string aliveBeginMsg = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"alive\"/>";
			string aliveEndMsg = "</MESSAGE>";
			string aliveBlobsMsg;

			map<int, Blob>::iterator blob;
			for(blob = blobs->begin(); blob != blobs->end(); blob++)
			{
				// omit point (0,0) since this means that we are outside of the range
				if(blob->second.centroid.x == 0 && blob->second.centroid.y == 0)
					continue;

				// if sending height and width
				if(bHeightWidth)
				{
					setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(blob->second.id)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.centroid.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.centroid.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.D.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.D.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.maccel)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.boundingRect.width)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.boundingRect.height)+"\"/>"+
					"</MESSAGE>";
				}
				else
				{
					setBlobsMsg += "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"set\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(blob->second.id)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.centroid.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.centroid.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.D.x)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.D.y)+"\"/>"+
					"<ARGUMENT TYPE=\"f\" VALUE=\""+ofToString(blob->second.maccel)+"\"/>"+
					"</MESSAGE>";
				}
				aliveBlobsMsg += "<ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(blob->second.id)+"\"/>";
			}

// 			std::map<int, Blob>::iterator this_blobID;
// 			for(this_blobID = blobs->begin(); this_blobID != blobs->end(); this_blobID++)
// 			{
// 				aliveBlobsMsg += "<ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(this_blobID->second.id)+"\"/>";
// 			}

			string fseq = "<MESSAGE NAME=\"/tuio/2Dcur\"><ARGUMENT TYPE=\"s\" VALUE=\"fseq\"/><ARGUMENT TYPE=\"i\" VALUE=\""+ofToString(frameseq) + "\"/></MESSAGE>";
			m_tcpServer.sendToAll("<OSCPACKET ADDRESS=\"127.0.0.1\" PORT=\"" + 
									ofToString(TUIOPort) + "\" TIME=\""+ofToString(ofGetElapsedTimef()) + "\">" +
									setBlobsMsg + aliveBeginMsg + aliveBlobsMsg + aliveEndMsg + fseq + "</OSCPACKET>");
		}
	}
	else if(bBinaryMode) // else, if TCP (binary) mode
	{
		uchar buf[1024*8];
		uchar *p = buf;
		// Add "CCV" as a data header
		*p++ = 'C';	*p++ = 'C';	*p++ = 'V';	*p++ = '\0';
		if(blobs->size() == 0)
		{
			memset(p, 0, 4);	p += 4;
		}
		else
		{
			int count = 0;
			map<int, Blob>::iterator blob;
			// count the blobs that are non (0,0)
			for(blob = blobs->begin(); blob != blobs->end(); blob++)
			{
				// omit point (0,0) since this means that we are outside of the range
				if(blob->second.centroid.x == 0 && blob->second.centroid.y == 0)
					continue;
				count++;
			}
			// send blob count first
			memcpy(p, &count, 4);	p += 4;
			// send blob information
			for(blob = blobs->begin(); blob != blobs->end(); blob++)
			{
				// omit point (0,0) since this means that we are outside of the range
				if(blob->second.centroid.x == 0 && blob->second.centroid.y == 0)
					continue;
				memcpy(p, &blob->second.id, 4);							p += 4;
				memcpy(p, &blob->second.centroid.x, 4);					p += 4;
				memcpy(p, &blob->second.centroid.y, 4);					p += 4;
				memcpy(p, &blob->second.D.x, 4);						p += 4;
				memcpy(p, &blob->second.D.y, 4);						p += 4;
				memcpy(p, &blob->second.maccel, 4);						p += 4;
				if(bHeightWidth)
				{
					memcpy(p, &blob->second.boundingRect.width, 4);		p += 4;
					memcpy(p, &blob->second.boundingRect.height, 4);	p += 4;
				}
			}
		}
		// send blob data to clients
		m_tcpServer.sendRawBytesToAll((const char*)buf, p-buf);
	}
}
