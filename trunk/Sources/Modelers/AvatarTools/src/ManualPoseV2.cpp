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

#include "ManualPoseV2.h"
using namespace Ogre;

std::list<ManualPoseV2::ManualPoseIsPossible> ManualPoseV2::sAllPoses;

ManualPoseV2::ManualPoseV2(void)
{
}

ManualPoseV2::~ManualPoseV2(void)
{
}

void ManualPoseV2::stopAllAnimationStateFor(Ogre::Entity * pEnt)
{
	if(pEnt)
	{
		AnimationStateSet* animsset = pEnt->getAllAnimationStates();
		AnimationStateIterator iterAnim = animsset->getAnimationStateIterator();
		for(;iterAnim.hasMoreElements();)
		{
			Ogre::AnimationState * lAnimState = iterAnim.getNext();
			if(lAnimState)
			{
				lAnimState->setEnabled(false);
				lAnimState->setLoop(false);
				lAnimState->setTimePosition(0.0);
			}
		}
	}
}
std::string ManualPoseV2::getAnimationStateNameFromPoseName(const std::string& pPoseName)
{
	return pPoseName;
}

bool ManualPoseV2::initialise(Ogre::Entity * pEnt, bool pStopOtherAnimationStates)
{
	if(!pEnt) {return false;}
	Ogre::LogManager::getSingleton().logMessage("[ManualPoseV2::initialise] initialize for our entity.");
	mEntity = pEnt;// obligatoire pour l'utilisation des fonctions membres ensuite.
	if(pStopOtherAnimationStates)
	{
		stopAllAnimationStateFor(pEnt);
	}

	bool success = true;
	std::vector<std::string> lNameAnimationStates = getAllPoseAnimationStateNames();
	unsigned int lAnimationsCount = lNameAnimationStates.size();
	if(lAnimationsCount)
	{
		for(unsigned int i = 0; i < lNameAnimationStates.size(); i++)
		{
			// TODO : ATTENTION  ici utiliser le nom de la pose...
			bool success2 = initialise(lNameAnimationStates[i]);
			success = success && success2;
		}
	}else{
		success = false;
	}

	if(!success)
	{
		mEntity = NULL;
		Ogre::LogManager::getSingleton().logMessage("[ManualPoseV2::initialise] ERROR : Entity is not valid.");
	}
	return success;
}

bool ManualPoseV2::initialise(const std::string& pPoseName)
{
	bool found = false;
	if(mEntity)
	{
		std::string lAnimationStateName = getAnimationStateNameFromPoseName(pPoseName);
		Ogre::AnimationStateSet* lSetAnimation = mEntity->getAllAnimationStates();
		if(lSetAnimation->hasAnimationState(lAnimationStateName))
		{
			Ogre::AnimationState* as = mEntity->getAnimationState(lAnimationStateName);
			as->setTimePosition(0);
			as->setEnabled(true);
			as->setWeight(0.0);
			found = true;
		}else{
			MessageBox(NULL,"[ManualPoseV2::initialise] The pose does not exist","pb",MB_OK);
		}
	}
	return found;
}


void ManualPoseV2::initialiseManualPoseFor(Ogre::Mesh& pMesh)
{
	//if(pMesh.isNull())
	//{
	//	Ogre::LogManager::getSingleton().logMessage("[ManualPoseV2::initialiseManualPoseFor] mesh introuvable");
	//}else
	{
		// we'll read through this list while loading
		PoseList poseList = pMesh.getPoseList();

		Ogre::LogManager::getSingleton().logMessage("[ManualPoseV2::initialiseManualPoseFor] " + pMesh.getName() +", poseListe size :"+Ogre::StringConverter::toString(poseList.size()));
		if(poseList.size()>0)
		{
			bool isEmpty = true;
			
			ManualPoseIsPossible lFor1Mesh;
			lFor1Mesh.mMeshName = pMesh.getName();

			for(int numPose = 0; numPose < int(poseList.size()); numPose++)
			{
				//nom de la pose
				const std::string lPoseName = poseList[numPose]->getName();

				// nom de l animation state
				std::string lPoseAnimationStateName = getAnimationStateNameFromPoseName(lPoseName);
				
				// on declare l'animation dans notre tableau static si elle n'existe pas déja
				bool alreadyDefine = pMesh.hasAnimation(lPoseAnimationStateName);

				if(!alreadyDefine)
				{
					isEmpty = false;

					//create the animation on the mesh
					Animation* anim = pMesh.createAnimation(lPoseAnimationStateName, 0);

					// on declare l animation dans notre fichier static
					ManualPoseIsPossible::InfoFor1Pose lInfoFor1Pose;
					lInfoFor1Pose.mAnimationName = lPoseAnimationStateName;
					lInfoFor1Pose.mName = lPoseName;

					//used to loop through each submesh
					int numSubMeshes = pMesh.getNumSubMeshes();

					// danger : shared geometry is "0", et submesh N°0 is in "1" (il y a un decalage de 1).
					for(int curSubMesh = 1; curSubMesh <= numSubMeshes; curSubMesh++)
					{	
						//Ogre::LogManager::getSingleton().logMessage("[ManualPoseV2::initialiseManualPoseFor] entree submeshes "+mpip.mMeshName);
						//create the VertexTrack on this animation
						Ogre::VertexAnimationTrack *vt = anim->createVertexTrack(curSubMesh, Ogre::VAT_POSE);

						//create the keyframe we will use to update this vertex track
						//keep all our keyframes in a map for later updating
						lInfoFor1Pose.mVertexPoseKeyFrameMap[curSubMesh] = vt->createVertexPoseKeyFrame(0);
						
						for(int curPoseIndex = 0;curPoseIndex < int(poseList.size());curPoseIndex++)
						{
							//add the references to each pose that applies to this subMesh
							if(poseList[curPoseIndex]->getTarget() == curSubMesh && poseList[curPoseIndex]->getName().compare(lPoseName) == 0)
							{
								//create a pose reference for each submesh pose
								Ogre::LogManager::getSingleton().logMessage("[ManualPoseV2::initialiseManualPoseFor] Added a pose");
								lInfoFor1Pose.mVertexPoseKeyFrameMap[curSubMesh]->addPoseReference(curPoseIndex, 1.0f);
							}
						}
					}
					lFor1Mesh.mPosesInfo.push_back(lInfoFor1Pose);
					Ogre::LogManager::getSingleton().logMessage("[ManualPoseV2::initialiseManualPoseFor] found "+lFor1Mesh.mMeshName);
				}else{
					Ogre::LogManager::getSingleton().logMessage("[ManualPoseV2::initialiseManualPoseFor] animation already found "+ lFor1Mesh.mMeshName);
				}
			}
			if(!isEmpty)
			{
				ManualPoseV2::sAllPoses.push_back(lFor1Mesh);
			}
		}
		else
		{
			// poseliste size = 0
		}
	}
}

void ManualPoseV2::initialiseManualPoseFor(const std::string& pMeshName, const std::string& pResourceGroupName)
{
	// on ne garde que les resource NON loadee
	Ogre::MeshManager* lMeshManager = Ogre::MeshManager::getSingletonPtr();
	{
		Ogre::MeshPtr lMesh = lMeshManager->getByName(pMeshName);
		if(!lMesh.isNull())
		{
			if(lMesh->isLoaded())
			{
				return;
			}
		}
	}

	// charger le mesh
	Ogre::MeshPtr lMesh = lMeshManager->load(pMeshName,pResourceGroupName);

	if(!lMesh.isNull())
	{
	/* 
		// réorganisation des buffers au cas où l'export se serait mal passé.
		// INCOMPATIBLE avec les STENCIL SHADOW
		for(int i = 0; i < lMesh->getNumSubMeshes(); i++)
		{
			VertexDeclaration* vertDecl = lMesh->getSubMesh(i)->vertexData->vertexDeclaration;
			lMesh->getSubMesh(i)->vertexData->reorganiseBuffers(vertDecl->getAutoOrganisedDeclaration(true,true));
		}
		*/
		ManualPoseV2::initialiseManualPoseFor(*lMesh.get());
	}
}

void ManualPoseV2::initialiseManualPosesForMeshesInRessourcesGroups(const std::string& pResourceGroupName)
{
	Ogre::LogManager::getSingleton().logMessage("[ManualPoseV2::initialiseManualPosesForMeshesInRessourcesGroups] : " + pResourceGroupName);
	Ogre::ResourceGroupManager * lrgmg = Ogre::ResourceGroupManager::getSingletonPtr();
	Ogre::StringVectorPtr lNames = lrgmg->findResourceNames(pResourceGroupName, "*.mesh", false);
	if(!lNames.isNull())
	{
		unsigned int lSize = lNames->size();
		for(unsigned int i = 0; i< lSize; i++)
		{
			initialiseManualPoseFor(lNames->at(i),pResourceGroupName);
		}
	}
}

std::vector<std::string> ManualPoseV2::getAllPoseAnimationStateNames()
{
	std::vector<std::string> vRes;
	if(mEntity)
	{
		MeshPtr lMesh = mEntity->getMesh();
		{
			std::list<ManualPoseV2::ManualPoseIsPossible>::iterator it = ManualPoseV2::sAllPoses.begin();
			std::list<ManualPoseV2::ManualPoseIsPossible>::iterator itEnd = ManualPoseV2::sAllPoses.end();
			bool found = false;
			for(; !found && it!=itEnd; ++it)
			{
				ManualPoseV2::ManualPoseIsPossible& mpip = *it;

				if(mpip.mMeshName == lMesh->getName())
				{
					for(unsigned int i = 0; i< mpip.mPosesInfo.size(); i++)
					{
						vRes.push_back(mpip.mPosesInfo[i].mAnimationName);
					}
					found = true;
				}
			}
		}
	}
	return vRes;
}

std::vector<std::string> ManualPoseV2::getAllPoseName()
{
	std::vector<std::string> vRes;
	if(mEntity)
	{
		MeshPtr lMesh = mEntity->getMesh();
		if(!lMesh.isNull())
		{
			std::list<ManualPoseV2::ManualPoseIsPossible>::iterator it = ManualPoseV2::sAllPoses.begin();
			std::list<ManualPoseV2::ManualPoseIsPossible>::iterator itEnd = ManualPoseV2::sAllPoses.end();
			bool found = false;
			for(; !found && it!=itEnd; ++it)
			{
				ManualPoseV2::ManualPoseIsPossible& mpip = *it;

				if(mpip.mMeshName == lMesh->getName())
				{
					for(unsigned int i = 0; i< mpip.mPosesInfo.size(); i++)
					{
						vRes.push_back(mpip.mPosesInfo[i].mName);
					}
					found = true;
				}
			}
		}
	}
	return vRes;
}

// Goes through all the submeshes and all the animState finding and updating anim with this name
// code du wiki MODIFIEE!! il y avait plusieurs bug (boucle while incorrecte, et -1 en trop)
void ManualPoseV2::updatePose(const std::string& pPoseName, float pWeight)
{   
	if(mEntity)
	{
		std::string lPoseName = getAnimationStateNameFromPoseName(pPoseName);
		Ogre::AnimationStateSet* lSetAnimation = mEntity->getAllAnimationStates();
		if(lSetAnimation->hasAnimationState(lPoseName))
		{
			mEntity->getAnimationState(lPoseName)->setWeight(pWeight);
		}
	}
}



