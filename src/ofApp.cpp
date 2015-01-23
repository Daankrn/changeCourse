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

	int baud = 9600;
	serial.listDevices();
    serial.setup(2, baud); //bluetooth on mac...
	ofLogLevel(OF_LOG_VERBOSE);
	wheel = new DiJoyStick();
	lpDi = 0;
	noForce = WMAX/2;
	wForce = noForce; //center force
	forceEnabled = false;
	speed = 0;
	if(SUCCEEDED(DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &lpDi, 0))) {
		ofLog(OF_LOG_NOTICE, "created direct input");
			 wheel->enumerate(lpDi);
			 if(wheel->getEntryCount() > 0){
				 ofLog(OF_LOG_NOTICE, "wheel connected");
			 } else{
				 ofLog(OF_LOG_ERROR,"no wheel connected");
			 }
	}
}

//--------------------------------------------------------------
void ofApp::update(){

    updateController();			//read and write FFwheel
	updateRcCarControl();		//write bytes to rc car
	updateRcCarPosition();		//update position from webcam for track+steering
	
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
    drawHighlightString(ofToString((int) wForce) + " wForce", 10, 40);
    drawHighlightString(trackingColorMode == TRACK_COLOR_HSV ? "HSV tracking" : "RGB tracking", 10, 60);
    drawHighlightString(ofToString((int) wAimPos) + " wAimPos", 10, 80);
	drawHighlightString(ofToString((int) wPos) + " wPos", 10, 100);
    ofTranslate(8, 125);
    ofFill();
    ofSetColor(0);
    ofRect(-3, -3, 64+6, 64+6);
    ofSetColor(targetColor);
    ofRect(0, 0, 64, 64);

}

//--------------------------------------------------------------
void ofApp::updateController(){

	wAimPos = ofMap(mouseX,0, ofGetWidth(),0,WMAX); //temporarily map aimed wheel position to mouse
	//cout << wAimPos << endl;
	wheel->update();
            const DiJoyStick::Entry* e = wheel->getEntry(0);
            if(e) {
                const DIJOYSTATE2* js = &e->joystate;
				//cout << js->lX << endl;
				//map wheel position from 0 to WMAX
				wPos = ofMap(js->lX,0,WINMAX,0,WMAX);
				//cout << wPos << endl;
				//cout << wAimPos << endl;
				ofLog(OF_LOG_VERBOSE, "wPos = "+wPos);
				ofLog(OF_LOG_VERBOSE, "wAimPos = " +wAimPos);

				//function to calculate force for steering:
				//add max force to steering wheel, should steer crappy
				int f = ofMap(abs(wAimPos - wPos), 0, 100,MINFORCE, MAXFORCE,true);
				if(wPos < wAimPos-WTRESH) //on the left
				{
					wForce = noForce - f;
					//wForce = noForce-MAXFORCE;
					//steer to right
				} else if(wPos > wAimPos+WTRESH)//on the right
				{
					wForce = noForce+f;
					//steer left
				} else {
					//quite close no need for force steering
					wForce = noForce;
				}

				//send forces to wheel
				if(forceEnabled){
				wheel->g_nXForce = wheel->SetDeviceForcesXY(wForce,WMAX);
				}
            }
}

//--------------------------------------------------------------
//updates steering position and speed of RC car (arduino) over
//bluetooth serial connection
void ofApp::updateRcCarControl(){
    
	string msg = "s"+ofToString(speed)+"a"+ofToString(wPos)+"b"+ofToString(1);
	
	cout << msg << endl;
	unsigned char* msguc = new unsigned char[msg.size()];
	memcpy(msguc, msg.c_str(), msg.size());
	serial.writeBytes(msguc, msg.size());
	delete [] msguc;

    //start serial connection by handshake
    /*if(!serialStart){
		inByte = serial.readByte();				//read serial data coming in
		startSerial();
	} else {*/
		//we have connection, let's steer!
		//serial.writeByte(speed);
		//TODO add some delimiter
		//serial.writeByte(wPos);
	//}
}

//--------------------------------------------------------------
//update car position via webcam tracking
void ofApp::updateRcCarPosition(){
	//reads position of RC car via calibrated webcam. no mapping
	//supposed to happen here. Look @ mapCamToTrack()
}

//--------------------------------------------------------------
//uses ofxCv to map camera to track so the position of the car
//can be translated easily
void ofApp::mapCamToTrack(){
	//not quite sure what to use...
}

//--------------------------------------------------------------
void ofApp::startSerial(){
    serial.writeByte('B');
    if(inByte == 'A'){
        serial.flush(false, true);
        cout << "handshake made" << endl;
        serialStart = true;
//can start sending speed and steering now.
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
if(key == 'f'){
	//enable/disable force feedback
	forceEnabled = !forceEnabled;
	cout << "forceEnabled " << forceEnabled << endl;
}
if(key == 's'){
	speed--;
	cout << "speed =" << speed << endl;
}
if(key == 'S'){
	speed++;
	cout << "speed =" << speed << endl;
}
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

//--------------------------------------------------------------
void ofApp::printLog(string msg){ 
	if(logEnabled){
		cout << msg << endl;
	}
}
