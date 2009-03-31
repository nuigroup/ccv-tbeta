/*
 *  BlobManager.h
 *  tbeta
 *
 *  Created by Artem Titoulenko on 2/2/09.
 *  Copyright 2009 NUI Inc.. All rights reserved.
 *
 */

#ifndef BLOBMANAGER_H
#define BLOBMANAGER_H

#include "../ofxNCore.h"

class BlobManager
	{
	public:
		BlobManager();
		~BlobManager();
		
		map<int, ofxTBetaCvBlob> blobs;
	};

#endif
