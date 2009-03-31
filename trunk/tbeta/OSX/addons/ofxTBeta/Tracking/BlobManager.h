#ifndef BLOBMANAGER_H
#define BLOBMANAGER_H
/*
 *  BlobManager.h
 *  tbeta
 *
 *  Created by Artem Titoulenko on 2/2/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */

#include "../ofxTBeta.h"

class BlobManager
	{
	public:
		BlobManager();
		~BlobManager();
		
		map<int, ofxTBetaCvBlob> blobs;
	};

#endif
