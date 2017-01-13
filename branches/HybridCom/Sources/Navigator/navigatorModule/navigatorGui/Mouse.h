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

#ifndef __Mouse_h__
#define __Mouse_h__

#include <map>
#include <CTSingleton.h>
#include <OgrePanelOverlayElement.h>
#include "Cursor.h"

namespace Solipsis
{
	/**
	* A simple little class that displays a mouse cursor using an Ogre Overlay that follows
	* the mouse coordinates injected.
	*/
    class Mouse : public CommonTools::Singleton<Mouse>, public Ogre::ManualResourceLoader
	{
		friend class Cursor;
		int mouseX, mouseY;
		unsigned short width, height;
		unsigned short texWidth, texHeight;
		Ogre::Overlay* overlay;
		Ogre::PanelOverlayElement* panel;
		Ogre::TexturePtr texture;
		std::map<std::string, Cursor*> cursors;
		std::map<std::string, Cursor*>::iterator iter;
		Cursor* activeCursor;
		std::string defaultCursorName;
		bool visible;

        void loadResource(Ogre::Resource* resource);

    public:
		/**
		* Creates the Mouse Singleton.
		*
		* @param	width	The width of the Mouse cursor (all loaded cursors should be this width to avoid stretching).
		* @param	height	The height of the Mouse cursor (all loaded cursors should be this height to avoid stretching).
		*
		* @param	visibility	Whether or not the cursor should be visible upon creation. Use Mouse::show() later.
		*/
		Mouse(unsigned short width = 64, unsigned short height = 64, bool visibility = true);
        /// Destructor
        ~Mouse();

		/**
		* Creates a cursor for use with this Mouse.
		*
		* @param	cursorName	The name of the cursor, used to activate or remove the cursor later.
		*
		* @param	hotspotX	Every cursor has a hot spot, which is used to define the single pixel a cursor 
		*						is pointing to when the mouse is clicked. This is the X-value of the hotspot.
		*
		* @param	hotspotY	The Y-value of the hotspot.
		*/
		Cursor* createCursor(const std::string &cursorName, unsigned short hotspotX = 0, unsigned short hotspotY = 0);

		/**
		* This should be called before application begins updating the mouse. Sets the default cursor for the mouse.
		*
		* @param	cursorName	The name of the cursor, from here on you may refer to this cursor by this cursorName
		*						or by "default", both will work. You may not remove a default cursor.
		*/
		void setDefaultCursor(const std::string &cursorName);

		/**
		* Removes a cursor from this Mouse.
		*
		* @param	cursorName	The cursor to remove. You may not remove a default cursor. If you try to remove a cursor
		*						that is currently active, the cursor will change to the default cursor after removal.
		*/
		void removeCursor(const std::string &cursorName);

		/**
		* Changes the active cursor to a specified cursor.
		*
		* @param	cursorName	The name of the cursor to change to. To change to the default cursor, simply specify "default".
		*						If the cursor cannot be found, nothing will happen.
		*/
		void activateCursor(std::string cursorName);

		/**
		* Displays the mouse cursor (if it is hidden via Mouse::hide).
		*/
		void show();

		/**
		* Hides the mouse cursor. Show it again via Mouse::show.
		*/
		void hide();

		/**
		* Returns whether or not the mouse cursor is visible.
		*/
		bool isVisible();

		/**
		* Move the mouse cursor.
		*/
        void move(int x, int y);

		/**
		* Update the mouse cursor.
		*/
        void update();
	};
} // namespace Solipsis

#endif // #ifndef __Mouse_h__
