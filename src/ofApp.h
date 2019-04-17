#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxGui.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "ofxCv.h"
#include "CvUtils.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void reset();
		void keyPressed(int key);
        void setupGui();
        void drawGui();
    
    /// ------- kinect
    ofxKinect kinect;
    float kinectWidth = 320/2;
    float kinectHeight = 240/2;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage; // grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    ofRectangle rect;

    ///------- control
    bool drawInfo = false;
    bool showFourCam = true;
    bool showContourFinder = false;
    
    /// ------- camera
    ofEasyCam cam;
    


    /// ------- random model
    bool isPressed = false;
    /// -------
    float moveX, moveY;
    float speedX, speedY;
    std::vector<float> rotationOffset;
    
    /// ------- each model on particle
    std::vector<std::unique_ptr<Particle>> particles;
    ParticleSystem particleSystem;
    std::vector<glm::vec2> getRandomPositionOutsideContour(const ofPolyline& polyline, std::size_t howMany);
    int totalModel = 5;
    int maxModel = 7;
    /// ------- attraction
    glm::vec2 target;
    cv::Point2f centroid;
    ofVec2f controidPoint;
    ofVec2f prev, curr;
    ofVec2f controidVel;
    
    /// ------- contour finder
    ofSoundPlayer sound;
    bool detectCountour = false;
    
    ofxCv::ContourFinder contourFinder;
    ofxCvContourFinder contourFinderKinect;
    
    //--------------- assignment 01
    float hullMinumumDefectDepth = 10;
    
    ConvexHull convexHull;
    ofPolyline contour;
    ofPolyline convexHullSmoothed;
    /// control:
    bool isfin  = false;
    
    /// ------- gui
    ofxPanel gui;
    ofxFloatSlider rSpeed, rDeg, rDegSpeed, headDeg;
    ofxFloatSlider explodeSpeed, particleSize, modelScale;
    ofxFloatSlider lineDecSpeed, particleDecSpeed;
    //ofxFloatSlider speedX, speedY;
    ofxFloatSlider impuseScale, mass;
    ofxFloatSlider lineOpacity;
    /// -------- cv gui
    ofxPanel guiFinder;
    ofParameter<float> finderThreshold, simplify;
    ofParameter<bool> finderTrackHueSaturation;
    ofParameter<ofColor> finderTargetColor;
    
    ofxPanel cvGui;
    ofParameter<float> blurLevel;
    ofParameter<int> threshold;
    ofParameter<bool> invert;
    ofParameter<int> erodeIterations;
    ofParameter<int> dilateIterations;
    ofParameter<float> contourMinArea;
    ofParameter<float> contourMaxArea;
    ofParameter<float> contourThreshold;
    ofParameter<bool> contourHoles;
    ofParameter<bool> contourSimplify;

    ofxPanel mapping;
    ofParameter<float> theX, theY;
    ofParameter<float> imageScale;
    ofParameter<float> rotateDeg;
    
    ofxPanel kinectGUI;
    ofParameter<float> nearThresholds, farThresholds;
    ofParameter<float> x, y, w, h;
    // controuFinder GUI
    
    float guix;
    

    
};
