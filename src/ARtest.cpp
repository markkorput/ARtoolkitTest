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
}

// ARtest::~ARtest(){}

void ARtest::setupParams(){
    RUI_NEW_GROUP("ARtest");
    RUI_DEFINE_VAR_WV(bool, "artest-enabled", true);
    RUI_DEFINE_VAR_WV(int, "artest-treshold", 85, 0, 255);
    RUI_DEFINE_VAR_WV(bool, "artest-mirror", true);
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

    // Load the image we are going to distort
    displayImage.loadImage("of.jpg");
    // Load the corners of the image into the vector
    int displayImageHalfWidth = displayImage.width / 2;
    int displayImageHalfHeight = displayImage.height / 2;
    displayImageCorners.push_back(ofPoint(0, 0));
    displayImageCorners.push_back(ofPoint(displayImage.width, 0));
    displayImageCorners.push_back(ofPoint(displayImage.width, displayImage.height));
    displayImageCorners.push_back(ofPoint(0, displayImage.height));
    
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
   
    // ofBackground(127,127,127);
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
    TS_STOP("artest U");
}

void ARtest::draw(){
    if(!bEnabled)
        return;
    
    TS_START("artest D");

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

    // ARTK 2D stuff
    // See if marker ID '0' was detected
    // and draw blue corners on that marker only
    int myIndex = artk.getMarkerIndex(0);
    if(myIndex >= 0) {
        // Get the corners
        vector<ofPoint> corners;
        artk.getDetectedMarkerBorderCorners(myIndex, corners);
        // Can also get the center like this:
        // ofPoint center = artk.getDetectedMarkerCenter(myIndex);
        ofSetHexColor(0x0000ff);
        for(int i=0;i<corners.size();i++) {
            ofCircle(corners[i].x, corners[i].y, 10);
        }
        // Homography
        // Here we feed in the corners of an image and get back a homography matrix
        ofMatrix4x4 homo = artk.getHomography(myIndex, displayImageCorners);
        // We apply the matrix and then can draw the image distorted on to the marker
        ofPushMatrix();
        glMultMatrixf(homo.getPtr());
        ofSetHexColor(0xffffff);
        displayImage.draw(0, 0);
        ofPopMatrix();
    }
  
        
    // ARTK 3D stuff
    // This is another way of drawing objects aligned with the marker
    // First apply the projection matrix once
    artk.applyProjectionMatrix();

    // Find out how many markers have been detected
    int numDetected = artk.getNumDetectedMarkers();
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
