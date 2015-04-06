#include "ofApp.h"

#include "ofxSuperLog.h"
#include "ofxTimeMeasurements.h"
#include "ofxRemoteUIVars.h"

//--------------------------------------------------------------
// callback function for ofxRemoteUI actions
bool bRecalcNow = true;

void ruiServerCallback(RemoteUIServerCallBackArg arg){
    switch (arg.action) {
        case CLIENT_DID_SET_PRESET:
        case CLIENT_UPDATED_PARAM:
            bRecalcNow = true;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofLogToFile("log.txt", true);
    //ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetWindowShape(1280, 900);

    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();

    TIME_SAMPLE_SET_FRAMERATE(60);
    TIME_SAMPLE_ENABLE();

    // setup ofxRemoteUI
    RUI_SETUP();
    RUI_SET_CALLBACK(ruiServerCallback);
    // TODO: setup params here
    RUI_NEW_GROUP("App");
    RUI_DEFINE_VAR_WV(bool, "app-fullscreen", false);
    arTest.setupParams();
    RUI_LOAD_FROM_XML();
    
    arTest.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    float dt = (1.f/60.f); // elapsed time (60 fps)

    if(bRecalcNow){
        // TODO: perform post-param-change updates
        ofSetFullscreen(RUI_VAR(bool, "app-fullscreen"));
        arTest.updateParams();
    }

    arTest.update(dt);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(120), ofColor(170));
    arTest.drawRestoring();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case 'f':
            OFX_REMOTEUI_SERVER_SET_VAR(bool, "app-fullscreen", !RUI_VAR(bool, "app-fullscreen"));
            bRecalcNow = true;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::exit(){

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
