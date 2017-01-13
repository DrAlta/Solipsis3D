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

#include "Prerequisites.h"

#include "Mouse.h"

#include <OgreBitwise.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

Solipsis::Mouse *CommonTools::Singleton<Solipsis::Mouse>::ms_Singleton = 0;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Mouse::Mouse(unsigned short width, unsigned short height, bool visibility)
{
	mouseX = mouseY = 0;
	activeCursor = 0;
	defaultCursorName = "";
	visible = visibility;
	this->width = texWidth = width;
	this->height = texHeight = height;

	bool compensateNPOT = false;
	if(!Ogre::Bitwise::isPO2(width) || !Ogre::Bitwise::isPO2(height))
	{
		
		if(Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_NON_POWER_OF_2_TEXTURES))
		{
			if(Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->getNonPOW2TexturesLimited())
				compensateNPOT = true;
		}
		else compensateNPOT = true;
		
		if(compensateNPOT)
		{
			texWidth = Ogre::Bitwise::firstPO2From(width);
			texHeight = Ogre::Bitwise::firstPO2From(height);
		}
	}

	// Create the texture
	texture = Ogre::TextureManager::getSingleton().createManual(
		"MouseTexture", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::TEX_TYPE_2D, texWidth, texHeight, 0, Ogre::PF_BYTE_BGRA,
		Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE, this);

	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("MouseMaterial", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->createTextureUnitState("MouseTexture")->setTextureFiltering(Ogre::FO_NONE, Ogre::FO_NONE, Ogre::FO_NONE);

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	panel = static_cast<Ogre::PanelOverlayElement*>(overlayManager.createOverlayElement("Panel", "MousePanel"));
	panel->setMetricsMode(Ogre::GMM_PIXELS);
	panel->setPosition(0, 0);
	panel->setDimensions(width, height);
	panel->setMaterialName("MouseMaterial");
	if(compensateNPOT)
		panel->setUV(0, 0, (Ogre::Real)width/(Ogre::Real)texWidth, (Ogre::Real)height/(Ogre::Real)texHeight);	

	overlay = overlayManager.create("MouseOverlay");
	overlay->add2D(panel);
	overlay->setZOrder(650);
	if(visible) overlay->show();

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	ShowCursor(false);
#endif
}

//-------------------------------------------------------------------------------------
Mouse::~Mouse()
{
	for(iter = cursors.begin(); iter != cursors.end();)
	{
		Cursor* toDelete = iter->second;
		iter = cursors.erase(iter);
		delete toDelete;
	}

	if(overlay)
	{
		overlay->remove2D(panel);
		Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(panel);
		Ogre::OverlayManager::getSingletonPtr()->destroy(overlay);
	}

	Ogre::MaterialManager::getSingletonPtr()->remove("MouseMaterial");
	Ogre::TextureManager::getSingletonPtr()->remove("MouseTexture");
}

//-------------------------------------------------------------------------------------
Cursor* Mouse::createCursor(const std::string &cursorName, unsigned short hotspotX, unsigned short hotspotY)
{
	if(cursorName.empty()) 
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, 
			"An attempt was made to create a Cursor with an empty name!", 
			"Mouse::createCursor");

	if(cursors.find(cursorName) != cursors.end())
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, 
			"A Cursor named '" + cursorName + "' already exists! Could not create a new Cursor.",
			"Mouse::createCursor");

	return cursors[cursorName] = new Cursor(cursorName, hotspotX, hotspotY, texWidth, texHeight);
}

//-------------------------------------------------------------------------------------
void Mouse::setDefaultCursor(const std::string &cursorName)
{
	if(cursorName.empty()) 
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, 
			"An attempt was made to set the default cursor with an empty name!", 
			"Mouse::setDefaultCursor");

	iter = cursors.find(cursorName);
	if(iter == cursors.end())
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, 
			"A Cursor named '" + cursorName + "' does not exist! Could not set the default Cursor.", 
			"Mouse::setDefaultCursor");

	activeCursor = iter->second;
	activeCursor->update(true);
	move(mouseX, mouseY);
	defaultCursorName = cursorName;
}

//-------------------------------------------------------------------------------------
void Mouse::removeCursor(const std::string &cursorName)
{
	if(cursorName == defaultCursorName)
		return;

	iter = cursors.find(cursorName);
	if(iter == cursors.end())
		return;

	Cursor* cursorToDelete = iter->second;
	if(cursorToDelete == activeCursor)
		activateCursor("default");

	cursors.erase(iter);
	delete cursorToDelete;
}

//-------------------------------------------------------------------------------------
void Mouse::activateCursor(std::string cursorName)
{
	if(cursorName == "default")
		cursorName = defaultCursorName;

	iter = cursors.find(cursorName);
	if(iter == cursors.end())
		return;
	else if(activeCursor == iter->second)
		return;

	activeCursor = iter->second;
	activeCursor->update(true);
	move(mouseX, mouseY);
}

//-------------------------------------------------------------------------------------
void Mouse::show()
{
	if(visible)
		return;

	visible = true;
	overlay->show();
}

//-------------------------------------------------------------------------------------
void Mouse::hide()
{
	if(!visible)
		return;

	visible = false;
	overlay->hide();
}

//-------------------------------------------------------------------------------------
bool Mouse::isVisible()
{
	return visible;
}

//-------------------------------------------------------------------------------------
void Mouse::move(int x, int y)
{
	panel->setPosition(x-activeCursor->hsX, y-activeCursor->hsY);
	mouseX = x;
	mouseY = y;
}

//-------------------------------------------------------------------------------------
void Mouse::update()
{
	if(activeCursor && visible)
		activeCursor->update();
}

//-------------------------------------------------------------------------------------
void Mouse::loadResource(Ogre::Resource *resource)
{
	Ogre::Texture *tex = static_cast<Ogre::Texture*>(resource); 

	tex->setTextureType(Ogre::TEX_TYPE_2D);
	tex->setWidth(texWidth);
	tex->setHeight(texHeight);
	tex->setNumMipmaps(0);
	tex->setFormat(Ogre::PF_BYTE_BGRA);
	tex->setUsage(Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	tex->createInternalResources();

	if(activeCursor && visible)
		activeCursor->update(true);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
