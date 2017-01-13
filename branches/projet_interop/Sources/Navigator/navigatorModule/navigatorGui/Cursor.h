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

#ifndef __Cursor_h__
#define __Cursor_h__

#include <string>
#include <vector>
#include <Ogre.h>

namespace Solipsis
{
	/**
	* A quick little class that lets you define animated/static cursors for use with Mouse.
	*
	* @note	You should instantiate this class through Mouse::createCursor.
	*/
	class Cursor
	{
		friend class Mouse;
		Ogre::TexturePtr mouseTex;
		std::string name;
		unsigned short hsX, hsY, mWidth, mHeight;
		struct Frame { unsigned short duration; Ogre::TexturePtr texture; unsigned short index; };
		std::vector<Frame*> frames;
		Frame* curFrame;
		unsigned short frameCount;
		Ogre::Timer timer;
		unsigned long frameStartTime;
		bool lockedDuration;

		void update(bool force = false);
		Cursor(const std::string &cursorName, unsigned short hotspotX, unsigned short hotspotY, unsigned short mouseWidth, unsigned short mouseHeight);
		~Cursor();
	public:
		/**
		* Adds a new frame (image) to the cursor. If you add more than one frame to a cursor, they will
		* be animated by looping the images.
		*
		* @param	durationMS	The number of milliseconds to display this frame for. Use a duration of '0' to lock
		*						the animation when it reaches this frame.
		*
		* @param	imageFilename	The filename of the image to use for this frame. Please note that the
		*							width and height of the image must each be greater than or equal to 64.
		*
		* @param	imageResourceGroup	The Ogre Resource Group to find the image filename. Defaults to 
		*								Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME.
		*
		* @throws	Ogre::Exception::ERR_INVALIDPARAMS	Throws this if the width or height of the image is less than 64.
		*
		* @return	A pointer to the Cursor you added a frame to.
		*/
		Cursor* addFrame(unsigned short durationMS, const std::string &imageFilename, 
			const std::string &imageResourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		/**
		* Gets the name of this Cursor.
		*
		* @return	The name of this cursor as a string.
		*/
		std::string getName();
	};
} // namespace Solipsis

#endif // #ifndef __Cursor_h__
