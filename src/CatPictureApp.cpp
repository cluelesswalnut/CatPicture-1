

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
	int frameNumber;

	void setDefaultColor();
	void addRectangle(uint8_t* dataArray, int rectHeight, int rectLength, int centerX, int centerY, Color8u c, bool fill);
	void addCircle(uint8_t* dataArray, int radius, int xCenter, int yCenter, Color8u c, bool fill);
	void addLine(int firstX, int firstY, int secondX, int secondY, Color8u c); //used http://www.codekeep.net/snippets/e39b2d9e-0843-4405-8e31-44e212ca1c45.aspx as a template
	void setPixel(int x, int y, Color8u c); //Required by addLine
	void addTriangle(int firstX, int firstY, int secondX, int secondY, int thirdX, int thirdY, Color8u c);
	void blur(uint8_t* image_to_blur);
};

void CatPictureApp::prepareSettings(Settings* settings){
	(*settings).setWindowSize(AppWidth, AppHeight);
	(*settings).setResizable(false);
}

void CatPictureApp::setDefaultColor(){
	uint8_t* dataArray = (*mySurface).getData();
	Color8u c = Color8u(255,255,255);

	for(int i = 0; i < AppWidth; i++){
		for(int j = 0; j < AppHeight; j++){
			int offset = 3*(i + j*TextureSize);
			dataArray[offset] = c.r;
			dataArray[offset+1] = c.b;
			dataArray[offset+2] = c.g;
		}
	}	
}

void CatPictureApp::addRectangle(uint8_t* dataArray, int rectHeight, int rectLength, int centerX, int centerY, Color8u c, bool fill){
	//A.1

	int staticHeight = rectHeight;
	int staticLength = rectLength;
	for(int i = centerY; i <= centerY + staticHeight; i++){
		if(rectHeight >= 0){
			for(int j = centerX; j <= centerX + staticLength; j++){
				if(rectLength >= 0){
					if(fill){
						if(i >= centerY || i <= centerY + staticHeight || j >= centerX || j <= centerX + staticLength){
							int offset = 3*(j + i*TextureSize);
							dataArray[offset] = c.r;
							dataArray[offset+1] = c.b;
							dataArray[offset+2] = c.g;
							rectLength--;
						}
					} else {
						if(i == centerY || i == centerY + staticHeight || j == centerX || j == centerX + staticLength){
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
	//A.2: Draws a circle at a given x,y coordinate with a given radius.
	if(fill){
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
	} else {
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
	addLine(firstX, firstY, secondX, secondY, c);
	addLine(firstX, firstY, thirdX, thirdY, c);
	addLine(secondX, secondY, thirdX, thirdY, c);
}

void CatPictureApp::blur(uint8_t* image_to_blur){
	//Convolution filters tend to overwrite the data that you need, so
	// we keep a temporary copy of the image_to_blur. There are certainly
	// more efficient ways to deal with this problem, but this is simple to
	// understand. 
	static uint8_t work_buffer[3*TextureSize*TextureSize];
	//This memcpy is not much of a performance hit.
	memcpy(work_buffer,image_to_blur,3*TextureSize*TextureSize);
	
	//These are used in right shifts.
	//Both of these kernels actually darken as well as blur.
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
	
	//Visit every pixel in the image, except the ones on the edge.
	//TODO Special purpose logic to handle the edge cases
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
					total_red   += (work_buffer[offset  ] >> k);
					total_green += (work_buffer[offset+1] >> k);
					total_blue  += (work_buffer[offset+2] >> k);
				}
			}
			offset = 3*(x + y*TextureSize);
			image_to_blur[offset]   = total_red;
			image_to_blur[offset+1] = total_green;
			image_to_blur[offset+2] = total_blue;
		}
	}
}

void CatPictureApp::setup() {
	mySurface = new Surface(TextureSize, TextureSize, false);
	frameNumber = 0;
}

void CatPictureApp::mouseDown( MouseEvent event ) {
	
	uint8_t* dataArray = (*mySurface).getData();
	addCircle(dataArray,20, event.getX(), event.getY(), Color8u(255,255,255), true);
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
	if(frameNumber%10 < 2){
		addLine(180, 500, 220, 500, Color8u(155,78,0));
		addLine(200, 520, 200, 480, Color8u(155,78,0));
	} else {
		addLine(188, 516, 212, 484, Color8u(155,78,0));
		addLine(212, 516, 188, 484, Color8u(155,78,0));
	}
	//Wheel2
	addCircle(dataArray, 20, 100, 500, Color8u(155,78,0), false);
	if(frameNumber%10 < 2){
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

	blur(dataArray);

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
