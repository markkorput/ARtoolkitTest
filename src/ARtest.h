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

#include "ofxHistoryPlot.h"
#include "sattribute.h"

class ARtest {

public: // con-/destructors
    ARtest();
    ~ARtest();

public: // methods
    void setupParams();
    void updateParams();
    void setup();
    void destroy();
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

    /* The four corners of the image */
    vector<ofPoint> displayImageCorners;
    bool bEnabled, bMirror;

    ofxHistoryPlot *plot1, *plot2, *plot3;
    Sattribute<int> cursor;
    vector<ofImage*> portrets;
};

#endif /* defined(__BaseApp__ARtest__) */
