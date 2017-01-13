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

#include "VLCTextureSource.h"
#include "VLCPlugin.h"
#include "VLCInstance.h"

#include <OgreExternalTextureSourceManager.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgrePass.h>
#include <OgreRoot.h>
#include <OgreTechnique.h>
#include <OgreTextureManager.h>
#include <OgreStringConverter.h>

namespace Solipsis {

#define DEFAULT_WIDTH 256
#define DEFAULT_HEIGHT 256

VLCTextureSource::CmdMrl VLCTextureSource::msCmdMrl;
VLCTextureSource::CmdWidth VLCTextureSource::msCmdWidth;
VLCTextureSource::CmdHeight VLCTextureSource::msCmdHeight;
VLCTextureSource::CmdSoundParams VLCTextureSource::msCmdSoundParams;
VLCTextureSource::CmdVlcParams VLCTextureSource::msCmdVlcParams;
VLCTextureSource::CmdSoundId VLCTextureSource::msCmdSoundId;

ExternalTextureSourceExSoundHandler *VLCTextureSource::msSoundHandler = 0;

//-------------------------------------------------------------------------------------
VLCTextureSource::VLCTextureSource(VLCPlugin *plugin)
    : mPlugin(plugin)
{
    mDictionaryName = "vlc";
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6 && OGRE_VERSION_PATCH <4)
	mParamDictName = "vlc";
#else
	createParamDictionary("vlc");
#endif
    mPlugInName = "VLC texture source plugin";
    mMrl = "";
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    Ogre::ExternalTextureSourceManager::getSingleton().setExternalTextureSource("vlc", this);
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::instanceDestroyed(int id)
{
    MaterialListMap::iterator i = mMaterials.find(id);
    if (i != mMaterials.end())
        mMaterials.erase(id);
    mPlugin->deleteInstance(id);
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::clearInstances()
{
    for(MaterialListMap::iterator materialListIt=mMaterials.begin();materialListIt!=mMaterials.end();materialListIt=mMaterials.begin())
    {
        mPlugin->destroyInstance(materialListIt->first, true);
        mMaterials.erase(materialListIt);
    }
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdMrl::doGet(const void* target) const
{
	return static_cast<const VLCTextureSource*>(target)->getMrl();
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdMrl::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setMrl(val);
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdWidth::doGet(const void* target) const
{
	return Ogre::StringConverter::toString(static_cast<const VLCTextureSource*>(target)->getWidth());
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdWidth::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setWidth(Ogre::StringConverter::parseInt(val));
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdHeight::doGet(const void* target) const
{
	return Ogre::StringConverter::toString(static_cast<const VLCTextureSource*>(target)->getHeight());
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdHeight::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setHeight(Ogre::StringConverter::parseInt(val));
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdSoundParams::doGet(const void* target) const
{
	return static_cast<const VLCTextureSource*>(target)->getSoundParams();
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdSoundParams::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setSoundParams(val);
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdVlcParams::doGet(const void* target) const
{
	return static_cast<const VLCTextureSource*>(target)->getVlcParams();
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdVlcParams::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setVlcParams(val);
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdSoundId::doGet(const void* target) const
{
	return Ogre::StringConverter::toString(static_cast<const VLCTextureSource*>(target)->getSoundId());
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdSoundId::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setSoundId(Ogre::StringConverter::parseInt(val));
}

//-------------------------------------------------------------------------------------
bool VLCTextureSource::initialise()
{
    addBaseParams();

    // Add commands to set VLC parameters
    Ogre::ParamDictionary* dict = getParamDictionary();
    dict->addParameter(Ogre::ParameterDef("mrl", "VLC Media Resource Link", Ogre::PT_STRING), &msCmdMrl);
    dict->addParameter(Ogre::ParameterDef("width", "Video width", Ogre::PT_INT), &msCmdWidth);
    dict->addParameter(Ogre::ParameterDef("height", "Video height", Ogre::PT_INT), &msCmdHeight);
    dict->addParameter(Ogre::ParameterDef("sound_params", "Additional sound parameters", Ogre::PT_STRING), &msCmdSoundParams);
    dict->addParameter(Ogre::ParameterDef("vlc_params", "Additional VLC parameters", Ogre::PT_STRING), &msCmdVlcParams);
    dict->addParameter(Ogre::ParameterDef("sound_id", "Additional VLC parameters", Ogre::PT_STRING), &msCmdSoundId);

    return true;
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::shutDown()
{
    clearInstances();
    cleanupDictionary();
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::createDefinedTexture(const Ogre::String& materialName, const Ogre::String& group)
{
    Ogre::LogManager::getSingleton().logMessage("VLCTextureSource::createDefinedTexture (" + materialName + ")");

    // Get material by name
    // XXX Should we create a new material if an existing isn't found?
    Ogre::MaterialPtr material = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(materialName);
    if (material.isNull())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "VLC material '" + materialName +
            "' not found!");
        return;
    }

    if (mMrl.empty())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
            "VLCTextureSource - Error: missing or invalid parameter 'mrl'");
        return;
    }

    // Lookup existing instance on this mrl
    int id = mPlugin->lookupInstance(mMrl);
    if (id == -1)
    {
        // If no existing instance was found create a new one
        id = mPlugin->newInstance(mMrl, mWidth, mHeight, mFramesPerSecond, mSoundParams, mVlcParams);
        if (id == -1)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
                "VLCTextureSource - Error: VLC instance creation for '" + mMrl + "' failed!");
            return;
        }
    }

    // Bind this material to the sound buffer of the instance into the sound handler
    if ((msSoundHandler != 0) && (mSoundParams.find("3d") == 0))
        msSoundHandler->bindMaterialToSoundBuffer(materialName, mPlugin->getInstance(id)->getSoundId());
    // Add material to materials list using this VLC texture instance
    MaterialListMap::iterator materialListIt = mMaterials.find(id);
    if (materialListIt == mMaterials.end())
    {
        MaterialList newMaterialList;
        newMaterialList.push_back(material);
        mMaterials.insert(std::make_pair(id, newMaterialList));
    }
    else
    {
        materialListIt->second.push_back(material);
    }
    Ogre::TexturePtr tex = mPlugin->getTextureForInstance(id);
    if (!tex.isNull())
    {
        Ogre::LogManager::getSingleton().logMessage(
            "VLCTextureSource - Assigning VLC texture '" + tex->getName() +
            "' to material");

        Ogre::Technique* tech = material->getTechnique(0);
        Ogre::Pass* pass = tech->getPass(0);
        pass->removeAllTextureUnitStates();
        Ogre::TextureUnitState* state = pass->createTextureUnitState();
        state->setTextureFiltering(Ogre::FO_ANISOTROPIC, Ogre::FO_ANISOTROPIC, Ogre::FO_NONE);
        state->setTextureAnisotropy(4);
        state->setTextureName(tex->getName());
    }
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::destroyAdvancedTexture(const Ogre::String& material, const Ogre::String& group)
{
    Ogre::LogManager::getSingleton().logMessage("VLCTextureSource::destroyAdvancedTexture (" + material + ")");

    for (MaterialListMap::iterator materialListIt = mMaterials.begin(); materialListIt != mMaterials.end(); ++materialListIt)
    {
        MaterialList& materials = materialListIt->second;
        for (MaterialList::iterator materialIt = materials.begin(); materialIt != materials.end(); ++materialIt)
        {
            Ogre::MaterialPtr matPtr = *materialIt;
            if (!matPtr.isNull() && matPtr->getName() == material)
            {
                // Unbind this material from the sound buffer of the instance into the sound handler
                if ((msSoundHandler != 0) && (mSoundParams.find("3d") == 0))
                    msSoundHandler->unbindMaterialToSoundBuffer(material);
                // remove this material from list of materials using this VLC texture
                materials.erase(materialIt);
                // no more material using this VLC texture ?
                if (materials.empty())
                    // destroy this VLC texture
                    mPlugin->destroyInstance(materialListIt->first, false);
                return;
            }
        }
    }
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::handleEvt(const Ogre::String& material, const Ogre::String& evt)
{
    for (MaterialListMap::iterator materialListIt = mMaterials.begin(); materialListIt != mMaterials.end(); ++materialListIt)
    {
        MaterialList& materials = materialListIt->second;
        for (MaterialList::iterator materialIt = materials.begin(); materialIt != materials.end(); ++materialIt)
        {
            Ogre::MaterialPtr matPtr = *materialIt;
            if (!matPtr.isNull() && matPtr->getName() == material)
                return mPlugin->handleEvt(materialListIt->first, evt);
        }
    }
    return "";
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::setSoundHandler(ExternalTextureSourceExSoundHandler* soundHandler)
{
    msSoundHandler = soundHandler;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
