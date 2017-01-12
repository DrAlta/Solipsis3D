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

#include "GUI_Panel.h"
#include "MainApplication/Navigator.h"
#include <CTLog.h>
#include <CTStringHelpers.h>
#include <Navi.h>

using namespace Solipsis;
using namespace CommonTools;

//-------------------------------------------------------------------------------------
GUI_Panel::GUI_Panel(const std::string & panelName)
{
    mPanelName = panelName;
    m_curState = NSNotCreated;
    NavigatorGUI::registerGuiPanel(this);
    mCurrentNaviCreationDate = 0;
    mNavi = NULL;
}

//-------------------------------------------------------------------------------------
GUI_Panel::~GUI_Panel()
{
}

//-------------------------------------------------------------------------------------
bool GUI_Panel::show()
{
    if ((m_curState != GUI_Panel::NSCreated) || (mNavi == 0))
        return false;

    if (!mNavi->getVisibility())
    {
        mNavi->show(true);
        mNavi->focus();
    }

    return true;
}



//-------------------------------------------------------------------------------------
bool GUI_Panel::isVisible()
{
    if (m_curState != GUI_Panel::NSCreated) 
        return false;

    return ((mNavi != 0) && mNavi->getVisibility());
}

//-----virtual fct hide can be overloaded for more actions when hidden -----------------------------------------------------------
void GUI_Panel::hide()
{
    if (m_curState != GUI_Panel::NSCreated) 
        return;

    mNavi->hide();
    NaviManager::Get().deFocusAllNavis();
}

//-------------------------------------------------------------------------------------
void GUI_Panel::destroyPreviousNavi()
{
    GUI_Panel * pCurrentPanel = NavigatorGUI::getCurrentPanel();
    // Hide previous Navi UI
    if (pCurrentPanel != 0) 
    {
        pCurrentPanel->destroy();
    }
}

//-------------------------------------------------------------------------------------
void GUI_Panel::destroy()
{
    if (m_curState != GUI_Panel::NSCreated) 
        return;

    mNavi->hide();
    NaviLibrary::NaviManager::Get().destroyNavi(mNavi);
    m_curState = NSNotCreated;
    mNavi = NULL;
}

//-------------------------------------------------------------------------------------
void GUI_Panel::createNavi(const NaviPosition &naviPosition,
                unsigned short width, unsigned short height)
{
    mCurrentNaviCreationDate = 0;

    mNavi = NaviLibrary::NaviManager::Get().createNavi(mPanelName, width, height, naviPosition);
}

//-------------------------------------------------------------------------------------
void GUI_Panel::onPanelLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    // Show Navi UI
    if (m_curState == NSCreated)
    {
        mNavi->show(true);
        mNavi->focus();
    }

    // used for timeout
    mCurrentNaviCreationDate = 0;
}

//-------------------------------------------------------------------------------------
void GUI_Panel::fitOnScreen(int & w, int & h)
{
    unsigned int width, height, colourDepth;
    int left, top;
    Navigator::getSingletonPtr()->getRenderWindowPtr()->getMetrics(width, height, colourDepth, left, top);

//     while (w > width && h > height)
//     {
//         if (w > width) w = w/2;
//         if (h > width) h = h/2;
//     }

    w = width - 32;
    h = height - 32;
}

//-------------------------------------------------------------------------------------
