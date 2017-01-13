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

#ifndef __IInstance_h__
#define __IInstance_h__

#include "NavigatorModule.h"

namespace Solipsis {

/** This class represents a running instance of the application.
 */
class IInstance
{
public:
    /** Sets the IWindow object for the rendering window that Navigator should render into
    @param w 
        The container application should create an object that implements
        the IWindow interface to return handle, width and height information
    */
    virtual bool setWindow(IWindow* w) = 0;

    /** Returns the IWindow pointer for width, height, etc.
    */
    virtual IWindow* getIWindow() = 0;

    /** Process 1 new event (Mouse, Keyboard, ...)
    */
    virtual bool processEvent(const Event& event) = 0;

    /** runs the render loop
    */
    virtual bool run() = 0;

    /** terminates the instance.
    */
    virtual void requestTerminate() = 0;
};

} // namespace Solipsis

#endif // #ifndef __IInstance_h__
