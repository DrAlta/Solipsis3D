/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2010-2011 Metaverse1 (Artefacto, LORIA)
Author ARTEFACTO - Pierre MORCELLO

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


#ifndef MANUALPOSEV2_H
#define MANUALPOSEV2_H

#include <Ogre.h>
#include <OgreAnimationState.h>
#include <list>
/// \brief Manual Pose class
/// 1 ManualPose is for 1 entity
/// Use of the class 
///				:	1/ initialiseManualPoseFor(meshquejeeuxinitialiser, nomduressourcegroup)
///					2/ mp = new ManualPose();
///					3/ mp->initialise( monEntitee, true); // true pour arrete les autres animationstate
///					4/ mp->getAllPoseName();
///					5/ mp->updatePose(nomdemapose, 0.3);// wheight.
///					6/ delete mp; mp = NULL;
/// Rq:  A simple way to use this class :
/// initialiseManualPosesForMeshesInRessourcesGroups(nameduresourcegroup)
class ManualPoseV2
{
	//--------------------------------------------------------------------------------
	//-----------------------------------Static informations   -----------------------
	//--------------------------------------------------------------------------------

	/// \brief structure to know usable manual poses.
	struct ManualPoseIsPossible
	{
		///\brief Name of the mesh
		std::string mMeshName;

		///\brief Information about one pose
		struct InfoFor1Pose
		{
			std::string mName; // Name
			std::string mAnimationName; // Animation name
			std::map<int, Ogre::VertexPoseKeyFrame*> mVertexPoseKeyFrameMap; // Poses map
		};

		typedef std::vector<InfoFor1Pose> InfosForNPoses;
		///\brief Le pose list 
		InfosForNPoses mPosesInfo;
	};

	static std::list<ManualPoseIsPossible> sAllPoses;

	/// \brief Pointer to the Ogre entity
	Ogre::Entity* mEntity;

public:

	ManualPoseV2(void);
	~ManualPoseV2(void);	
	
	///\brief Initialize  the animationState 
	bool initialise(Ogre::Entity * ent, bool stopOtherAnimationStates = true);

	///\brief Initialize  one animationstate
	bool initialise(const std::string& pPoseName);

	std::vector<std::string> getAllPoseAnimationStateNames();
	std::vector<std::string> getAllPoseName();

	/// \brief Stop all animation states
	void stopAllAnimationStateFor(Ogre::Entity* pEnt);

	/// \brief Initialize a manual mesh for an unloaded ressource group!
	static void initialiseManualPoseFor(const std::string& pMeshName, const std::string& pResourceGroupName);
	
	///\brief Initialize a manual mesh already loaded
	static void initialiseManualPoseFor(Ogre::Mesh& pMesh);

	static void initialiseManualPosesForMeshesInRessourcesGroups(const std::string& pResourceGroupName);

	///\brief Update weigth
	void updatePose(const std::string& pPoseName, float pWeight);

	static std::string getAnimationStateNameFromPoseName(const std::string& pPoseName);
};

#endif

