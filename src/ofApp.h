#pragma once

#include "ofMain.h"
#include "ofxCv.h"
//#include "inputclass.h"
#include "DiJoyStick.h"
#define WMAX 200 //maximum steering value we use to calculate
#define WTRESH 5 //threshold on a scale of WMAX
#define MAXFORCE 100 //on a scale of WMAX
#define MINFORCE 25
#define WINMAX 65535 //wheel input max, from directinput
#define WINCENTER 32767

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void printLog(string msg);
		void updateController();
		void updateRcCarControl();
		void updateRcCarPosition();
		void mapCamToTrack();

		//serial connection:
		void startSerial();									//handshakes with the Arduino
		ofSerial serial;
		int inByte;
		bool serialStart;

		bool logEnabled;
    
    ofPoint center; //holds the center of the track
    

    //car tracking implementation
    ofPoint ploc;       //old location
    ofPoint loc;        //location
    ofVec2f vel;        //speed
    float heading;          //heading in degrees (should maybe change with steering input)
    float distance;         //distance from center
    //end
    
    //webcam color tracking implementation:
    ofVideoGrabber cam;
    ofxCv::ContourFinder contourFinder;
    float threshold;
    ofxCv::TrackingColorMode trackingColorMode;
    ofColor targetColor;
    //end
    
	//forcefeedback wheel
	DiJoyStick* wheel;
	LPDIRECTINPUT lpDi;
    long wPos; //holds position of wheel
	int wAimPos; //holds position wheel should go to
	int noForce;
	int wForce; //holds calculated force to reach aimed position
	bool forceEnabled;

	//car variables
	int speed; //speed of car 
    //drawing the track... class? thread?
    
    //
    
    
		
};
