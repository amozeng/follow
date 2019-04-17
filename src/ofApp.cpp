#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(0);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    
    ///---------- kinect v1
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // enable depth->video image calibration
    kinect.setRegistration(true);
    
    kinect.init();
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)
    
    kinect.open();        // opens first available kinect
    //kinect.open(1);    // open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");    // open a kinect using it's unique serial #
    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    
    /// ------- sound
    sound.load("/Users/amo/Documents/project/sound/tibetan_bell.aiff");
    
    /// ------ contour finder
    contourFinder.setMinAreaRadius(80);
    contourFinder.setMaxAreaRadius(1000);
    contourFinder.setSimplify(true);
    
    
    // Setup the gui.
    setupGui();

    /// ------- move
    moveX = 0;
    moveY = 0;
    
    /// test: random Particle
    totalModel = ofRandom(1,maxModel);
    for(int i = 0; i < totalModel; i++){
        glm::vec2 randomPos = glm::vec2(ofRandom(0,ofGetWidth()), ofRandom(0,ofGetHeight()));
        particleSystem.particles.push_back(std::make_unique<Particle>(randomPos));
    }
    
    /// model rotation offset
    for(int i = 0; i < totalModel; i++){
        rotationOffset.push_back(ofRandom(360));
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    kinect.update();
    
    /// contour finder update
    if (kinect.isFrameNew())
    {
        grayImage.setFromPixels(kinect.getDepthPixels());
        if(false){
            //            bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThresholds, true);
            grayThreshFar.threshold(farThresholds);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        } else {
            
            // or we do it ourselves - show people how they can work with the pixels
            ofPixels & pix = grayImage.getPixels();
            int numPixels = pix.size();
            
            rect.set(x,y,w,h);
            
            // Region of Interest
            for(std::size_t y= 0; y < grayImage.getHeight(); y++){
                for(std::size_t x = 0; x < grayImage.getWidth(); x++) {
                    if (!rect.inside(x, y))
                    {
                        pix.setColor(x, y, 255);
                    }
                }
            }
            
            for(int i = 0; i < numPixels; i++) {
                if(255 - pix[i] > nearThresholds && 255 - pix[i] < farThresholds) {
                    pix[i] = 255;
                } else {
                    pix[i] = 0;
                }
            }
        }
        
        ofPixels & pix = grayImage.getPixels();

        //ofxCv::convertColor(kinect, pix, CV_RGB2GRAY);
        //ofxCv::blur(pix, pix, blurLevel);
        //ofxCv::threshold(pix, pix, threshold, invert);
        ofxCv::erode(pix, pix, erodeIterations);
        ofxCv::dilate(pix, pix, dilateIterations);
        
        
        contourFinder.setMinAreaRadius(contourMinArea);
        contourFinder.setMaxAreaRadius(contourMaxArea);
        contourFinder.setThreshold(contourThreshold);
        contourFinder.setFindHoles(contourHoles);
        contourFinder.setSimplify(contourSimplify);
        contourFinder.findContours(pix);
        
        
        
    }
    
    grayImage.flagImageChanged();
    contourFinderKinect.findContours(grayImage, 10, (kinect.width * kinect.height)/2, 20, false);
    
    
    for (auto contourIndex = 0; contourIndex < contourFinder.size(); ++contourIndex)
    {
        
        ofPolyline contour = contourFinder.getPolylines()[contourIndex];
        ConvexHull convexHull(contour, hullMinumumDefectDepth);
        ofPolyline convexHullSmoothed = convexHull.convexHull();
        convexHullSmoothed.simplify(simplify);
        
    }
    
    if (!detectCountour && contourFinder.size() > 0)
    {
        sound.play();
    }
    
    if (contourFinder.size() > 0)
    {
        detectCountour = true;
        ofPolyline ourBiggestContour = contourFinder.getPolyline(0);
    }else{
        detectCountour = false;
    }

    
    
    
    
    /// impuse
    target = glm::vec2(controidPoint);
    //target = glm::vec2(ofGetWidth()/2, ofGetHeight()/2);
     //target = glm::vec2(ofGetMouseX(), ofGetMouseY());

    for (auto& p: particleSystem.particles)
    {
        /// update each particle, relate with gui
        p->modelScale = modelScale;
        p->particleDecSpeed = particleDecSpeed;
        p->lineDecSpeed = lineDecSpeed;
        p->particleSize = particleSize;
        /// problem: lineOpacity transition conflicts with gui
        /// attraction
        glm::vec2 dir = target - p->position;
        float d = ofDist(target.x, target.y, p->position.x, p->position.y);
        d = ofClamp(d,25.0,50.0);
        dir /= d ;
        float G = 0.4;
        float attractorMass = mass;
        float force = (G * attractorMass * p->mass) / (d * d);
        dir *= force;
        //float scaler = impuseScale;
        
        p->impulse = dir;
        
        
        /// each model disappear after too closed to target
        
        /// make sure each particle will die
        //if(opacityMin < 0) p->age = 101;
    }
    
    /// update particles
    particleSystem.update();
    
    /// add particle outside contour
//    if (particleSystem.particles.size() < totalModel)
//    {
//        std::size_t numNeeded = totalModel - particleSystem.particles.size();
//
//        std::vector<glm::vec2> newParticles = ofApp::getRandomPositionInsideOfPolyline(ourBiggestContour, numNeeded);
//
//
//        for (auto position: newParticles)
//        {
//            glm::vec2 velocity;
//
////            velocity.x = ofRandom(-0.3, 0.3);
////            velocity.y = ofRandom(-0.3, 0.3);
//            //                velocity.x = ofSignedNoise(time);
//            //                velocity.y = ofSignedNoise(time + 500);
//            Particle aNewParticle(position);
//            particles.push_back(aNewParticle);
//        }
//    }
    
    
    
}

void ofApp::reset(){
    particleSystem.particles.clear();
    
    totalModel = ofRandom(1,maxModel);
    for(int i = 0; i < totalModel; i++){
        glm::vec2 randomPos = glm::vec2(ofRandom(0,ofGetWidth()), ofRandom(0,ofGetHeight()));
        particleSystem.particles.push_back(std::make_unique<Particle>(randomPos));
    }
    
    /// model rotation offset
    for(int i = 0; i < totalModel; i++){
        rotationOffset.push_back(ofRandom(360));
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableDepthTest();
    //cam.begin();
    ofSetColor(255, 255, 255);
    
    
    if(showFourCam){
        ofPushMatrix();
        /// draw for kinect V1
        kinect.drawDepth(10, 10, 400, 300);
        kinect.draw(420, 10, 400, 300);
        grayImage.draw(10, 320, 400, 300);
        contourFinderKinect.draw(10, 320, 400, 300);
        ofPopMatrix();
    }

    /// draw particleSystem
    particleSystem.draw();
    

    
    /// ref: draw model wireframe
//    ofSetColor(255, 0, 0, 100);
//    model.drawWireframe();
    
    //cam.end();
    ofDisableDepthTest();
    
    if(drawInfo){
        drawGui();
        
        /// draw center
        ofSetColor(0, 255, 0, 200);
        ofDrawCircle(target, 50);
        //cout << "target: " << target << endl;
    }
    
    /// mapping the centroidPoint
    if(contourFinder.size() > 0) {
        ofPushMatrix();
//        ofScale(-1 * imageScale, -1 * imageScale, 1 * imageScale);
//        ofTranslate(theX, theY);
        centroid = contourFinder.getCentroid(0);
        controidPoint = ofxCv::toOf(centroid);
        controidPoint = ofVec2f((kinect.getWidth() * imageScale) - controidPoint.x * imageScale, (kinect.getHeight() * imageScale) - controidPoint.y * imageScale + theY -50);
        //ofTranslate(theX, theY);
        if(drawInfo){
            ofSetColor(255, 0, 0, 200);
            ofDrawCircle(controidPoint, 50);
        }

        curr = controidPoint;
        controidVel = curr - prev;
        controidVel.normalize();
        prev = curr;
        ofPopMatrix();
        
        

    }
    
    ofPushMatrix();
    ofScale(-1 * imageScale, -1 * imageScale, 1 * imageScale);
    ofTranslate(theX, theY);
    if(showContourFinder){
        ofSetColor(255);
        contourFinder.draw();
    }
    ofPopMatrix();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key) {
        case ' ':
            for (auto& p: particleSystem.particles)
            {
                p->goDie = !p->goDie;
                p->opacityDecrease = !p->opacityDecrease;
            }
            break;
        case 'r':
            reset();
            break;
        case 'i':
            drawInfo = !drawInfo;
//            for (auto& p: particleSystem.particles)
//            {
//                p->drawDir = !p->drawDir;
//            }
            break;
        case 'a':
            showFourCam = !showFourCam;
            break;
            
        case 'c':
            showContourFinder = !showContourFinder;
            break;
    }
}

std::vector<glm::vec2> ofApp::getRandomPositionOutsideContour(const ofPolyline& polyline, std::size_t howMany)
{
    std::vector<glm::vec2> results;
    ofRectangle boundBox = polyline.getBoundingBox();
    
    for (std::size_t i = 0; i < howMany; ++i)
    {
        glm::vec2 v;
        while (polyline.inside(v.x, v.y))
        {
            v.x = ofRandom(boundBox.getMinX(), boundBox.getMaxX());
            v.y = ofRandom(boundBox.getMinY(), boundBox.getMaxY());
        }
        results.push_back(v);
    }
    return results;
}

void ofApp::setupGui()
{
    // Set up the gui.
    guix = ofGetWidth() + 200;
    gui.setup();
    gui.setPosition(guix, 0);
    gui.add(rSpeed.setup("rSpeed", 34, 0, 100));
    gui.add(rDeg.setup("rDeg", 15, 0, 180));
    gui.add(rDegSpeed.setup("rDegSpeed", 2, 0.01, 2));
    gui.add(explodeSpeed.setup("explodeSpeed", 0.01, 0.001, 0.01));
    gui.add(particleSize.setup("particleSize", 0.03, 0.01, 0.20));
    gui.add(modelScale.setup("modelScale", 20, 0, 100));
    gui.add(lineDecSpeed.setup("lineDecSpeed", 7, 1, 10));
    gui.add(particleDecSpeed.setup("particleDecSpeed", 5, 0, 10));
    //gui.add(speedX.setup("speedX", 0, -5, 5));
    //gui.add(speedY.setup("speedY", 0, -5, 5));
    gui.add(impuseScale.setup("impuseScale", 2, 1, 50));
    gui.add(mass.setup("mass", 100, 1, 1000));
    gui.add(headDeg.setup("headDeg", 0, 0, 360));
    gui.add(lineOpacity.setup("lineOpacity", 100, 0, 255));

    
    guiFinder.setup();
    guiFinder.setPosition(guix, gui.getHeight());
    guiFinder.add(finderThreshold.set("Threshold", 60, 0, 255));
    guiFinder.add(finderTrackHueSaturation.set("Track Hue/Saturation", false));
    guiFinder.add(finderTargetColor.set("Track Color", ofColor(38, 23, 19)));
    guiFinder.add(simplify.set("simplify", 20.0, 0.0, 40.0));
    
    cvGui.setup();
    cvGui.setPosition(guix, gui.getHeight() + guiFinder.getHeight());
    cvGui.add(blurLevel.set("Blur Level", 17, 0, 30));
    cvGui.add(threshold.set("Threshold", 93, 0, 255));
    cvGui.add(erodeIterations.set("Erode Iterations", 1, 0, 30));
    cvGui.add(dilateIterations.set("Dilate Iterations", 7, 0, 30));
    cvGui.add(invert.set("Invert", false));
    cvGui.add(contourMinArea.set("Contour Min Area", 10, 1, 100));
    cvGui.add(contourMaxArea.set("Contour Max Area", 200, 1, 500));
    cvGui.add(contourThreshold.set("Contour Threshold", 128, 0, 255));
    cvGui.add(contourHoles.set("Contour Holes", false));
    cvGui.add(contourSimplify.set("Contour Simplify", true));
    
    kinectGUI.setup();
    kinectGUI.setPosition(guix, gui.getHeight() + guiFinder.getHeight() + cvGui.getHeight());
    kinectGUI.setName("kinect");
    kinectGUI.add(nearThresholds.set("nearThresholds", 30, 0, 150));
    kinectGUI.add(farThresholds.set("farThresholds", 210, 0, 255));
    kinectGUI.add(x.set("x", 20, 0, 400));
    kinectGUI.add(y.set("y", 0, 0, 400));
    kinectGUI.add(w.set("w", 600, 0, 1500));
    kinectGUI.add(h.set("h", 300, 0, 800));
    
    mapping.setup();
    mapping.setName("mapping");
    mapping.setPosition(guix, gui.getHeight() + guiFinder.getHeight() + cvGui.getHeight() + kinectGUI.getHeight());
    mapping.add(theX.set("theX", -650, -1000, 1000));
    mapping.add(theY.set("theY", -320, -1000, 1000));
    mapping.add(imageScale.set("imageScale", 2.665, 1.00, 10.00));
}


void ofApp::drawGui()
{
    guiFinder.draw();
    cvGui.draw();
    gui.draw();
    mapping.draw();
    kinectGUI.draw();
}
