/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2010-2011 Metaverse1 (Artefacto, LORIA)
Author ARTEFACTO - Gilles GAUDIN

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Emotion.h"
#include <CTLog.h>

using namespace Solipsis;

//---------------------------------------------------------------------------------
Emotion::Emotion( std::string type, Ogre::AnimationState* animation, unsigned long start, unsigned long duration, unsigned long timeToUp, unsigned long timeToDown ):
    mType(type), 
    mAnimation(animation),
    mDuration(duration), 
    mStartTime(start), 
    mTimeToUp(timeToUp), 
    mTimeToDown(timeToDown), 
    mIsFinish(false) 
{
    // ...
};

//---------------------------------------------------------------------------------
Emotion::~Emotion() 
{
    // ...
};

//---------------------------------------------------------------------------------
void Emotion::update(unsigned long elapsedTime)
{
    unsigned long dt = elapsedTime - mStartTime;

    if (! mAnimation || (elapsedTime > (mStartTime + mDuration)) )
    {
        mIsFinish = true;
        mAnimation->setWeight( 0. );
        mAnimation->setEnabled( false );
    }
    else
    {
        float weight = 0.;
        if (dt < mTimeToUp)                         // timeToUp
        {
            weight = 1. * dt / mTimeToUp;
        }
        else if (dt > (mDuration - mTimeToDown))    // timeToDown
        {
            weight = 1.0 - (1. *(dt - (mDuration - mTimeToDown)) / mTimeToDown);
        }
        else                                        // ideal time
        {
            weight = 1.0;
        }

        // update the pose
        //char temp[256];
        //sprintf( temp, "######### %s : %ul -> %f", mAnimation->getAnimationName().c_str(), dt, weight );
        //Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage( temp );
        mAnimation->setEnabled( true );
        mAnimation->setWeight( weight );
    }
}

//---------------------------------------------------------------------------------
bool Emotion::isFinish() 
{
    return mIsFinish;
}

//---------------------------------------------------------------------------------
void Emotion::forceToFinish() 
{
    if(mDuration > mTimeToDown) mDuration = mTimeToDown; 
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
