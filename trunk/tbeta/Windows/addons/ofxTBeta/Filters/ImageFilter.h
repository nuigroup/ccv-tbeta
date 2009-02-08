#ifndef IMAGEFILTER_H_
#define IMAGEFILTER_H_

#include "ShaderProgram.h"

#include <map>
#include <string>




class FilterParameter{
public:
	const char* name;
	int type;
	float value;
	float max;
	float min;
	
	
	inline FilterParameter(const char* n, float val, float minVal, float maxVal, int t) { 
		this->name=n; this->value=val; this->min=minVal; this->max=maxVal; this->type=t;  
	};

};



class ImageFilter		
{
	private:
		//should maybe go wih image..but for now we are only passing gl texture handles...
		int res_x, res_y;
		bool useGeometryShader;
		ShaderProgram* shader;
		
		//GLuint output_texture;
		
		const char* name;
		
		void parseXML(const char* fname);
	
	
	public:
		GLuint output_buffer;
		GLuint output_texture;
		std::map<std::string, FilterParameter*> parameters;
		
		ImageFilter(const char* fname, int outputWidth, int outputHeight);
		
		GLuint apply(GLuint inputTexture, GLuint inputTexture2=0);

		void drawOutputTexture(float x,float y, float w, float h);

		void setOutputSize(int x, int y){ res_x = x; res_y = y; }
		
		virtual ~ImageFilter();
};



//small helper function
inline void drawGLTexture(float x, float y, float w, float h, GLuint t){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, t);
		glPushMatrix();
		glBegin( GL_QUADS );
			glTexCoord2f(0,0);			glVertex3i(x, y,0);
			glTexCoord2f(1,0);			glVertex3i(x+w, y,0);
			glTexCoord2f(1,1);			glVertex3i(x+w, y+h,0);
			glTexCoord2f(0,1);			glVertex3i(x, y+h,0);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
}




#endif /*IMAGEFILTER_H_*/
