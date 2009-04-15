/*  portVideo, a cross platform camera framework
    Copyright (C) 2005-2008 Martin Kaltenbrunner <mkalten@iua.upf.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef CAMERAENGINE_H
#define CAMERAENGINE_H

#include <fstream>
#include <iostream>
#include "tinyxml.h"
#include <math.h>
#include <SDL.h>
#include "FontTool.h"

#ifdef __APPLE__
#include <CoreFoundation/CFBundle.h>
#endif

#define SAT(c) \
        if (c & (~255)) { if (c < 0) c = 0; else c = 255; }
#define SETTING_DEFAULT -1
#define SETTING_AUTO -2
#define SETTING_MIN -3
#define SETTING_MAX -4

class CameraEngine
{
public:
	CameraEngine(const char* cfg) { 
		config_file = cfg;
		settingsDialog=false;
	}
	
	virtual ~CameraEngine() {};
	
	virtual bool findCamera() = 0;
	virtual bool initCamera() = 0;
	virtual bool startCamera() = 0;
	virtual unsigned char* getFrame() = 0;
	virtual bool stopCamera() = 0;
	virtual bool resetCamera() = 0;
	virtual bool closeCamera() = 0;	
	virtual bool stillRunning() = 0;

	enum CameraSetting { BRIGHTNESS, GAIN, SHUTTER, EXPOSURE, SHARPNESS, FOCUS, GAMMA };	

	virtual int getCameraSettingStep(int mode) = 0;
	virtual int getMinCameraSetting(int mode) = 0;
	virtual int getMaxCameraSetting(int mode) = 0;
	virtual int getCameraSetting(int mode) = 0;
	virtual bool setCameraSetting(int mode, int value) = 0;
	virtual bool setCameraSettingAuto(int mode) = 0;
	
	void applyCameraSettings();
	void applyCameraSetting(int mode, int value);

	virtual void showSettingsDialog();	
	virtual void control(int key);
	void drawGUI(SDL_Surface *display);
	void readSettings();
	void saveSettings();

	int getFps() { return (int)floor(fps+0.5f); }
	int getWidth() { return width; }
	int getHeight() { return height; }
	char* getName() { return cameraName; }
	
protected:
	int cameraID;
	char cameraName[256];
	
	struct portvideo_settings {
			int device;
			char file[255];
			char folder[255];
			bool color;
			bool format7;
			int width;
			int height;
			float fps;
			int xoff;
			int yoff;
			int brightness;
			int gain;
			int shutter;
			int exposure;
			int sharpness;
			int focus;
			int gamma;
	};
	portvideo_settings config;
	const char* config_file;
	char* image_file;
	#ifdef __APPLE__
	char full_path[1024];
	#endif
		
	unsigned char* buffer;
	int bytes;
	
	int width;
	int height;
	float fps;
	bool colour;

	int timeout;
	int lost_frames;
	
	bool running;
	bool settingsDialog;
	int currentCameraSetting;
	
	void uyvy2gray(int width, int height, unsigned char *src, unsigned char *dest);
	void yuyv2gray(int width, int height, unsigned char *src, unsigned char *dest);
	void uyvy2rgb(int width, int height, unsigned char *src, unsigned char *dest);
};
#endif
