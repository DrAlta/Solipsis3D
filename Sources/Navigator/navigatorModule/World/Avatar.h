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

#ifndef __Avatar_h__
#define __Avatar_h__

#include "MainApplication/OgrePeer.h"
#include <CharacterInstance.h>
#include <IVoiceEngine.h>
#include "MainApplication/KeyMotion.h"
#include "OgreGraphicObjects/MovableText.h"
#include "Tools/Event.h"

#include "OgreGraphicObjects/SoundIcon.h"
#include "OgreGraphicObjects/ProgressBar.h"


using namespace Ogre;

namespace Solipsis {

/** This class represents an avatar.
*/
class Avatar : public OgrePeer, public IVoiceEngineAvatarHandler
{
public:
    /** Enumeration denoting the type of movement (rotate, straff, ...) */
    enum MvtType {
        MT1stPerson,
        MT3rdPerson
    };

protected:
    /// Updated entity
    RefCntPoolPtr<XmlEntity> mUpdatedXmlEntity;

    /// Default animation names
    static String mDefaultStateAnimName[ASAvatarAnimCount];
    /// Animation names
    String mStateAnimName[ASAvatarAnimCount];
    /// Current state
    AnimationState mState;
    /// Current movement type
    MvtType mMvtType;
    /// Last real position received
    Vector3 mLastRealPosition;
    /// Last real orientation received
    Quaternion mLastRealOrientation;

    /// Character
    CharacterInstance* mCharacterInstance;
    /// Scene node to attach cameras
    SceneNode* mCamerasSceneNode;
    /// Current animation state
    Ogre::AnimationState* mAnimationState;

    /// Name label
    MovableText* mNameLabel;
	/// Chat label
    MovableText* mChatLabel;
	/// Chat label alpha timer
    Real mChatLabelAlphaTimer;
    // soundIcon if needed
    SoundIcon* mSoundIcon; 
    /// Selection object
    ManualObject* mSelectionObject;
    /// Whether to apply the gravity
    bool mGravity;
    /// Voice min and max distances
    float mVoiceMinDist, mVoiceMaxDist;
    /// Ghost attribute
    bool mGhost;

private:
    /// Motion for Up key
    KeyMotion mUpKeyMotion;
    /// Motion for Down key
    KeyMotion mDownKeyMotion;
    /// Motion for Left key
    KeyMotion mLeftKeyMotion;
    /// Motion for Right key
    KeyMotion mRightKeyMotion;
    /// Motion for PgUp key
    KeyMotion mPgupKeyMotion;
    /// Motion for PgDown key
    KeyMotion mPgdownKeyMotion;

public:
    /** Constructor. */
    Avatar(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, CharacterInstance* characterInstance);

    /** Destructor. */
    virtual ~Avatar();

    /** Get the updated entity. */
    virtual RefCntPoolPtr<XmlEntity>& getUpdatedXmlEntity()
    {
        if (isLocal() && mUpdatedXmlEntity->getDefinedAttributes() & (
            XmlEntity::DAFlags |
            XmlEntity::DADisplacement |
            XmlEntity::DAPosition |
            XmlEntity::DAOrientation |
            XmlEntity::DAAnimation))
            return mUpdatedXmlEntity;

        return RefCntPoolPtr<XmlEntity>::nullPtr;
    }

    /** Get the character instance. */
    CharacterInstance* getCharacterInstance();
    /** Set the character instance. */
    void setCharacterInstance(CharacterInstance* characterInstance);
    /** Get the scene node. */
    inline SceneNode* getSceneNode() { return mCharacterInstance->getSceneNode(); }
    /** Get the scene Manager. */
    inline SceneManager* getSceneMgr() { return mCharacterInstance->getSceneMgr(); }
    /** Get the entity. */
    inline Entity* getEntity() { return mCharacterInstance->getEntity(); }

    /** Refresh extra nodes and movables according to the new character instance scene node. */
    void onSceneNodeChanged();
    /** Detach extra nodes and movables from the character instance scene node. */
    void detachFromSceneNode();

    /** Enable or Disable the selection box around avatar. (eg. disabled on 1st person camera) */
    void enableDisableSelection(bool enabled);

	/** Called when the avatar was saved. */
	void onAvatarSave();

    /** Set whether the name is visible or not. */
    void setNameVisibility(bool visible);

    /** Get the ghost attribute. */
    bool getGhost() { return mGhost; }
    /** Set ghost state. */
    void setGhost(bool ghost);

    /** Set the current state. */
    void setState(AnimationState state);
    /** Get the current state. */
    AnimationState getState();
    /** Set the animation name of 1 state. */
    void setStateAnimName(AnimationState state, const String& name);

    /** Set the current movement type. */
    void setMvtType(MvtType mvtType);
    /** Get the current movement type. */
    MvtType getMvtType();

    /** Set whether the gravity is applied or not. */
    void setGravity(bool enabled);
    /** Determines whether the gravity is applied or not. */
    bool isGravityEnabled();

    /** Set voice 3D sound min/max distances. */
    void setVoiceDistances(float minDist, float maxDist);
    /** Get voice 3D sound min/max distances. */
    void getVoiceDistances(float &minDist, float &maxDist);

    /** See OgrePeer. */
    virtual void update(Real timeSinceLastFrame);
    /** See OgrePeer. */
    virtual bool updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity);
    /** See OgrePeer. */
    virtual bool action(RefCntPoolPtr<XmlAction>& xmlAction);


    /** Starts 1 animation. */
    void startAnimation(const String &name, bool loop = true);
    /** Stop the current animation. */
    void stopAnimation();
    /** Animate. */
    void animate(Real timeSinceLastFrame);

    /** Compute movement according to key pressed. */
    void movementKeyPressed(KeyCode code);
    /** Compute movement according to key released. */
    void movementKeyReleased(KeyCode code);

    /** Rotate the avatar around the Y-axis. */
    void yaw(const Radian& angle);

    // from IVoiceEngine
    /** Called when avatar voice is created.
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
    virtual void onVoiceCreation()
    {
        mSoundIcon->addStatus(SoundIcon::SVisible);
    }

    /** Called when avatar voice is destroyed.
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
    virtual void onVoiceDestruction()
    {
          mSoundIcon->delStatus(SoundIcon::SVisible);
    };

    /** Called when avatar is talking or not.
    @param talking TRUE if avatar is speaking, FALSE otherwise
    @remarks An implementation must be supplied for this method to uniquely identify the engine.
    */
    virtual void onTalking(bool talking)
    {
        if (talking)
            mSoundIcon->addStatus(SoundIcon::SAnimated);
        else
            mSoundIcon->delStatus(SoundIcon::SAnimated);
    }

protected:
    /** Update the avatar properties into the voice engine. */
    void updateVoiceEngine(bool updatePosDirVel, bool updateDist);

public : 

	/** One bone information. */
	struct BoneInfo
	{
		BoneInfo():mBoneName(),mLocalPosition(Ogre::Vector3::ZERO),mLocalOrientation(Ogre::Quaternion::IDENTITY){}
		/// Bone position
		Ogre::Vector3 mLocalPosition;
		/// Bone orientation - parent based
		Ogre::Quaternion mLocalOrientation;
		/// Bone name.
		std::string mBoneName;
	};

	/** One skeleton informations. */
	struct SkeletonInfo
	{
		SkeletonInfo():mLocalPosition(Ogre::Vector3::ZERO),mLocalOrientation(Ogre::Quaternion::IDENTITY),mBones(){}
		/// Position of the entity.
		Ogre::Vector3 mLocalPosition;
		/// Orientation of the entity
		Ogre::Quaternion mLocalOrientation;
		/// The bones list.
		std::vector<BoneInfo> mBones;
	};
	
	// Conditions  : 
	//		=> Entity must have a skeleton
	//		=> Entity must be attached to a SceneNode that is not the RootSceneNode
	//		=> pSkeletonInfo's Quaternions has to be normalised.
	void manualUpdate(SkeletonInfo &pSkeletonInfo, Ogre::Entity& pEntity);
};

} // namespace Solipsis

#endif // #ifndef __Avatar_h__