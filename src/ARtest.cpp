//
//  ARtest.cpp
//  BaseApp
//
//  Created by Mark van de Korput on 06/04/15.
//
//

#include "ARtest.h"

#include "ofxSuperLog.h"
#include "ofxTimeMeasurements.h"
#include "ofxRemoteUIVars.h"


ARtest::ARtest(){
    width = 640;
    height = 480;
    threshold = -1;
    bEnabled = true;
    bMirror = true;
    plot1 = plot2 = plot3 =NULL;
}

ARtest::~ARtest(){
    destroy();
}

void ARtest::setupParams(){
    RUI_NEW_GROUP("ARtest");
    RUI_DEFINE_VAR_WV(bool, "artest-enabled", true);
    RUI_DEFINE_VAR_WV(int, "artest-treshold", 85, 0, 255);
    RUI_DEFINE_VAR_WV(bool, "artest-mirror", true);
    RUI_DEFINE_VAR_WV(float, "artest-cursor-accuracy", 0.001, 0.0001, 1.);
    RUI_DEFINE_VAR_WV(int, "artest-cursor-max", 5, 0, 1000);
}

void ARtest::updateParams(){
    int newTreshold = RUI_VAR(int, "artest-treshold");
    
    if(threshold != newTreshold){
        threshold = newTreshold;
        artk.setThreshold(threshold);
    }

    bEnabled = RUI_VAR(bool, "artest-enabled");
    bMirror = RUI_VAR(bool, "artest-mirror");
}

void ARtest::setup(){
    vidGrabber.initGrabber(width, height);
    colorImage.allocate(width, height);
    grayImage.allocate(width, height);
    grayThres.allocate(width, height);

    
    // This uses the default camera calibration and marker file
    artk.setup(width, height);

    // The camera calibration file can be created using GML:
    // http://graphics.cs.msu.ru/en/science/research/calibration/cpp
    // and these instructions:
    // http://studierstube.icg.tu-graz.ac.at/doc/pdf/Stb_CamCal.pdf
    // This only needs to be done once and will aid with detection
    // for the specific camera you are using
    // Put that file in the data folder and then call setup like so:
    // artk.setup(width, height, "myCamParamFile.cal", "markerboard_480-499.cfg");

    // Set the threshold
    // ARTK+ does the thresholding for us
    // We also do it in OpenCV so we can see what it looks like for debugging
    artk.setThreshold(threshold);
   
    plot1 = new ofxHistoryPlot(NULL, "dbg", 400, false);
    plot1->setAutoRangeShrinksBack(true);
    plot2 = new ofxHistoryPlot(NULL, "dbg", 400, false);
    plot2->setAutoRangeShrinksBack(true);
    plot3 = new ofxHistoryPlot(NULL, "dbg", 400, false);
    plot3->setAutoRangeShrinksBack(true);

    for(int i=0; i<5; i++){
        ofImage* img = new ofImage();
        img->loadImage("portrets/"+ofToString(i+1, 0, 2, '0')+".jpg");
        img->resize(150, 150);
        portrets.push_back(img);
    }

    // Load the corners of the image into the vector
    displayImageCorners.push_back(ofPoint(0, 0));
    displayImageCorners.push_back(ofPoint(150, 0));
    displayImageCorners.push_back(ofPoint(150, 150));
    displayImageCorners.push_back(ofPoint(0, 150));

    updateParams();
}

void ARtest::destroy(){
    #define DELPLOT(x) if(x){ delete x; x = NULL; }
    DELPLOT(plot1);
    DELPLOT(plot2);
    DELPLOT(plot3);
    #undef DELPLOT
}

void ARtest::update(float dt){
    if(!bEnabled)
        return;

    TS_START("artest U");
	vidGrabber.update();

    if(!vidGrabber.isFrameNew())
        return;

    colorImage.setFromPixels(vidGrabber.getPixels(), width, height);
    
    TS_START("conversion");
    // convert our camera image to grayscale
    grayImage = colorImage;
    if(bMirror)
        grayImage.mirror(false /* vertically */, true /* horizontally */);
    // apply a threshold so we can see what is going on
    grayThres = grayImage;
    grayThres.threshold(threshold);
    TS_STOP("conversion");

    TS_START("artk U");
    // Pass in the new image pixels to artk
    artk.update(grayImage.getPixels());
    TS_STOP("artk U");

    if(artk.getNumDetectedMarkers() > 0){
        ofVec3f pos = artk.getCameraPosition(0);
        plot1->update(pos.x);
        plot2->update(pos.y);
        float val = pos.x * RUI_VAR(float, "artest-cursor-accuracy");
        int curval = (int)val % RUI_VAR(int, "artest-cursor-max");
        cursor.set(curval);
        plot3->update(cursor.get()); //pos.z);
        // LOG << "cam pos: " << pos;
    } else {
        plot1->update(0.0);
        plot2->update(0.0);
        plot3->update(0.0);
    }

    TS_STOP("artest U");
}

void ARtest::draw(){
    if(!bEnabled)
        return;
    
    TS_START("artest D");

    ofImage* currentPortret = portrets[cursor.get() % portrets.size()];
    ofSetColor(255);
    currentPortret->draw(width, height);

    // Main image
    ofSetHexColor(0xffffff);
    grayImage.draw(width, 0);
    ofSetHexColor(0x666666);
    ofDrawBitmapString(ofToString(artk.getNumDetectedMarkers()) + " marker(s) found", width+10, 20);
    
    // Threshold image
    ofSetHexColor(0xffffff);
    grayThres.draw(0, 0);
    ofSetHexColor(0x666666);
    ofDrawBitmapString("Threshold: " + ofToString(threshold), 10, 20);

    // ARTK draw
    // An easy was to see what is going on
    // Draws the marker location and id number
    artk.draw(0, 0);

    // Find out how many markers have been detected
    int numDetected = artk.getNumDetectedMarkers();

    // ARTK 2D stuff
    // and draw blue corners on each marker
    for(int idx=0; idx<numDetected; idx++) {
        // Get the corners
        vector<ofPoint> corners;
        artk.getDetectedMarkerBorderCorners(idx, corners);
        // Can also get the center like this:
        // ofPoint center = artk.getDetectedMarkerCenter(myIndex);
        ofSetHexColor(0x0000ff);
        for(int i=0;i<corners.size();i++) {
            ofCircle(corners[i].x, corners[i].y, 10);
        }
        // Homography
        // Here we feed in the corners of an image and get back a homography matrix
        ofMatrix4x4 homo = artk.getHomography(idx, displayImageCorners);

        // We apply the matrix and then can draw the image distorted on to the marker
        ofPushMatrix();
        glMultMatrixf(homo.getPtr());
        // ofSetHexColor(0xffffff);
        ofSetColor(255,255,255,100);
        currentPortret->draw(0, 0);
        ofPopMatrix();
    }

    plot1->draw(0, height, width, 120);
    plot2->draw(0, height+120, width, 120);
    plot3->draw(0, height+240, width, 120);

    // ARTK 3D stuff
    // This is another way of drawing objects aligned with the marker
    // First apply the projection matrix once
    artk.applyProjectionMatrix();

    ofEnableAlphaBlending();
    // Draw for each marker discovered
    for(int i=0; i<numDetected; i++) {
        
        // Set the matrix to the perspective of this marker
        // The origin is in the middle of the marker
        artk.applyModelMatrix(i);
        
        // Draw a line from the center out
        ofNoFill();
        ofSetLineWidth(5);
        ofSetHexColor(0xffffff);
        //		glBegin(GL_LINES);
        //		glVertex3f(0, 0, 0);
        //		glVertex3f(0, 0, 50);
        //		glEnd();
        
        // Draw a stack of rectangles by offseting on the z axis
        ofNoFill();
        ofEnableSmoothing();
        ofSetColor(255, 255, 0, 50);	
        for(int i=0; i<10; i++) {		
            ofRect(-25, -25, 50, 50);
            ofTranslate(0, 0, i*1);
        }
    }

    TS_STOP("artest D");
}

void ARtest::drawRestoring(){
    glMatrixMode( GL_PROJECTION );
    ofPushView();

    // the line artk.applyProjectionMatrix(); messes up the
    // projection view (?), which has to be restored for further
    // any other components to be rendered normally, hence this confenience function
    draw();

    glMatrixMode( GL_PROJECTION );
    ofPopView();
}
