#include "ofApp.h"
using namespace ofxCv;
using namespace cv;
//--------------------------------------------------------------
void ofApp::setup(){
    cam.initGrabber(640, 480);
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(200);
    trackingColorMode = TRACK_COLOR_RGB;
    center.set(ofGetWidth()/2, ofGetHeight()/2);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    if(cam.isFrameNew()) {
        threshold = ofMap(mouseX, 0, ofGetWidth(), 0, 255);
        contourFinder.setThreshold(threshold);
        contourFinder.findContours(cam);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    cam.draw(0, 0);
    
    if(contourFinder.size() > 0){
        //if we find the color, update the location
        ploc = loc;
        loc.set(contourFinder.getCenter(0).x, contourFinder.getCenter(0).y);
    }
    
    ofCircle(loc.x, loc.y, 10); //car position visualized
    distance = ofDist(center.x, center.y, loc.x, loc.y);
    ofLine(center.x, center.y, loc.x, loc.y);
    
    
    drawHighlightString(ofToString((int) ofGetFrameRate()) + " fps", 10, 20);
    drawHighlightString(ofToString((int) threshold) + " threshold", 10, 40);
    drawHighlightString(trackingColorMode == TRACK_COLOR_HSV ? "HSV tracking" : "RGB tracking", 10, 60);
    drawHighlightString(ofToString((float) distance) + " distance", 10, 80);
    ofTranslate(8, 95);
    ofFill();
    ofSetColor(0);
    ofRect(-3, -3, 64+6, 64+6);
    ofSetColor(targetColor);
    ofRect(0, 0, 64, 64);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    targetColor = cam.getPixelsRef().getColor(x, y);
    contourFinder.setTargetColor(targetColor, trackingColorMode);

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
