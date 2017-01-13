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

#include "Cursor.h"
#include <stdlib.h>

using namespace Ogre;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Cursor::Cursor(const std::string &cursorName, unsigned short hotspotX, unsigned short hotspotY, unsigned short mouseWidth, unsigned short mouseHeight)
{
	mouseTex = TextureManager::getSingleton().getByName("MouseTexture");
	name = cursorName;
	hsX = hotspotX;
	hsY = hotspotY;
	mWidth = mouseWidth;
	mHeight = mouseHeight;
	frameCount = 0;
	curFrame = 0;
	frameStartTime = 0;
	lockedDuration = false;
}

//-------------------------------------------------------------------------------------
Cursor::~Cursor()
{
	for(unsigned int i = 0; i < frames.size(); i++)
	{
		Frame* frmToDelete = frames.at(i);
		TextureManager::getSingleton().remove(frmToDelete->texture->getName());
		delete frmToDelete;
	}
}

//-------------------------------------------------------------------------------------
Cursor* Cursor::addFrame(unsigned short durationMS, const std::string &imageFilename, const std::string &imageResourceGroup)
{
	Image cursorFrameImg;
	cursorFrameImg.load(imageFilename, imageResourceGroup);

	std::string textureName = name;
	textureName += "Frame";
	char buffer[8];
	textureName += itoa(frameCount, buffer, 10);

	Frame* newFrame = new Frame();
	newFrame->duration = durationMS;
	newFrame->texture = Ogre::TextureManager::getSingleton().loadImage(
		textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		cursorFrameImg, Ogre::TEX_TYPE_2D, 0, 1, false, Ogre::PF_BYTE_BGRA);
	newFrame->index = frameCount;

	frames.push_back(newFrame);

	if(!frameCount) curFrame = newFrame;

	frameCount++;

	return this;
}

//-------------------------------------------------------------------------------------
std::string Cursor::getName()
{
	return name;
}

//-------------------------------------------------------------------------------------
void Cursor::update(bool force)
{
	if(!curFrame || (lockedDuration && !force)) return;
	if(force) lockedDuration = false;

	if(timer.getMilliseconds()-frameStartTime > curFrame->duration || force)
	{
		if(frameStartTime)
		{
			if(curFrame->index == frames.size()-1) curFrame = frames.at(0);
			else curFrame = frames.at(curFrame->index + 1);
		}

		Ogre::TexturePtr cursorTex = curFrame->texture;
		
		if(!mouseTex.isNull() && !cursorTex.isNull())
		{
			HardwarePixelBufferSharedPtr mousePBuff = mouseTex->getBuffer();
			HardwarePixelBufferSharedPtr cursorPBuff = cursorTex->getBuffer();

			// Workaround for OpenGL reading problems
			uint8* tempBuffer = new uint8[cursorPBuff->getSizeInBytes()];
			PixelBox cursorPBox(cursorPBuff->getWidth(), cursorPBuff->getHeight(), cursorPBuff->getDepth(), cursorPBuff->getFormat(), tempBuffer);

			cursorPBuff->blitToMemory(cursorPBox);
			
			mousePBuff->blitFromMemory(cursorPBox, Ogre::Image::Box(0, 0, mWidth, mHeight));

			delete[] tempBuffer;

			if(curFrame->duration == 0) lockedDuration = true;
		}
		else
		{
			OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
				"When updating the texture of the Mouse Cursor, an internally-created dynamic texture that should have been there was not found.", 
				"Cursor::update");
		}

		frameStartTime = timer.getMilliseconds();
	}
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
