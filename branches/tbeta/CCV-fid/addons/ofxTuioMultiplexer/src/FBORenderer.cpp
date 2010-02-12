#include "FBORenderer.h"

FBORenderer::FBORenderer(){
//initFBO()
}

void FBORenderer::initFBO(int _width,int _height,float _blobSize, float _scaleFactor){
	
	cout <<"FBO.: blobsize: "<< _blobSize << ", scale: "<<_scaleFactor<< endl;
		 GLenum status;
	
		width =_width;
		height = _height;
		blobSize = _blobSize;
		scaleFactor = _scaleFactor;
		gpuReadBackBuffer = new unsigned char[(width*height*8)+1];
		//gpuReadBackBuffer = new unsigned char[width*height];
		//return;

		
	 //allocate ouput buffer on GPU using FBO
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &output_texture);
	glBindTexture(GL_TEXTURE_2D, output_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glGenerateMipmapEXT(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
	
	//generate fbo
    glGenFramebuffersEXT(1, &fbo_buffer);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_buffer);

	 //attach texture to fbo
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, output_texture, 0);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	
	
	//renderbuffer
	glGenRenderbuffersEXT(1, &render_buffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, render_buffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, render_buffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	

	/*
	 // create a renderbuffer object to store color info                             
	  glGenRenderbuffersEXT(1, &render_buffer);
	  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, render_buffer);
	  //The storage format is RGBA8
	  glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8,  width, height);
	  //Attach color buffer to FBO
	  glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, color_rb);

	  glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	*/


    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	cout <<"FBO.Status:"<< status << endl;

	
	}


GLuint * FBORenderer::getTexture(){

return &output_texture;
}

void FBORenderer::grabImageData(void *pixels)
{ //target= Texture
	//grab the frame to a raw openGL texture
	
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, output_texture);

	//if(ffmv != NULL){
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);//Hier wird die Textur überschrieben
	//}  
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D,0);
}



void FBORenderer::drawBlobs(ofxCvColorImage &gpuReadBackImageGS, std::vector<Blob> &vtcur){
//beispiel:  grayScale->apply(processedTex);//hier die OpenGL Befehle
	
	glPushAttrib(GL_VIEWPORT_BIT);
    glPushAttrib(GL_ENABLE_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    //render to the fbo  
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_buffer);

	//Normal RenderToTexture Mode
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, output_texture);
	//-----


    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-1, 1.0, -1, 1.0, -1, 1.0);
	glOrtho(0, width, height, 0, -1, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	ofSetRectMode(OF_RECTMODE_CORNER);
	ofFill();
	for(std::vector<Blob>::iterator it = vtcur.begin();it != vtcur.end();it++)
	{
		//Here we draw the blobs 
		ofCircle( (*it).centroid.x * width, height-((*it).centroid.y*height), blobSize/scaleFactor);
		cout << "CIRCLE:" << (*it).centroid.x << ":" << (*it).centroid.y << endl;
	}
	/*
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, output_texture);

	//draw full screen quad with input texture aplied
		
        glBegin(GL_QUADS);
			glColor3d(1.0, 0.0, 0.0);
			glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
			glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
			glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
			glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
        glEnd();
	
	glBindTexture(GL_TEXTURE_2D,0);
	*/
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	glPopAttrib();

	  //----- glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
       //----- glReadPixels(0,0,width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, gpuReadBackBuffer);//glReadPixels - ließt einen Block von Pixel aus dem Framebuffer. 
		
	
		
		glReadPixels(0,0,width, height, GL_RGB, GL_UNSIGNED_BYTE, gpuReadBackBuffer);       
		gpuReadBackImageGS.setFromPixels(gpuReadBackBuffer, width, height);//Schreibt die Pixel in ein ofImage
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		
		
		//ATIRadeon HD 5850
		//OpenGL Version 6.14.10.8918
		//Treiber ATI: 8.651
		//http://support.amd.com/us/gpudownload/windows/previous/Pages/radeonaiw_xp.aspx
		//http://game.amd.com/us-en/drivers_catalyst.aspx?p=xp/radeonx-xp


		//gpuReadBackImageGS.setFromPixels(output_texture, width, height);
		//gpuReadBackImageGS.getTextureReference().loadData(output_texture,width,height,GL_RGB);
		//gpuReadBackImageGS= output_texture;
		//glBindTexture(GL_TEXTURE_2D,0);



	
}
void FBORenderer::drawBlobs(ofImage &gpuReadBackImageGS, std::vector<Blob> &vtcur){

	glPushAttrib(GL_VIEWPORT_BIT);
    glPushAttrib(GL_ENABLE_BIT);

	
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

	 glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-1, 1.0, -1, 1.0, -1, 1.0);
	glOrtho(0, width,  0,height, -1, 1.0);// we have to flip the screen as grabScreen will flip it back
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	ofSetRectMode(OF_RECTMODE_CORNER);
	ofFill();
	
	for(std::vector<Blob>::iterator it = vtcur.begin();it != vtcur.end();it++)
	{
		//Here we draw the blobs 
		ofCircle( (*it).centroid.x * width, height-((*it).centroid.y*height), blobSize/scaleFactor);
	}
	
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glPopAttrib();
	glPopAttrib();

	gpuReadBackImageGS.grabScreen(0,0,width,height);
	//glReadPixels(0,0,width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, gpuReadBackBuffer); 
	//glReadPixels(0,0,width, height, GL_RGB, GL_UNSIGNED_BYTE, gpuReadBackBuffer);   
	//gpuReadBackImageGS.setFromPixels(gpuReadBackBuffer, width, height,OF_IMAGE_COLOR);//Schreibt die Pixel in ein ofImage
	
	}
