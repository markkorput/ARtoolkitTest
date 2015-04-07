//
//  ofxLiveOsc.cpp
//  BaseApp
//
//  Created by Mark van de Korput on 07/04/15.
//
//

#include "ofxLiveOsc.h"

#include "ofxSuperLog.h"
#include "ofxRemoteUIVars.h"

bool ofxLiveOsc::bClassParamsCreated = false;

ofxLiveOsc::ofxLiveOsc(){
    url = "";
    port = 0;
}

ofxLiveOsc::~ofxLiveOsc(){

}

void ofxLiveOsc::setupParams(){
    if(!bClassParamsCreated){
        bClassParamsCreated = true;

        LOG_WARNING << "TODO: make sure ofxLiveOsc::setupParams() is executed only once";
        RUI_NEW_GROUP("ofxLiveOsc");
        RUI_DEFINE_VAR_WV(string, "ofxLiveOsc::url", "127.0.0.1");
        RUI_DEFINE_VAR_WV(int, "ofxLiveOsc::port", 9000, 0, 9999);
        RUI_DEFINE_VAR_WV(string, "ofxLiveOsc::launchClipMsg", "/live/play/clip");
    }

    // instance-specific param definitions go here
}

void ofxLiveOsc::updateParams(){
    string newUrl = RUI_VAR(string, "ofxLiveOsc::url");
    int newPort = RUI_VAR(int, "ofxLiveOsc::port");

    // if either the url or the port changed,
    // re-initialize the OSC connection
    if(newPort != port || newUrl != url){
        url = newUrl;
        port = newPort;
        oscSender.setup(url, port);
    }
}

void ofxLiveOsc::setup(){
    setupParams();
    updateParams();
}

void ofxLiveOsc::update(float dt){

}

void ofxLiveOsc::draw(){

}

void ofxLiveOsc::playClip(unsigned int track, unsigned int clip){
    ofxOscMessage msg;
    msg.setAddress(RUI_VAR(string, "ofxLiveOsc::launchClipMsg"));
    msg.addIntArg(track);
    msg.addIntArg(clip);
    oscSender.sendMessage(msg);
    LOG_VERBOSE << "ofxLiveOsc::playClip sent " << msg.getAddress() << "(track: " << track << ", clip: " << clip << ")";
}

