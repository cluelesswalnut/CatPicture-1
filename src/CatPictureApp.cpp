/**
 *author: Jason Brammer
 *date: 2012-09-04
 *
 *The code for creating a line was derived from http://www.codekeep.net/snippets/e39b2d9e-0843-4405-8e31-44e212ca1c45.aspx
 *The setPixel(x,y) function was modified to accept a color as well
 *
 *This project satisfies goals A.1 (rectangle), A.2 (circle), A.3 (line), A.7 (triangle), B.1 (blur), E.5 (animation)
 */

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
	Surface* mySurface; //mySurface is the surface object whose pixel array is modified to create a drawing

	//Width and height of shown screen
	static const int AppWidth = 800;
	static const int AppHeight = 600;
	//full texture size that is allocated to memory must be a square and a power of 2
	static const int TextureSize = 1024;

	//track frames shown for wheel animation
	int frameNumber;

	/**
	 *Add a rectangle to the surface (A.1)
	 *@dataArray array of pixels the rectangle is to be added to
	 *@rectHeight height of the rectangle
	 *@rectLength length of the rectangle
	 *@startX top left corner x coordinate
	 *@startY top left corner y coordinate
	 *@c the color to draw the rectangle pixels in
	 *@fill (true)fill the rectangle, (false)only draw perimeter
	 */
	void addRectangle(uint8_t* dataArray, int rectHeight, int rectLength, int startX, int startY, Color8u c, bool fill);

	/**
	 *Add a circle to the surface (A.2)
	 *@dataArray array of pixels the rectangle is to be added to
	 *@radius of the circle to be drawn
	 *@xCenter x value of the center of the circle
	 *@yCenter y value of the center of the circle
	 *@c color to draw the circle pixels in
	 *@fill (true) fill the circle, (false)only draw perimeter
	 */
	void addCircle(uint8_t* dataArray, int radius, int xCenter, int yCenter, Color8u c, bool fill);
	
	/**
	 *Adds a line between two given points (A.3)
	 *used http://www.codekeep.net/snippets/e39b2d9e-0843-4405-8e31-44e212ca1c45.aspx as a template
	 *@firstX first point x coordinate
	 *@firstY first point y coordinate
	 *@secondX second point x coordinate
	 *@secondY second point y coordinate
	 *@c color of the line to be drawn
	 */
	void addLine(int firstX, int firstY, int secondX, int secondY, Color8u c); 
	
	/**
	 *Sets a specific pixel to a given color
	 *addLine and addTriangle are inherantly dependant on setPixel
	 *@x coordinate of pixel
	 *@y coordinate of pixel
	 *@c color to make pixel
	 */
	void setPixel(int x, int y, Color8u c);

	/**
	 *Adds a triangle to the surface by passing in three points (A.7)
	 *@firstX, firstY :: x,y coordinates of point 1
	 *@secondX, secondY :: x,y coordinate of point 2
	 *@thirdX, thirdY :: x,y coordinates of point 3
	 */
	void addTriangle(int firstX, int firstY, int secondX, int secondY, int thirdX, int thirdY, Color8u c);

	/**
	 *Adds a blur to the sky of the image only (B.1)
	 *Intent is to eventually have a mouse event to draw clouds and blur the sky to make them look more "realistic"
	 *@surfaceToBlur surface to be modified by the blur function
	 */
	void blur(uint8_t* surfaceToBlur);
};

void CatPictureApp::prepareSettings(Settings* settings){
	(*settings).setWindowSize(AppWidth, AppHeight);
	(*settings).setResizable(false);
}


void CatPictureApp::addRectangle(uint8_t* dataArray, int rectHeight, int rectLength, int startX, int startY, Color8u c, bool fill){
	
	//static variables are required to preserve the original dimensions as we decrement in the for loop
	int staticHeight = rectHeight;
	int staticLength = rectLength;

	for(int i = startY; i <= startY + staticHeight; i++){
		if(rectHeight >= 0){
			for(int j = startX; j <= startX + staticLength; j++){
				if(rectLength >= 0){
					if(fill){
						if(i >= startY || i <= startY + staticHeight || j >= startX || j <= startX + staticLength){
							int offset = 3*(j + i*TextureSize);
							dataArray[offset] = c.r;
							dataArray[offset+1] = c.b;
							dataArray[offset+2] = c.g;
							rectLength--;
						}
					} else {
						if(i == startY || i == startY + staticHeight || j == startX || j == startX + staticLength){
							int offset = 3*(j + i*TextureSize);
							dataArray[offset] = c.r;
							dataArray[offset+1] = c.b;
							dataArray[offset+2] = c.g;
							rectLength--;
						}
					}
				}
			}
			rectLength = staticLength;
			rectHeight--;
		}
	}
}


void CatPictureApp::addCircle(uint8_t* dataArray, int radius, int xCenter, int yCenter, Color8u c, bool fill){
	//Draws a circle at a given x,y coordinate with a given radius.
	if(fill){ //Will fill the circle
		for(int i = yCenter-radius; i<=yCenter+radius; i++){
			for(int j = xCenter-radius; j <= xCenter+radius; j++){
				int distance = (int)sqrt((double)((j - xCenter)*(j-xCenter) + (i - yCenter)*(i - yCenter)));
				if(distance <= radius){
					int offset = 3*(j + i*TextureSize);
					dataArray[offset] = c.r;
					dataArray[offset+1] = c.g;
					dataArray[offset+2] = c.b;
					
				}
			}
		}
	} else { //Will draw only the circumference of the circle
		for(int i = yCenter-radius; i<=yCenter+radius; i++){
			for(int j = xCenter-radius; j <= xCenter+radius; j++){
				int distance = (int)sqrt((double)((j - xCenter)*(j-xCenter) + (i - yCenter)*(i - yCenter)));
				if(distance <= radius){
					if(distance%radius < 1 && distance > 9){
						int offset = 3*(j + i*TextureSize);
						dataArray[offset] = c.r;
						dataArray[offset+1] = c.g;
						dataArray[offset+2] = c.b;
					}
				}
			}
		}
	}
}

void CatPictureApp::addLine(int firstX, int firstY, int secondX, int secondY, Color8u c){
	//I changed many of the variables to make them more consistant with the naming conventions
	//I used in this code
	
	int F, x, y;

    if (firstX > secondX) {  
		// Swap points if first point is on the right of second point
		// This alleviates the need for two algorithms, one that writes left to right and one that writes right to left
        swap(firstX, secondX);
        swap(firstY, secondY);
    }

    // Handle trivial cases separately for algorithm speed up.
    // Trivial case 1: m = +/-INF (Vertical line), no special handling required
    if (firstX == secondX) {
        if (firstY > secondY){  
			// Swap y-coordinates if the first point is above the second point
            swap(firstY, secondY);
        }

        x = firstX;
        y = firstY;
        while (y <= secondY) {
            setPixel(x, y, c);
            y++;
        }
        return;
    }
    // Trivial case 2: m = 0 (Horizontal line), no special handling required
    else if (firstY == secondY) {
        x = firstX;
        y = firstY;

        while (x <= secondX) {
            setPixel(x, y, c);
            x++;
        }
        return;
    }

    int dy            = secondY - firstY;  // y-increment from p1 to p2
    int dx            = secondX - firstX;  // x-increment from p1 to p2
    int dy2           = (dy << 1);  // dy << 1 == 2*dy
    int dx2           = (dx << 1);
    int dy2_minus_dx2 = dy2 - dx2;  // precompute constant for speed up
    int dy2_plus_dx2  = dy2 + dx2;

    if (dy >= 0){    // What to do if slope >= 0
        if (dy <= dx) {
			// Case 1: 0 <= m <= 1 (Original case)
            F = dy2 - dx;    // initial F

            x = firstX;
            y = firstY;
            while (x <= secondX) {
                setPixel(x, y, c);
                if (F <= 0) {
                    F += dy2;
                }
                else {
                    y++;
                    F += dy2_minus_dx2;
                }
                x++;
            }
        } else {
			// Case 2: 1 < m < INF (Mirror about y=x line replace all dy by dx and dx by dy)
            F = dx2 - dy;    // initial F

            y = firstY;
            x = firstX;
            while (y <= secondY) {
                setPixel(x, y, c);
                if (F <= 0) {
                    F += dx2;
                } else {
                    x++;
                    F -= dy2_minus_dx2;
                }
                y++;
            }
        }
    } else {    // What to do if slope < 0
    
        // Case 3: -1 <= m < 0 (Mirror about x-axis, replace all dy by -dy)
        if (dx >= -dy){
            F = -dy2 - dx;    // initial F

            x = firstX;
            y = firstY;
            while (x <= secondX){
                setPixel(x, y, c);
                if (F <= 0){
                    F -= dy2;
                } else {                
                    y--;
                    F -= dy2_plus_dx2;
                }
                x++;
            }
        // Case 4: -INF < m < -1 (Mirror about x-axis and mirror about y=x line, replace all dx by -dy and dy by dx)
        } else { 
            F = dx2 + dy;    // initial F

            y = firstY;
            x = firstX;
            while (y >= secondY) {
                setPixel(x, y, c);
                if (F <= 0) {
                    F += dx2;
                } else {
                    x++;
                    F += dy2_plus_dx2;
                }
                y--;
            }
        }
    }
}

void CatPictureApp::setPixel(int x, int y, Color8u c)
{
	//Sets a single pixel on the screen to a specified color
	//setPixel is a dependancy for addLine and addTriangle
	uint8_t* dataArray = (*mySurface).getData();

	for(int i = 0; i < AppHeight; i++){
		if(i == y){
			for(int j = 0; j < AppWidth; j++){
				if(j == x){
					int offset = 3*(j + i*TextureSize);
					dataArray[offset] = c.r;
					dataArray[offset+1] = c.g;
					dataArray[offset+2] = c.b;
				}
			}
		}
	}
}

void CatPictureApp::addTriangle(int firstX, int firstY, int secondX, int secondY, int thirdX, int thirdY, Color8u c){
	//Since a triangle is simply 3 lines drawn over three points we can call addLine for three given points
	//to achieve a triangle on the surface
	addLine(firstX, firstY, secondX, secondY, c);
	addLine(firstX, firstY, thirdX, thirdY, c);
	addLine(secondX, secondY, thirdX, thirdY, c);
}

void CatPictureApp::blur(uint8_t* surfaceToBlur){
	static uint8_t surfaceBuffer[3*TextureSize*TextureSize];
	
	//Makes a copy of the surface so we don't overwrite the data we need
	memcpy(surfaceBuffer,surfaceToBlur,3*TextureSize*TextureSize);
	
	//Generic kernel to blur all the pixels
	uint8_t blurKernel[9] = 
	   {4,3,4,
		4,2,4,
		4,3,4};
	
	uint8_t total_red  =0;
	uint8_t total_green=0;
	uint8_t total_blue =0;
	int offset;
	int k;
	int y,x,ky,kx;
	
	//iterates through entire surface array and alters all the pixels
	for( y=1;y<300-1;y++){
		for( x=1;x<AppWidth-1;x++){
			offset = 3*(x + y*AppWidth);
			total_red=0;
			total_green=0;
			total_blue=0;
			for( ky=-1;ky<=1;ky++){
				for( kx=-1;kx<=1;kx++){
					offset = 3*(x + kx + (y+ky)*TextureSize);
					k = blurKernel[kx+1 + (ky+1)*3];
					total_red   += (surfaceBuffer[offset  ] >> k);
					total_green += (surfaceBuffer[offset+1] >> k);
					total_blue  += (surfaceBuffer[offset+2] >> k);
				}
			}
			offset = 3*(x + y*TextureSize);
			surfaceToBlur[offset]   = total_red;
			surfaceToBlur[offset+1] = total_green;
			surfaceToBlur[offset+2] = total_blue;
		}
	}
}

void CatPictureApp::setup() {
	mySurface = new Surface(TextureSize, TextureSize, false);
	frameNumber = 0;
}

void CatPictureApp::mouseDown( MouseEvent event ) {

}

void CatPictureApp::update()
{
	uint8_t* dataArray = (*mySurface).getData();
	//sky
	addRectangle(dataArray, 400, 799, 0, 0, Color8u(23,225,236), true);
	//grass
	addRectangle(dataArray, 200, 799, 0, 400, Color8u(37, 52, 167), true);
	//Wheel1
	addCircle(dataArray, 20, 200, 500, Color8u(155,78,0), false);
	if(frameNumber%10 < 2){ //rudimentary wheel animation
		addLine(180, 500, 220, 500, Color8u(155,78,0));
		addLine(200, 520, 200, 480, Color8u(155,78,0));
	} else {
		addLine(188, 516, 212, 484, Color8u(155,78,0));
		addLine(212, 516, 188, 484, Color8u(155,78,0));
	}
	//Wheel2
	addCircle(dataArray, 20, 100, 500, Color8u(155,78,0), false);
	if(frameNumber%10 < 2){ //rudimentary wheel animation
		addLine(80, 500, 120, 500, Color8u(155,78,0));
		addLine(100, 520, 100, 480, Color8u(155,78,0));
	} else {
		addLine(88, 516, 112, 484, Color8u(155,78,0));
		addLine(112, 516, 88, 484, Color8u(155,78,0));
	}
	//base of wagon
	addRectangle(dataArray, 20, 140, 80, 450, Color8u(155,78,0), true);
	//wagon cover
	addRectangle(dataArray, 60, 100, 85, 400, Color8u(155,78,0), true);
	addLine(80, 450, 220, 450, Color8u(0,0,0));
	addLine(90, 400, 90, 450, Color8u(0,0,0));
	addLine(120, 400, 120, 450, Color8u(0,0,0));
	addLine(150, 400, 150, 450, Color8u(0,0,0));
	addLine(180, 400, 180, 450, Color8u(0,0,0));
	//house
	addRectangle(dataArray, 90, 200, 500, 360, Color8u(170, 10, 0), true);
	//windows
	addRectangle(dataArray, 40, 40, 520, 380, Color8u(255, 255, 255), true);
	addRectangle(dataArray, 40, 40, 520, 380, Color8u(0, 0, 0), false);
	addRectangle(dataArray, 40, 40, 640, 380, Color8u(255, 255, 255), true);
	addRectangle(dataArray, 40, 40, 640, 380, Color8u(0, 0, 0), false);
	addLine(660, 380, 660, 420, Color8u(0,0,0));
	addLine(640, 400, 680, 400, Color8u(0,0,0));
	addLine(520, 400, 560, 400, Color8u(0,0,0));
	addLine(540, 380, 540, 420, Color8u(0,0,0));
	//roof
	addTriangle(480,360,600,320,720,360, Color8u(0,0,0));
	//clouds
	addCircle(dataArray, 20, 40, 40, Color8u(255,255,255), true);
	addCircle(dataArray, 30, 50, 60, Color8u(255,255,255), true);
	addCircle(dataArray, 25, 500, 100, Color8u(255,255,255), true);
	addCircle(dataArray, 40, 550, 75, Color8u(255,255,255), true);


	blur(dataArray);

	//write image to a file
	if(frameNumber == 0){
		writeImage("brammejd.png",*mySurface);
	}

	frameNumber++;
}

void CatPictureApp::draw()
{
	gl::draw(*mySurface);
}

CINDER_APP_BASIC( CatPictureApp, RendererGl )
