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

#include "CameraEngine.h"
#include "PortVideoSDL.h"

	void CameraEngine::showSettingsDialog() {
		if (settingsDialog) {
			settingsDialog = false;
			saveSettings();
			PortVideoSDL::display_lock = false;
		} else if (!PortVideoSDL::display_lock) {
			currentCameraSetting = BRIGHTNESS;
			settingsDialog = true;
			PortVideoSDL::display_lock = true;
		}
	}
	
	void CameraEngine::control(int key) {
		if(!settingsDialog) return;
		int value,min,max,step = 0;
		switch(key) {
			case SDLK_LEFT:
				value = getCameraSetting(currentCameraSetting);
				min = getMinCameraSetting(currentCameraSetting);
				max = getMaxCameraSetting(currentCameraSetting);
				step = getCameraSettingStep(currentCameraSetting);
				if (step==1) step = (int)((float)max/256.0f);
				if (step<1) step=1;
				value -= step;
				if (value<min) value=min;
				setCameraSetting(currentCameraSetting,value);
				break;
			case SDLK_RIGHT:
				value = getCameraSetting(currentCameraSetting);
				min = getMinCameraSetting(currentCameraSetting);
				max = getMaxCameraSetting(currentCameraSetting);
				step = getCameraSettingStep(currentCameraSetting);
				if (step==1) step = (int)((float)max/256.0f);
				if (step<1) step=1;
				value += step;
				if (value>max) value=max;
				setCameraSetting(currentCameraSetting,value);
				break;
			case SDLK_UP:
				currentCameraSetting--;
				if(currentCameraSetting<0) currentCameraSetting=GAMMA;
				break;
			case SDLK_DOWN:
				currentCameraSetting++;
				if(currentCameraSetting>GAMMA) currentCameraSetting=0;
				break;
		}
		
	}

	void CameraEngine::drawGUI(SDL_Surface *display) {
		if(!settingsDialog) return;
		unsigned char* disp = (unsigned char*)(display->pixels);
		
		int settingValue = getCameraSetting(currentCameraSetting);
		int maxValue = getMaxCameraSetting(currentCameraSetting);
		int minValue = getMinCameraSetting(currentCameraSetting);
		int valueRange = maxValue - minValue;

		const char *settingText = NULL;
		switch (currentCameraSetting) {
			case BRIGHTNESS:	settingText = "Brightness"; break;
			case GAIN:		settingText = "Gain"; break;
			case SHUTTER:		settingText = "Shutter"; break;
			case EXPOSURE:		settingText = "Exposure"; break;
			case SHARPNESS:		settingText = "Sharpness"; break;
			case FOCUS:		settingText = "Focus"; break;
			case GAMMA:		settingText = "Gamma"; break;
		}
		
		int x_offset=width/2-128;
		int y_offset=height-100;

		char displayText[256];
		sprintf(displayText,"%s %d",settingText,settingValue);
		FontTool::drawText(x_offset+128-(FontTool::getTextWidth(displayText)/2),y_offset-FontTool::getFontHeight(),displayText,display);
		
		// draw the border
		for (int i=x_offset;i<(x_offset+256);i++) {
			int pixel=(width*y_offset+i)*3+1;
			disp[pixel]=255;
			pixel=(width*(y_offset+25)+i)*3+1;
			disp[pixel]=255;
		}
		for (int i=y_offset;i<(y_offset+25);i++) {
			int pixel=(width*i+x_offset)*3+1;
			disp[pixel]=255;
			pixel=(width*i+x_offset+256)*3+1;
			disp[pixel]=255;
		}

		// draw the bar
		int xpos = (int)(254*(((float)settingValue-(float)minValue)/(float)valueRange));
		for (int i=x_offset+2;i<=(x_offset+xpos);i++) {
			for (int j=y_offset+2;j<=(y_offset+23);j++) {
				int pixel=(width*j+i)*3+1;
				disp[pixel]=255;
			}
		}


	}

	void CameraEngine::readSettings() {

		config.device = 0;
		config.color = false;
		config.width = SETTING_MAX;
		config.height = SETTING_MAX;
		config.fps = SETTING_MAX;
		config.format7 = false;
		config.xoff = 0;
		config.yoff = 0;
		sprintf(config.file,"none");		
		sprintf(config.folder,"none");		
		config.brightness = SETTING_DEFAULT;
		config.gain = SETTING_DEFAULT;
		config.gamma = SETTING_DEFAULT;
		config.exposure = SETTING_DEFAULT;
		config.sharpness = SETTING_DEFAULT;
		config.shutter = SETTING_DEFAULT;

#ifdef __APPLE__
		char path[1024];
#endif		
		
		if (strcmp( config_file, "none" ) == 0) {
#ifdef __APPLE__
			CFBundleRef mainBundle = CFBundleGetMainBundle();
			CFURLRef mainBundleURL = CFBundleCopyBundleURL( mainBundle);
			CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
			CFStringGetCString( cfStringRef, path, 1024, kCFStringEncodingASCII);	
			CFRelease( mainBundleURL);
			CFRelease( cfStringRef);
			sprintf(full_path,"%s/Contents/Resources/camera.xml",path);
			config_file = full_path;
#else
			config_file = "./camera.xml";
#endif
		}

		TiXml::TiXmlDocument xml_settings( config_file );
		xml_settings.LoadFile();
		if( xml_settings.Error() )
		{
			std::cout << "Error loading camera configuration file: " << config_file << std::endl;
			return;
		}

		TiXml::TiXmlHandle docHandle( &xml_settings );
		TiXml::TiXmlHandle camera = docHandle.FirstChild("portvideo").FirstChild("camera");
		TiXml::TiXmlElement* camera_element = camera.Element();


		if( camera_element==NULL )
		{
			std::cout << "Error loading camera configuration file: " << config_file << std::endl;
			return;
		}
	
		if(camera_element->Attribute("id")!=NULL) config.device = atoi(camera_element->Attribute("id"));
		if(camera_element->Attribute("file")!=NULL) {
#ifdef __APPLE__
			sprintf(config.file,"%s/../%s",path,camera_element->Attribute("file"));
#else
			sprintf(config.file,"%s",camera_element->Attribute("file"));
#endif
		}
		if(camera_element->Attribute("folder")!=NULL) {
#ifdef __APPLE__
			sprintf(config.folder,"%s/../%s",path,camera_element->Attribute("folder"));
#else
			sprintf(config.folder,"%s",camera_element->Attribute("folder"));
#endif
		}

		
		TiXml::TiXmlElement* image_element = camera.FirstChild("image").Element();
	
		if (image_element!=NULL) {
			if ((image_element->Attribute("color")!=NULL) && ( strcmp( image_element->Attribute("color"), "true" ) == 0 )) config.color = true;

			if(image_element->Attribute("width")!=NULL) {
				if (strcmp( image_element->Attribute("width"), "max" ) == 0) config.width = SETTING_MAX;
				else config.width = atoi(image_element->Attribute("width"));
			}

			if(image_element->Attribute("height")!=NULL) {
				if (strcmp( image_element->Attribute("height"), "max" ) == 0) config.height = SETTING_MAX;
				else config.height = atoi(image_element->Attribute("height"));
			}
			if(image_element->Attribute("fps")!=NULL) {
				if (strcmp( image_element->Attribute("fps"), "max" ) == 0) config.fps = SETTING_MAX;
				else config.fps = atof(image_element->Attribute("fps"));
			}
		}

		TiXml::TiXmlElement* format7_element = camera.FirstChild("format7").Element();
		if (format7_element!=NULL) {
			config.format7 = true;

			if(format7_element->Attribute("xoff")!=NULL) {
				if (strcmp( format7_element->Attribute("xoff"), "max" ) == 0) config.xoff = SETTING_MAX;
				else if (strcmp( format7_element->Attribute("xoff"), "min" ) == 0) config.xoff = 0;
				else config.xoff = atoi(format7_element->Attribute("xoff"));
			}

			if(format7_element->Attribute("yoff")!=NULL) {
				if (strcmp( format7_element->Attribute("yoff"), "max" ) == 0) config.yoff = SETTING_MAX;
				else if (strcmp( format7_element->Attribute("yoff"), "min" ) == 0) config.yoff = 0;
				else config.yoff = atoi(format7_element->Attribute("yoff"));
			}
		}
	
		TiXml::TiXmlElement* settings_element = camera.FirstChild("settings").Element();
		if (settings_element!=NULL) {
			if(settings_element->Attribute("brightness")!=NULL) {
				if (strcmp(settings_element->Attribute("brightness"), "max" ) == 0) config.brightness=SETTING_MAX;
				else if (strcmp(settings_element->Attribute("brightness"), "min" ) == 0) config.brightness=SETTING_MIN;
				else if (strcmp(settings_element->Attribute("brightness"), "auto" ) == 0) config.brightness=SETTING_AUTO;
				else if (strcmp(settings_element->Attribute("brightness"), "default" ) == 0) config.brightness=SETTING_DEFAULT;
				else config.brightness = atoi(settings_element->Attribute("brightness"));
			}
			if(settings_element->Attribute("gain")!=NULL) {
				if (strcmp(settings_element->Attribute("gain"), "max" ) == 0) config.gain=SETTING_MAX;
				else if (strcmp(settings_element->Attribute("gain"), "min" ) == 0) config.gain=SETTING_MIN;
				else if (strcmp(settings_element->Attribute("gain"), "auto" ) == 0) config.gain=SETTING_AUTO;
				else if (strcmp(settings_element->Attribute("gain"), "default" ) == 0) config.gain=SETTING_DEFAULT;
				else config.gain = atoi(settings_element->Attribute("gain"));
			}
			if(settings_element->Attribute("gamma")!=NULL) {
				if (strcmp(settings_element->Attribute("gamma"), "max" ) == 0) config.gamma=SETTING_MAX;
				else if (strcmp(settings_element->Attribute("gamma"), "min" ) == 0) config.gamma=SETTING_MIN;
				else if (strcmp(settings_element->Attribute("gamma"), "auto" ) == 0) config.gamma=SETTING_AUTO;
				else if (strcmp(settings_element->Attribute("gamma"), "default" ) == 0) config.gamma=SETTING_DEFAULT;
				else config.gamma = atoi(settings_element->Attribute("gamma"));
			}
			if(settings_element->Attribute("shutter")!=NULL) {
				if (strcmp(settings_element->Attribute("shutter"), "max" ) == 0) config.shutter=SETTING_MAX;
				else if (strcmp(settings_element->Attribute("shutter"), "min" ) == 0) config.shutter=SETTING_MIN;
				else if (strcmp(settings_element->Attribute("shutter"), "auto" ) == 0) config.shutter=SETTING_AUTO;
				else if (strcmp(settings_element->Attribute("shutter"), "default" ) == 0) config.shutter=SETTING_DEFAULT;
				else config.shutter = atoi(settings_element->Attribute("shutter"));
			}
			if(settings_element->Attribute("exposure")!=NULL) {
				if (strcmp(settings_element->Attribute("exposure"), "max" ) == 0) config.exposure=SETTING_MAX;
				else if (strcmp(settings_element->Attribute("exposure"), "min" ) == 0) config.exposure=SETTING_MIN;
				else if (strcmp(settings_element->Attribute("exposure"), "auto" ) == 0) config.exposure=SETTING_AUTO;
				else if (strcmp(settings_element->Attribute("exposure"), "default" ) == 0) config.exposure=SETTING_DEFAULT;
				else config.exposure = atoi(settings_element->Attribute("exposure"));
			}
			if(settings_element->Attribute("sharpness")!=NULL) {
				if (strcmp(settings_element->Attribute("sharpness"), "max" ) == 0) config.sharpness=SETTING_MAX;
				else if (strcmp(settings_element->Attribute("sharpness"), "min" ) == 0) config.sharpness=SETTING_MIN;
				else if (strcmp(settings_element->Attribute("sharpness"), "auto" ) == 0) config.sharpness=SETTING_AUTO;
				else if (strcmp(settings_element->Attribute("sharpness"), "default" ) == 0) config.sharpness=SETTING_DEFAULT;
				else config.sharpness = atoi(settings_element->Attribute("sharpness"));
			}
			if(settings_element->Attribute("focus")!=NULL) {
				if (strcmp(settings_element->Attribute("focus"), "max" ) == 0) config.focus=SETTING_MAX;
				else if (strcmp(settings_element->Attribute("focus"), "min" ) == 0) config.focus=SETTING_MIN;
				else if (strcmp(settings_element->Attribute("focus"), "auto" ) == 0) config.focus=SETTING_AUTO;
				else if (strcmp(settings_element->Attribute("focus"), "default" ) == 0) config.focus=SETTING_DEFAULT;
				else config.focus = atoi(settings_element->Attribute("focus"));
			}
		}
		
		width = config.width;
		height = config.height;
		colour = config.color;
		bytes = (colour?3:1);
		
	}

void CameraEngine::saveSettings() {

	if (strcmp( config_file, "none" ) == 0) {
#ifdef __APPLE__
		char path[1024];
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		CFURLRef mainBundleURL = CFBundleCopyBundleURL( mainBundle);
		CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
		CFStringGetCString( cfStringRef, path, 1024, kCFStringEncodingASCII);	
		CFRelease( mainBundleURL);
		CFRelease( cfStringRef);
		sprintf(full_path,"%s/Contents/Resources/camera.xml",path);
		config_file = full_path;
#else
		config_file = "./camera.xml";
#endif
	}

	TiXml::TiXmlDocument xml_settings( config_file );
	xml_settings.LoadFile();
	if( xml_settings.Error() )
	{
		std::cout << "Error loading camera configuration file: " << config_file << std::endl;
		return;
	}
	
	TiXml::TiXmlHandle docHandle( &xml_settings );
	TiXml::TiXmlHandle camera = docHandle.FirstChild("portvideo").FirstChild("camera");
	TiXml::TiXmlElement* settings_element = camera.FirstChild("settings").Element();
	
	char config_value[64];
	
	if (settings_element!=NULL) {
		if(settings_element->Attribute("brightness")!=NULL) {

			if (config.brightness==SETTING_MAX) settings_element->SetAttribute("brightness","max");
			else if (config.brightness==SETTING_MIN) settings_element->SetAttribute("brightness","min");
			else if (config.brightness==SETTING_AUTO) settings_element->SetAttribute("brightness","auto");
			else if (config.brightness==SETTING_DEFAULT) settings_element->SetAttribute("brightness","default");
			else {
				sprintf(config_value,"%d",config.brightness);
				settings_element->SetAttribute("brightness",config_value);
			}
		}
		if(settings_element->Attribute("gain")!=NULL) {
			if (config.gain==SETTING_MAX) settings_element->SetAttribute("gain","max");
			else if (config.gain==SETTING_MIN) settings_element->SetAttribute("gain","min");
			else if (config.gain==SETTING_AUTO) settings_element->SetAttribute("gain","auto");
			else if (config.gain==SETTING_DEFAULT) settings_element->SetAttribute("gain","default");
			else {
				sprintf(config_value,"%d",config.gain);
				settings_element->SetAttribute("gain",config_value);
			}
		}
		if(settings_element->Attribute("gamma")!=NULL) {
			if (config.gamma==SETTING_MAX) settings_element->SetAttribute("gamma","max");
			else if (config.gamma==SETTING_MIN) settings_element->SetAttribute("gamma","min");
			else if (config.gamma==SETTING_AUTO) settings_element->SetAttribute("gamma","auto");
			else if (config.gamma==SETTING_DEFAULT) settings_element->SetAttribute("gamma","default");
			else {
				sprintf(config_value,"%d",config.gamma);
				settings_element->SetAttribute("gamma",config_value);
			}
		}
		if(settings_element->Attribute("shutter")!=NULL) {
			if (config.shutter==SETTING_MAX) settings_element->SetAttribute("shutter","max");
			else if (config.shutter==SETTING_MIN) settings_element->SetAttribute("shutter","min");
			else if (config.shutter==SETTING_AUTO) settings_element->SetAttribute("shutter","auto");
			else if (config.shutter==SETTING_DEFAULT) settings_element->SetAttribute("shutter","default");
			else {
				sprintf(config_value,"%d",config.shutter);
				settings_element->SetAttribute("shutter",config_value);
			}
		}
		if(settings_element->Attribute("exposure")!=NULL) {
			if (config.exposure==SETTING_MAX) settings_element->SetAttribute("exposure","max");
			else if (config.exposure==SETTING_MIN) settings_element->SetAttribute("exposure","min");
			else if (config.exposure==SETTING_AUTO) settings_element->SetAttribute("exposure","auto");
			else if (config.exposure==SETTING_DEFAULT) settings_element->SetAttribute("exposure","default");
			else {
				sprintf(config_value,"%d",config.exposure);
				settings_element->SetAttribute("exposure",config_value);
			}
		}
		if(settings_element->Attribute("sharpness")!=NULL) {
			if (config.sharpness==SETTING_MAX) settings_element->SetAttribute("sharpness","max");
			else if (config.sharpness==SETTING_MIN) settings_element->SetAttribute("sharpness","min");
			else if (config.sharpness==SETTING_AUTO) settings_element->SetAttribute("sharpness","auto");
			else if (config.sharpness==SETTING_DEFAULT) settings_element->SetAttribute("sharpness","default");
			else {
				sprintf(config_value,"%d",config.sharpness);
				settings_element->SetAttribute("sharpness",config_value);
			}
		}
		if(settings_element->Attribute("focus")!=NULL) {
			if (config.focus==SETTING_MAX) settings_element->SetAttribute("focus","max");
			else if (config.focus==SETTING_MIN) settings_element->SetAttribute("focus","min");
			else if (config.focus==SETTING_AUTO) settings_element->SetAttribute("focus","auto");
			else if (config.focus==SETTING_DEFAULT) settings_element->SetAttribute("focus","default");
			else {
				sprintf(config_value,"%d",config.focus);
				settings_element->SetAttribute("focus",config_value);
			}
		}
	}
	
	xml_settings.SaveFile();
	if( xml_settings.Error() ) std::cout << "Error saving camera configuration file: "  << config_file << std::endl;

}

	void CameraEngine::uyvy2gray(int width, int height, unsigned char *src, unsigned char *dest) {

		for (int i=height*width/2;i>0;i--) {
			src++;
			*dest++ = *src++;
			src++;
			*dest++ = *src++;
		}
	}

	void CameraEngine::yuyv2gray(int width, int height, unsigned char *src, unsigned char *dest) {
		for (int i=height*width/2;i>0;i--) {
			*dest++ = *src++;
			src++;
			*dest++ = *src++;
			src++;
		}
	}

	void CameraEngine::uyvy2rgb(int width, int height, unsigned char *src, unsigned char *dest) {
	
			int R,G,B;
			int Y1,Y2;
			int cG,cR,cB;

			for(int i=height*width/2;i>0;i--) {
				cB = ((*src - 128) * 454) >> 8;
				cG = (*src++ - 128) * 88;
				Y1 = *src++;  
				cR = ((*src - 128) * 359) >> 8;
				cG = (cG + (*src++ - 128) * 183) >> 8;
				Y2 = *src++;
					
				R = Y1 + cR;
				G = Y1 + cG;
				B = Y1 + cB;
						
				SAT(R);
				SAT(G);
				SAT(B);
	
				*dest++ = B;
				*dest++ = G;
				*dest++ = R;

				R = Y2 + cR;
				G = Y2 + cG;
				B = Y2 + cB;
						
				SAT(R);
				SAT(G);
				SAT(B);
	
				*dest++ = B;
				*dest++ = G;
				*dest++ = R;
			}		
	}


void CameraEngine::applyCameraSetting(int mode, int value) {

	switch (value) {
		case SETTING_DEFAULT: break;
		case SETTING_AUTO: setCameraSettingAuto(mode); break;
		case SETTING_MIN: setCameraSetting(mode,getMinCameraSetting(mode)); break;
		case SETTING_MAX: setCameraSetting(mode,getMaxCameraSetting(mode)); break;
		default: {
			int max = getMaxCameraSetting(mode);
			int min = getMinCameraSetting(mode);
			if (value<min) value = min;
			else if (value>max) value = max;
			setCameraSetting(mode,value);
		}
	}
}

void CameraEngine::applyCameraSettings() {

	applyCameraSetting(BRIGHTNESS,config.brightness);
	applyCameraSetting(GAIN,config.gain);
	applyCameraSetting(SHUTTER,config.shutter);
	applyCameraSetting(EXPOSURE,config.exposure);
	applyCameraSetting(SHARPNESS,config.sharpness);
	applyCameraSetting(FOCUS,config.focus);
	applyCameraSetting(GAMMA,config.gamma);
}
