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

#ifndef __Panel2DExtTextSrcEx_h__
#define __Panel2DExtTextSrcEx_h__

#include "Panel2D.h"
#include <OgreExternalTextureSourceManager.h>
#include "OgreTools/ExternalTextureSourceEx.h"

namespace Solipsis {

/** This class manages 2D panel for ExternalTextureSourceEx materials (VLC/VNC/SWF).
*/
class Panel2DExtTextSrcEx : public Panel2D
{
protected:
    ExternalTextureSourceEx *mExtTextSrcEx;
    bool mMouseCaptured;
    bool mGotFocus;

public:
    /// Constructor
    Panel2DExtTextSrcEx(const Ogre::String& type, const Ogre::String& name);
    /// Destructor
    ~Panel2DExtTextSrcEx();

    virtual bool create(Ogre::ushort zOrder);

protected:
    /** See KeyboardEventListener. */
    virtual bool keyPressed(const KeyboardEvt& evt);
    /** See KeyboardEventListener. */
    virtual bool keyReleased(const KeyboardEvt& evt);

    /** See MouseEventListener. */
    virtual bool mouseMoved(const MouseEvt& evt);
    /** See MouseEventListener. */
    virtual bool mousePressed(const MouseEvt& evt);
    /** See MouseEventListener. */
    virtual bool mouseReleased(const MouseEvt& evt);

protected:
    virtual void onFocus(bool isFocused);
};

/** This factory class to create 2D panel for VLC ExternalTextureSourceEx material.
*/
class Panel2DVLCFactory : public Panel2DFactory
{
private:
    /// Panel2D type
    static const Ogre::String ms_Type;

public:
    virtual Panel2D* createPanel2D(const Ogre::String& name)
    {
        return new Panel2DExtTextSrcEx(ms_Type, name);
    }
    virtual const Ogre::String& getType() const
    {
        return ms_Type;
    }
};

/** This factory class to create 2D panel for VNC ExternalTextureSourceEx material.
*/
class Panel2DVNCFactory : public Panel2DFactory
{
private:
    /// Panel2D type
    static const Ogre::String ms_Type;

public:
    virtual Panel2D* createPanel2D(const Ogre::String& name)
    {
        return new Panel2DExtTextSrcEx(ms_Type, name);
    }
    virtual const Ogre::String& getType() const
    {
        return ms_Type;
    }
};

/** This factory class to create 2D panel for SWF ExternalTextureSourceEx material.
*/
class Panel2DSWFFactory : public Panel2DFactory
{
private:
    /// Panel2D type
    static const Ogre::String ms_Type;

public:
    virtual Panel2D* createPanel2D(const Ogre::String& name)
    {
        return new Panel2DExtTextSrcEx(ms_Type, name);
    }
    virtual const Ogre::String& getType() const
    {
        return ms_Type;
    }
};

} // namespace Solipsis

#endif // #ifndef __Panel2DExtTextSrcEx_h__
