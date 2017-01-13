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


#ifndef EMOTION_H
#define EMOTION_H

#include <Ogre.h>
#include <OgreAnimationState.h>
#include <list>
#include "ManualPoseV2.h"

namespace Solipsis {

    // the Emotion class
    class Emotion 
    {
    public:
        Emotion( std::string type, Ogre::AnimationState* animation, unsigned long start, unsigned long duration, unsigned long timeToUp, unsigned long timeToDown );
        ~Emotion();

        /// Update the current pose animation
        void update(unsigned long elapsedTime);
        /// Get the pose animation's state
        bool isFinish();
        /// Force to 0 the current pose animation
        void forceToFinish();

    private:
		/// [not used for the moment]
        std::string mType;                          
        /// the duration before the current pose animation should be finish
		unsigned long mDuration;                    
		/// ...
        unsigned long mTimeToUp;                    
		/// ...
        unsigned long mTimeToDown;                  
		/// the start time of the current pose animation
        unsigned long mStartTime;                   
		/// the pose animation's state
        bool mIsFinish;                             
		/// the current pose animation pointer
        Ogre::AnimationState* mAnimation;           
    };
}

#endif // EMOTION_H