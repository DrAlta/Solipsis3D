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

#include "VLCPlugin.h"

#include "VLCTextureSource.h"
#include "VLCInstance.h"

#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreStringConverter.h>

using namespace Ogre;

namespace Solipsis {

const String sPluginName = "VLC Texture Source";

//-------------------------------------------------------------------------------------
VLCPlugin::VLCPlugin() :
    mVLCTextureSource(0),
    mVLCInstanceMapMutex(PTHREAD_MUTEX_INITIALIZER),
    mVLCInstanceMapCounter(0)
{
}

//-------------------------------------------------------------------------------------
const String& VLCPlugin::getName() const
{
    return sPluginName;
}

//-------------------------------------------------------------------------------------
void VLCPlugin::install()
{
}

//-------------------------------------------------------------------------------------
void VLCPlugin::initialise()
{
    mVLCTextureSource = new VLCTextureSource(this);
}

//-------------------------------------------------------------------------------------
void VLCPlugin::shutdown()
{
    if (mVLCTextureSource != 0)
    {
        mVLCTextureSource->clearInstances();
        delete mVLCTextureSource;
        mVLCTextureSource = 0;
    }
}

//-------------------------------------------------------------------------------------
void VLCPlugin::uninstall()
{
}

//-------------------------------------------------------------------------------------
int VLCPlugin::newInstance(const String& mrl, int width, int height, int fps, const String& soundParams, const String& vlcParams)
{
    LogManager::getSingleton().logMessage("VLCPlugin - Creating new instance mrl='" + mrl + "', " + StringConverter::toString(width) + "x" + StringConverter::toString(height) + "x" + StringConverter::toString(fps) + ", soundParams=" + soundParams + ", vlcParams=" + vlcParams);

    pthread_mutex_lock(&mVLCInstanceMapMutex);

    // Set mrl and map it by id
    int id = mVLCInstanceMapCounter++;
    VLCInstancePtr instance(new VLCInstance(id, mVLCTextureSource, mrl, width, height, fps, soundParams, vlcParams));
    mVLCInstanceMap.insert(std::make_pair(id, instance));

    pthread_mutex_unlock(&mVLCInstanceMapMutex);

    return id;
}

//-------------------------------------------------------------------------------------
void VLCPlugin::destroyInstance(int id, bool force)
{
    LogManager::getSingleton().logMessage("VLCPlugin::destroyInstance" + String(force ? " forced" : ""));

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    VLCInstanceMap::iterator i = mVLCInstanceMap.find(id);
    if (i != mVLCInstanceMap.end())
        i->second->destroy(force);
    if (force)
        mVLCInstanceMap.erase(i);
    pthread_mutex_unlock(&mVLCInstanceMapMutex);
}

//-------------------------------------------------------------------------------------
void VLCPlugin::deleteInstance(int id)
{
    LogManager::getSingleton().logMessage("VLCPlugin::deleteInstance");

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    VLCInstanceMap::iterator i = mVLCInstanceMap.find(id);
    if (i != mVLCInstanceMap.end())
        mVLCInstanceMap.erase(i);
    pthread_mutex_unlock(&mVLCInstanceMapMutex);
}

//-------------------------------------------------------------------------------------
VLCPlugin::VLCInstancePtr VLCPlugin::getInstance(int id)
{
    VLCInstancePtr instance;

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    VLCInstanceMap::iterator i = mVLCInstanceMap.find(id);
    if (i != mVLCInstanceMap.end())
        instance = i->second;
    pthread_mutex_unlock(&mVLCInstanceMapMutex);

    return instance;
}

//-------------------------------------------------------------------------------------
int VLCPlugin::lookupInstance(const String& mrl)
{
    LogManager::getSingleton().logMessage("VLCPlugin::lookupInstance");

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    for (VLCInstanceMap::const_iterator i = mVLCInstanceMap.begin(); i != mVLCInstanceMap.end(); ++i)
    {
        if (i->second->getMrl() == mrl)
        {
            pthread_mutex_unlock(&mVLCInstanceMapMutex);
            return i->first;
        }
    }
    pthread_mutex_unlock(&mVLCInstanceMapMutex);

    return -1;
}

//-------------------------------------------------------------------------------------
TexturePtr VLCPlugin::getTextureForInstance(const int id)
{
    LogManager::getSingleton().logMessage("VLCPlugin::getTextureForInstance");

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    VLCInstanceMap::iterator i = mVLCInstanceMap.find(id);
    if (i != mVLCInstanceMap.end())
    {
        pthread_mutex_unlock(&mVLCInstanceMapMutex);
        return i->second->getTexture();
    }

    pthread_mutex_unlock(&mVLCInstanceMapMutex);

    // Not found
    return TexturePtr();
}

//-------------------------------------------------------------------------------------
Ogre::String VLCPlugin::handleEvt(int id, const Ogre::String& evt)
{
    Ogre::String result;

    LogManager::getSingleton().logMessage("VLCPlugin::handleEvt");

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    VLCInstanceMap::iterator i = mVLCInstanceMap.find(id);
    if (i != mVLCInstanceMap.end())
        result = i->second->handleEvt(evt);
    pthread_mutex_unlock(&mVLCInstanceMapMutex);

    return result;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
