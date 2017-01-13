/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#ifndef __Scene_h__
#define __Scene_h__

#include "MainApplication/OgrePeer.h"
// from ogre max
#include "OgreMaxScene.hpp"

#define NO_OGREMAX_STATIC_GEOM          1
#define USE_OGREMAX_SHADOWPARAM         0

using namespace Ogre;

namespace Solipsis {

/** This class represents a 3D scene.
 */
class Scene : public OgrePeer
{
protected:
    /// Resource group name
    String mResourceGroup;
    /// Resource location
    String mResourceLocation;
    /// Scene node
    SceneNode* mSceneNode;
    /// Converted static geometry (optimized scene geometry)
    StaticGeometry* mStaticGeometry;
    /// OgreMax scene
    OgreMax::OgreMaxScene* mOgreMaxScene;

public:
    Scene(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal);

    virtual ~Scene();

    /** See OgrePeer. */
    virtual void update(Real timeSinceLastFrame);
    /** See OgrePeer. */
    virtual bool updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity);
    /** See OgrePeer. */
    virtual bool action(RefCntPoolPtr<XmlAction>& xmlAction);

protected:
    void destroy();
    void convertToStaticGeometry(SceneNode* sceneNode);

#if 1 // GILLES
    void scanSceneNode(SceneNode* pSceneNode);
    std::vector<Ogre::AnimationState*> mVertexAnimationStates;
#endif
};

} // namespace Solipsis

#endif // #ifndef __Scene_h__