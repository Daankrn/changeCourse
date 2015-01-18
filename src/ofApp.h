#pragma once

#include "ofMain.h"
#include "ofxCv.h"

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
    
    
    //drawing the track... class? thread?
    
    //
    
    
		
};
