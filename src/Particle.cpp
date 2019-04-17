// Copyright (c) 2017 Christopher Baker <https://christopherbaker.net>
// SPDX-License-Identifier:    MIT

#include "Particle.h"
#include "ofGraphics.h"

Particle::Particle()
{
    offsetX = ofRandom(0,1000);
    offsetY = ofRandom(0,1000);
    age = ofRandom(0,80);
    mass = ofRandom(1,3);
    int randomNum = (int)ofRandom(1,7);
    cout << "randomNum: " << randomNum << endl;
    loadRandomModel(randomNum);
    
    /// ------- mesh setup
    numMeshes = model.getNumMeshes();
    for(int i = 0; i < numMeshes; i++){
        meshes.push_back(model.getCurrentAnimatedMesh(i));
    }
    for(int i = 0; i < numMeshes; i++){
        vertices.push_back(meshes[i].getVertices());
        totalNumVertices += meshes[i].getNumVertices();
    }
    //totalNumVertices = vertices.size();
    cout << "totalNumVertices: " << totalNumVertices;
    for (int i = 0; i < totalNumVertices; ++i) {
        speed.push_back(glm::vec3(ofRandom(-explodeSpeed,explodeSpeed), ofRandom(-explodeSpeed,explodeSpeed), ofRandom(-explodeSpeed,explodeSpeed)));
    }
    
    randomOpacity = ofRandom(200, 255);
}


Particle::Particle(const glm::vec2& _position): position(_position)
{
    offsetX = ofRandom(0,1000);
    offsetY = ofRandom(0,1000);
    age = ofRandom(0,80);
    mass = ofRandom(1,3);
    int randomNum = (int)ofRandom(1,7);
    cout << "randomNum: " << randomNum << endl;
    loadRandomModel(randomNum);
    
    /// ------- mesh setup
    numMeshes = model.getNumMeshes();
    for(int i = 0; i < numMeshes; i++){
        meshes.push_back(model.getCurrentAnimatedMesh(i));
    }
    for(int i = 0; i < numMeshes; i++){
        vertices.push_back(meshes[i].getVertices());
        totalNumVertices += meshes[i].getNumVertices();
    }
    //totalNumVertices = vertices.size();
    cout << "totalNumVertices: " << totalNumVertices;
    for (int i = 0; i < totalNumVertices; ++i) {
        speed.push_back(glm::vec3(ofRandom(-explodeSpeed,explodeSpeed), ofRandom(-explodeSpeed,explodeSpeed), ofRandom(-explodeSpeed,explodeSpeed)));
    }
    
    randomOpacity = ofRandom(200, 255);
}


Particle::Particle(const glm::vec2& _position, const glm::vec2& _velocity):
position(_position),
velocity(_velocity)
{
    offsetX = ofRandom(0,1000);
    offsetY = ofRandom(0,1000);
    age = ofRandom(0,80);
    mass = ofRandom(0.5,0.9);
    int randomNum = (int)ofRandom(1,7);
    cout << "randomNum: " << randomNum << endl;
    loadRandomModel(randomNum);
    
    /// ------- mesh setup
    numMeshes = model.getNumMeshes();
    for(int i = 0; i < numMeshes; i++){
        meshes.push_back(model.getCurrentAnimatedMesh(i));
    }
    for(int i = 0; i < numMeshes; i++){
        vertices.push_back(meshes[i].getVertices());
        totalNumVertices += meshes[i].getNumVertices();
    }
    //totalNumVertices = vertices.size();
    cout << "totalNumVertices: " << totalNumVertices;
    for (int i = 0; i < totalNumVertices; ++i) {
        speed.push_back(glm::vec3(ofRandom(-explodeSpeed,explodeSpeed), ofRandom(-explodeSpeed,explodeSpeed), ofRandom(-explodeSpeed,explodeSpeed)));
    }
    
    randomOpacity = ofRandom(200, 255);
}


Particle::~Particle()
{
}



void Particle::update()
{
    age += ageOffset;
    age = age + ageSpeed;

    
    /// model transition
    if(goDie){
        for(int j = 0; j < numMeshes; j++)
        {
            for (int i = 0; i < vertices[j].size(); i++)
            {
                vertices[j][i] = vertices[j][i] + speed[i];
            }
        }
        for(int j = 0; j < numMeshes; j++){
            vector<glm::vec3> currVertices = meshes[j].getVertices();
            int num = meshes[j].getNumVertices();
            for(int i = 0; i < num; i++){
                meshes[j].setVertex(i, vertices[j][i]);
            }
        }
        
    }else{
        model.update();
        //        meshes.clear();
        //        numMeshes = model.getNumMeshes();
        for(int i = 0; i < numMeshes; i++){
            meshes[i] = model.getCurrentAnimatedMesh(i);
        }
        for(int i = 0; i < numMeshes; i++){
            vertices[i] = meshes[i].getVertices();
        }
    }
    
    /// opacity update:
    if(opacityDecrease){
        if(opacityMin > 0) {
            opacityMin -= particleDecSpeed;
        }
        if(opacityMax > 0) {
            opacityMax -= particleDecSpeed;
        }
        if(lineOpacity > 0){
            lineOpacity -= lineDecSpeed;
        }
        if(lineOpacity < 0){
            lineOpacity = 0;
        }
    }
}

void Particle::reset(){
    totalNumVertices = 0;
    /// ------- mesh setup
    numMeshes = model.getNumMeshes();
    for(int i = 0; i < numMeshes; i++){
        meshes.push_back(model.getCurrentAnimatedMesh(i));
    }
    for(int i = 0; i < numMeshes; i++){
        vertices.push_back(meshes[i].getVertices());
        totalNumVertices += meshes[i].getNumVertices();
    }
    //totalNumVertices = vertices.size();
    cout << "totalNumVertices: " << totalNumVertices << endl;
    for (int i = 0; i < totalNumVertices; ++i) {
        speed.push_back(glm::vec3(ofRandom(-explodeSpeed,explodeSpeed), ofRandom(-explodeSpeed,explodeSpeed), ofRandom(-explodeSpeed,explodeSpeed)));
    }
    goDie = false;
    opacityDecrease = false;
    opacityMin = 100;
    opacityMax = 255;
    lineOpacity = 100;
}

void Particle::draw()
{
    /// red circle as position ref
    /// rotate toward attractor
    velocity += (gravity + impulse);
    impulse *= 0.0;
    //velocity *= drag;
    position += velocity;
    ofPushMatrix();
    ofTranslate(position);
//    cout << "velHis[0]" << velHis[0] << endl;
    float angle = (float)atan2(-velocity.y, velocity.x);
    
    float theta =  -1.0 * angle;
    heading2D = ofRadToDeg(theta);
    
    if(drawDir) {
        ofRotateDeg(heading2D, 0, 0, 1);
        ofSetColor(255, 0, 0, 200);
        ofDrawCircle(0,0 , 10);
        ofLine(0,0,100,0);
    }
    ofPopMatrix();

    
        ofPushMatrix();
        ofTranslate(position);
        ofScale(modelScale);
        ofRotateDeg(90, 1, 0, 0);
        //ofRotateDeg(180, 1, 0, 0);

        ofRotateDeg(heading2D, 0, 1, 0);
        
        //cout << "heading2D " << i <<" "<< rot << endl;
        float d = ofMap(sin(ofGetElapsedTimef() * 2) , -1, 1, -15, 15);
        float dx = ofMap(sin(ofGetElapsedTimef() * 2) /*+ rotationOffset[i]*/, -1, 1, -15, 15);
        ofRotateDeg(d, 0, 1, 0);
        ofRotateDeg(dx, 1, 0, 0);
        ofRotateDeg(dx, 0, 0, 1);
        
        for(int j = 0; j < numMeshes; j++){
            ofPushMatrix();
            ofRotateDeg(90, 0, 1, 0);
            /// draw particle
            for (int i = 0; i < vertices[j].size(); i++)
            {
                randomOpacity = ofRandom(opacityMin, opacityMax);
                if(goDie){
                    
                    vertices[j][i] = vertices[j][i] + speed[i];
                }
                ofPushMatrix();
                ofSetColor(255, 255, 255, randomOpacity);
                //ofTranslate(x, y, z);
                //ofDrawCircle(vertices[j][i], 0.02);
                ofDrawSphere(vertices[j][i], particleSize);
                ofPopMatrix();
            }
            /// draw wireframe
            ofSetColor(255, 255, 255, lineOpacity);
            meshes[j].drawWireframe();
            ofPopMatrix();
        }
        ofPopMatrix();
    
    
    velocity = glm::vec2(ofSignedNoise(time + offsetX), ofSignedNoise(time + offsetY));
}



bool Particle::isDead() const
{
    return age > 100 || opacity <= 0;
}

void Particle::loadRandomModel(int randomNum){
    if(randomNum == 1){
        model.loadModel("/Users/amo/Documents/project/3d/Bird.dae", 20);
    }else if (randomNum == 2){
        model.loadModel("/Users/amo/Documents/project/3d/Bird.dae", 20);
    }else if (randomNum == 3){
        model.loadModel("/Users/amo/Documents/project/3d/Chick.dae", 20);
    }else if (randomNum == 4){
        model.loadModel("/Users/amo/Documents/project/3d/Whale.dae", 20);
    }else if (randomNum == 5){
        model.loadModel("/Users/amo/Documents/project/3d/Piranha.dae", 20);
    }else if (randomNum == 6){
        model.loadModel("/Users/amo/Documents/project/3d/Bird.dae", 20);
    }
    model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
    model.playAllAnimations();
    if(model.getNumMeshes() > 0){
        reset();
    }
}
