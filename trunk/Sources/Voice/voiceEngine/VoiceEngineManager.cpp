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

#include "VoiceEngineManager.h"

using namespace Solipsis;

template<> VoiceEngineManager* CommonTools::Singleton<VoiceEngineManager>::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
VoiceEngineManager* VoiceEngineManager::getSingletonPtr(void)
{
    return ms_Singleton;
}

//-------------------------------------------------------------------------------------
VoiceEngineManager& VoiceEngineManager::getSingleton(void)
{
    assert(ms_Singleton);
    return (*ms_Singleton);
}

//-------------------------------------------------------------------------------------
VoiceEngineManager::VoiceEngineManager() :
    mSelected(0)
{
}

//-------------------------------------------------------------------------------------
VoiceEngineManager::~VoiceEngineManager()
{
}

//-------------------------------------------------------------------------------------
void VoiceEngineManager::addEngine(IVoiceEngine* engine)
{
    mEngines.push_back(engine);
}

//-------------------------------------------------------------------------------------
void VoiceEngineManager::removeEngine(IVoiceEngine* engine)
{
    // Remove only if equal to registered one, since it might overridden
    // by other plugins
    for (EngineList::iterator it=mEngines.begin(); it != mEngines.end(); ++it)
        if ((*it) == engine)
        {
            mEngines.erase(it);
            break;
        }
}

//-------------------------------------------------------------------------------------
VoiceEngineManager::EngineList& VoiceEngineManager::getEngines()
{
    return mEngines;
}

//-------------------------------------------------------------------------------------
void VoiceEngineManager::selectEngine(const std::string& name)
{
    mSelected = 0;
    for (EngineList::iterator it=mEngines.begin(); it != mEngines.end(); ++it)
        if ((*it)->getName().compare(name) == 0)
        {
            mSelected = (*it);
            break;
        }
}

//-------------------------------------------------------------------------------------
IVoiceEngine* VoiceEngineManager::getSelectedEngine()
{
    return mSelected;
}

//-------------------------------------------------------------------------------------
