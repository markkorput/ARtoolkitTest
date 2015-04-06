//
//  ARtest.h
//  BaseApp
//
//  Created by Mark van de Korput on 06/04/15.
//
//

#ifndef __BaseApp__ARtest__
#define __BaseApp__ARtest__


#include "ofxOpenCv.h"
#include "ofxARToolkitPlus.h"

#include "ofMain.h"


class ARtest {

public: // con-/destructors
    ARtest();
    // ~ARtest(){}

public: // methods
    void setupParams();
    void updateParams();
    void setup();
    void update(float dt);
    void draw();
    void drawRestoring();

protected: // attributes
    /* Size of the image */
    int width, height;
    
    /* Use either camera or a video file */
    ofVideoGrabber vidGrabber;
    
    /* ARToolKitPlus class */
    ofxARToolkitPlus artk;
    int threshold;
    
    /* OpenCV images */
    ofxCvColorImage colorImage;
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage	grayThres;
    
    /* Image to distort on to the marker */
    ofImage displayImage;
    /* The four corners of the image */
    vector<ofPoint> displayImageCorners;
    bool bEnabled, bMirror;
};

#endif /* defined(__BaseApp__ARtest__) */
