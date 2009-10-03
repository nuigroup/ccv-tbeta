#include "ofxDSVL.h"

ofxDSVL::ofxDSVL()
{
	//touchlib
	dsvl_vs = NULL;
    buffer = NULL;
	pbuffer = NULL;

    g_Timestamp = _INVALID_TIMESTAMP;

    cap_width = 0;
    cap_height = 0;
    cap_fps = 0.0;

	colour = false;
	newFrame = false;

	bytes = 0;
}

void ofxDSVL::setDeviceID(int id)
{
}

unsigned char * ofxDSVL::getPixels()
{
	return pbuffer;
}

int ofxDSVL::getDeviceID()
{
    return 0;
}

void ofxDSVL::initDSVL()
{
    CoInitialize(NULL);
    dsvl_vs = new DSVL_VideoSource();

    if(FAILED(dsvl_vs->BuildGraphFromXMLFile("DSVL_config.xml"))){return;}

	if(FAILED(dsvl_vs->GetCurrentMediaFormat(&cap_width,&cap_height,&cap_fps,NULL))){return;}

    PIXELFORMAT pxf;
    if(FAILED(dsvl_vs->GetCurrentMediaFormat(&cap_width,&cap_height,&cap_fps,&pxf))){return;}

	//choose byte size. grayscale by default
	bytes = 1;   
    switch (pxf)
    {	//only handle the two most important cases here
        case PIXELFORMAT_RGB24:
            bytes = 3;
            printf("[INFO] current pixel format is RGB24\n");
            break;

        case PIXELFORMAT_RGB32:
            bytes = 4;
            printf("[INFO] current pixel format is RGB32\n");
            break;
	}

    if(FAILED(dsvl_vs->EnableMemoryBuffer(3))){return;}
    // three concurrent threads will concurrently query for samples
	if(FAILED(dsvl_vs->Run())){return;}

	camWidth  = cap_width;
	camHeight = cap_height; 
	fps		  = (int)cap_fps;

	pbuffer = new unsigned char[camWidth*camHeight*bytes];
}

bool ofxDSVL::isFrameNew()
{
	//DWORD wait_result = dsvl_vs->WaitForNextSample(/*INFINITE*/1000/fps);
		//if(wait_result == WAIT_OBJECT_0)
/*		{
			unsigned char r,g,b;
			int dwidth = 2*camWidth;

			dsvl_vs->CheckoutMemoryBuffer(&g_mbHandle, &buffer);
			g_Timestamp = dsvl_vs->GetCurrentTimestamp();
			switch (colour) {
				case false: {
					unsigned char *src = (unsigned char*)buffer;
					unsigned char *dest = pbuffer;
					dest+=camWidth*camHeight-camWidth;

					for(int y=0;y<camHeight;y++) {
						for(int x=0;x<camWidth;x++) {
							//fastest and best
							r = *src++;
							g = *src++;
							b = *src++;
							*dest++ = hibyte(r * 77 + g * 151 + b * 28);
						}
						dest-=dwidth;
					}
					break;
				}
				case true: {
					memcpy(pbuffer,buffer,camWidth*camHeight*bytes);
					break;
				}
			}
			dsvl_vs->CheckinMemoryBuffer(g_mbHandle);
			
			return true;
		}

		return false;
*/

	
	//taken from reactivision source (dslibcamera.cpp), magic values?, better strategy?
	DWORD wait_result = dsvl_vs->WaitForNextSample(/*INFINITE*/ (long)(1000.0f/fps));
	{
		if (SUCCEEDED(dsvl_vs->CheckoutMemoryBuffer(&g_mbHandle, &buffer)))
		{
			g_Timestamp = dsvl_vs->GetCurrentTimestamp();
			//prepare array
			memcpy(pbuffer, buffer, camWidth*camHeight*bytes);
			dsvl_vs->CheckinMemoryBuffer(g_mbHandle);
			buffer = 0;

			return true;
		}
		else
		{
			printf("[WARNING] could NOT checkout membuffer (get sample)\n");
		}
	}

	return false;
}

int ofxDSVL::getCamWidth()
{
    return camWidth;
}

int ofxDSVL::getCamHeight()
{
    return camHeight;
}

int ofxDSVL::getNumByes()
{
	return bytes;
}

void ofxDSVL::showSettingsDialog() {

	dsvl_vs->ShowFilterProperties();
}

//Clean up
ofxDSVL::~ofxDSVL()
{
    dsvl_vs->Stop();
    dsvl_vs->ReleaseGraph();
	if (pbuffer!=NULL) delete []pbuffer;
    delete dsvl_vs;
}
