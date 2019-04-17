// Copyright (c) 2017 Christopher Baker <https://christopherbaker.net>
// SPDX-License-Identifier:    MIT

#pragma once
#include "ofTypes.h"
#include "ofUtils.h"
#include "ofxCv.h"
#include "ofxAssimpModelLoader.h"
#include "of3dGraphics.h"


using namespace std;


class Particle
{
public:
    Particle();
    Particle(const glm::vec2& position);
    Particle(const glm::vec2& position, const glm::vec2& velocity);
    
    virtual ~Particle();
    
    virtual void reset();
    virtual void update();
    virtual void draw() ;
    virtual bool isDead() const;
    void loadRandomModel(int randomNum);
    
    /// ------- with ofApp gui
    float particleSize = 0.02;

    
    /// ------- control
    bool dead = false;
    float scale = 1000;
    bool drawDir = false;
    
    /// ------- model setup
    ofxAssimpModelLoader model;
    /// ------- mesh setup
    ofMesh mesh, mesh2;
    vector<ofMesh> meshes;
    int numMeshes;
    vector< vector <glm::vec3> > vertices;
    vector<glm::vec3> speed;
    int totalNumVertices;
    /// -------- each particle dot
    float randomOpacity, particleOpacity;
    bool goDie = false;
    bool opacityDecrease = false;
    float opacityMin = 100;
    float opacityMax = 255;
    float lineOpacity = 50;
    
    float explodeSpeed = 0.01;
    float modelScale = 10;
    float particleDecSpeed = 0.7;
    float lineDecSpeed = 7;
    
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec2 gravity;
    glm::vec2 impulse;
    float drag = 1;
    glm::vec2 oldvel;
    vector<glm::vec2> velHis;
    
    uint64_t age = 0;
    float ageSpeed = 0;
    float ageOffset = 0;
    
    float opcDecreaseSpeed = 0;
    float opacity = 255;
    
    /// attraction
    float mass;
    float heading2D;
    
    
    //------------ ofSignedNoise setup ----------//
    float time;
    float offsetX;
    float offsetY;
};

