//
//  ofxLiveOsc.h
//  BaseApp
//
//  Created by Mark van de Korput on 07/04/15.
//
//

#ifndef __BaseApp__ofxLiveOsc__
#define __BaseApp__ofxLiveOsc__

#include "ofMain.h"
#include "ofxOsc.h"

#define LIVEOSC ofxLiveOsc::one()

class ofxLiveOsc {

public: // con-destructors
    ofxLiveOsc();
    ~ofxLiveOsc();

public: // singleton instance stuff; generally only one instance of this class is needed
        // this instance can be automatically created and its reference an be
        // obtained by calling ofxLiveOsc::one()
    static ofxLiveOsc& one()
    {
        static ofxLiveOsc instance;
        // Instantiated on first use.
        return instance;
    }

public: // default interface methods
    void setupParams();
    void updateParams();
    void setup();
    void update(float dt);
    void draw();

public: // operation methods
    void playClip(unsigned int track=0, unsigned int clip=0);
    
protected: // attributes
    static bool bClassParamsCreated;
    string url;
    int port;
    ofxOscSender oscSender;
    
};

#endif // defined(__BaseApp__ofxLiveOsc__)
