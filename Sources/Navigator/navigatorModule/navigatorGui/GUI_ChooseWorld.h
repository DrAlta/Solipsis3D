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

#ifndef __GUI_ChooseWorld_h__
#define __GUI_ChooseWorld_h__

// #include <Facebook.h>
#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
#include "GUI_FromServer.h"
// #include "World/Modeler.h"
// #include "ModifiableMaterialObject.h"

using namespace NaviLibrary;

namespace Solipsis 
{
    class Navigator;

 
    /** This class manages all Graphical User Interfaces of the Navigator.
    */
    class GUI_ChooseWorld : public GUI_FromServer
    {
    public:
        static bool createAndShowPanel();

        // Show the page
        virtual bool show();

        // ok is pressed
        void onOkPressed(Navi* caller, const Awesomium::JSArguments& args);
        void onCancelPressed(Navi* caller, const Awesomium::JSArguments& args);

     
    protected:
        GUI_ChooseWorld();
		~GUI_ChooseWorld();
 
        static GUI_ChooseWorld * stGUI_ChooseWorld;

    };


} // namespace Solipsis

#endif // #ifndef __GUI_ChooseWorld_h__