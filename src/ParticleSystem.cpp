//
// Copyright (c) 2017 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ParticleSystem.h"


ParticleSystem::ParticleSystem()
{
}


ParticleSystem::~ParticleSystem()
{
}


void ParticleSystem::update()
{
    std::vector<std::unique_ptr<Particle>>::iterator iter = particles.begin();
    
    while (iter != particles.end())
    {
        (*iter)->update();
        
        if ((*iter)->isDead())
        {
            iter = particles.erase(iter);
        }
        else
        {
            iter = iter + 1;
        }
    }
}


void ParticleSystem::draw() const
{
    for (const auto& particle: particles)
    {
        particle->draw();
    }
}
