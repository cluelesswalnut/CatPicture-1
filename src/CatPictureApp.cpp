

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CatPictureApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings(Settings* settings);

  private:
	Surface* mySurface;

	static const int AppWidth = 800;
	static const int AppHeight = 600;
	static const int TextureSize = 1024;
	Color8u c;

	void setDefaultColor();
	void addRectangle();
	void addCircle();
	void addLine();

};

void CatPictureApp::prepareSettings(Settings* settings){
	(*settings).setWindowSize(AppWidth, AppHeight);
	(*settings).setResizable(false);
}

void CatPictureApp::setDefaultColor(){
	uint8_t* dataArray = (*mySurface).getData();
	c.g = 255;
	c.r = 255;
	c.b = 255;

	for(int i = 0; i < AppWidth; i++){
		for(int j = 0; j < AppHeight; j++){
			int offset = 3*(i + j*TextureSize);
			dataArray[offset] = c.r;
			dataArray[offset+1] = c.b;
			dataArray[offset+2] = c.g;
				
		}
	}	
}

void CatPictureApp::addRectangle(){
	//A.1
	uint8_t* dataArray = (*mySurface).getData();
	c.g = 0;
	c.r = 0;
	c.b = 0;

	int rectHeight = 20;
	int rectLength = 20;

	for(int i = 0; i < AppHeight; i++){
		if(rectHeight >= 0){
			for(int j = 0; j < AppWidth; j++){
				if(rectLength >= 0){
					if(i == 0 || i == 20 || j == 0 || j == 20){
						int offset = 3*(j + i*TextureSize);
						dataArray[offset] = c.r;
						dataArray[offset+1] = c.b;
						dataArray[offset+2] = c.g;
						rectLength--;
						console() << rectLength << std::endl;
						//console() << rectHeight << std::endl;
					}
				}
			}
			rectLength = 20;
			rectHeight--;
			//console() << rectHeight << std::endl;
		}
	}
}

void CatPictureApp::addCircle(){
	//A.2
	int xCenter = AppWidth/2;
	int yCenter = AppHeight/2;
	int r = 15;
	uint8_t* dataArray = (*mySurface).getData();
	c = Color8u(0,0,0);

	for(int i = yCenter-r; i<=yCenter+r; i++){
		for(int j = xCenter-r; j <= xCenter+r; j++){
			int distance = (int)sqrt((double)((j - xCenter)*(j-xCenter) + (i - yCenter)*(i - yCenter)));
			if(distance <= r){
				if(distance%r < 1 && distance > 9){
					int offset = 3*(j + i*TextureSize);
					dataArray[offset] = c.r;
					dataArray[offset+1] = c.g;
					dataArray[offset+2] = c.b;
				}
			}
		}
	}
}


void CatPictureApp::addLine(){
	uint8_t* dataArray = (*mySurface).getData();
	
	int x0 = 20;
	int y0 = 20;
	int x1 = 20;
	int y1 = 40;

	bool steep = abs(y1 - y0) > abs(x1 - x0);

	if(steep){
		int placeHolder = x0;
		x0 = y0;
		y0 = placeHolder;

		placeHolder = x1;
		x1 = y1;
		y1 = placeHolder;
	}

	if(x0 > x1){
		int placeHolder = x0;
		x0 = x1;
		x1 = placeHolder;

		placeHolder = y0;
		y0 = y1;
		y1 = placeHolder;
	}
	
	int deltaX = x1 - x0;
	int deltaY = abs(y1 - y0);
	float error = 0;
	float deltaerr = deltaY / deltaX;
	int yStep;
	int y = y0;
	if(y0 < y1){
		yStep = 1;
	} else {
		yStep = -1;
	}

	for(int x = x0; x < x1; x++){
		if(steep){

		}
	{


}

void CatPictureApp::setup()
{
	

	mySurface = new Surface(TextureSize, TextureSize, false);
	setDefaultColor();
	addRectangle();
	addCircle();
}

void CatPictureApp::mouseDown( MouseEvent event )
{

}

void CatPictureApp::update()
{


}

void CatPictureApp::draw()
{

	gl::draw(*mySurface);
}

CINDER_APP_BASIC( CatPictureApp, RendererGl )
