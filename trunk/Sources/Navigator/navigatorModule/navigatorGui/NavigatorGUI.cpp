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

#include "NavigatorGUI.h"
#include "GUI_MessageBox.h"
#include "GUI_StatusBar.h"
#include "GUI_MainMenu.h"

#include <CTIO.h>
#include <Navi.h>

#ifdef _MSC_VER
 #pragma warning (disable:4355)
#endif // _MSC_VER

using namespace Solipsis;
using namespace CommonTools;

NavigatorGUI * NavigatorGUI::mNaviGui = NULL;

//-------------------------------------------------------------------------------------
NavigatorGUI::NavigatorGUI(Navigator* navigator) :
    mNavigator(navigator),
    mMouse(0),
    mLoginInfosText(""),
    m_pCurrentPanel(NULL)
{
	m_panels.clear();
    // Initializing Navi
    mNaviMgr = new NaviLibrary::NaviManager(mNavigator->getRenderWindowPtr()->getViewport(0), "./NaviLocal");
    mNaviMgr->setZOrderMinMax(200, 299);

    // Initializing 2D Panels manager
    mPanel2DMgr = new Panel2DMgr();
    mPanel2DMgr->setZOrderMinMax(100, 199);

    // Add ourself as a Window listener
    WindowEventUtilities::addWindowEventListener(mNavigator->getRenderWindowPtr(), this);

    mNaviGui = this;
}

//-------------------------------------------------------------------------------------
NavigatorGUI::~NavigatorGUI()
{
    // Remove ourself as a Window listener
    WindowEventUtilities::removeWindowEventListener(mNavigator->getRenderWindowPtr(), this);

	destroyAllRegisteredPanels();

    // Finalizing 2D Panels manager
    delete mPanel2DMgr;

    // Finalizing Navi
    delete mNaviMgr;

    // Destroy mouse
    if (mMouse != 0)
		delete mMouse;
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::startup()
{
    // Startup Mouse and create the cursors
    if (mMouse == 0)
    {
        mMouse = new Mouse();
        Cursor* defaultCursor = mMouse->createCursor("default_cursor", 1, 0);
	    defaultCursor->addFrame(1200, "cursor1.png")->addFrame(100, "cursor2.png")->addFrame(100, "cursor3.png")->addFrame(100, "cursor4.png");
	    defaultCursor->addFrame(100, "cursor5.png")->addFrame(100, "cursor6.png")->addFrame(100, "cursor5.png")->addFrame(100, "cursor4.png");
	    defaultCursor->addFrame(100, "cursor3.png")->addFrame(100, "cursor2.png");
        mMouse->setDefaultCursor("default_cursor");
	    Cursor* moveCursor = mMouse->createCursor("move", 19, 19);
	    moveCursor->addFrame(0, "cursorMove.png");

        mPanel2DMgr->initializeMouseCursors();
    }

    // Load Lua default GUI
    lua_State* luaState = mNavigator->getLuaState();
    std::string defaultGUIluaFilename = std::string("lua") + IO::getPathSeparator() + "defaultGUI.lua";
    if (luaL_loadfile(luaState, defaultGUIluaFilename.c_str()) != 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorGUI::startup() Unable to load defaultGUI.lua, error: %s", lua_tostring(luaState, -1));
        return false;
    }
    if (lua_pcall(luaState, 0, LUA_MULTRET, 0))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "NavigatorGUI::startup() Unable to run defaultGUI.lua, error: %s", lua_tostring(luaState, -1));
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::update()
{
    unsigned long now = Ogre::Root::getSingleton().getTimer()->getMilliseconds();

    if (Mouse::getSingletonPtr() != 0)
        Mouse::getSingletonPtr()->update();

    if (mNaviGui->m_pCurrentPanel)
    {
        mNaviGui->m_pCurrentPanel->update();
    }

    GUI_StatusBar::updateBar();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::windowResized(RenderWindow* rw)
{
    mNaviMgr->resetAllPositions();

    // send new position to all registered panels
    for (std::map<std::string, GUI_Panel *>::iterator it = mNaviGui->m_panels.begin(); it != mNaviGui->m_panels.end(); it++)
    {
        it->second->windowResized(rw);
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::SetMouseVisibility(bool visible)
{
    if (visible)
        Mouse::getSingleton().show();
    else
        Mouse::getSingleton().hide();
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::isMouseVisible()
{
    return ((mMouse != 0) && mMouse->isVisible());
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::inWorld()
{
    destroyAllRegisteredPanels();
    // show menu and status bar
	GUI_MainMenu::createAndShowPanel();
    GUI_StatusBar::createAndShowPanel();
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::ConnectionServerErrorMsgBoxResponse::onResponse(Navi* caller, const Awesomium::JSArguments& args)
{
    Navigator::getSingletonPtr()->disconnect(true);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::connectionServerError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::connectionError()");

    GUI_MessageBox::getMsgBox()->show("Network error",
        "Connection to server error...", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_EXCLAMATION,
        &connectionServerErrorMsgBoxResponse);
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::connectionLostError()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::connectionLostError()");

    GUI_MessageBox::getMsgBox()->show("Network error",
        "Peer lost its connection, re-connection in progress ...", 
        GUI_MessageBox::MBB_OK, 
        GUI_MessageBox::MBB_EXCLAMATION);
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::setNaviVisibility(const String& naviName, bool show)
{
    NaviLibrary::Navi* navi = mNaviMgr->getNavi(naviName);
    if (navi == 0) 
        return false;

    if (show)
    {
        if (navi->getVisibility()) 
            return true;

        navi->show(true);
    }
    else
    {
        if (!navi->getVisibility()) 
            return true;

        navi->hide(true);
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool NavigatorGUI::createPanel2D(const String& type, const String& name)
{
    Panel2D *newPanel = mPanel2DMgr->createPanel(type, name);
    mPanel2DMgr->focusPanel(0, 0, newPanel);
    return true;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::registerGuiPanel(GUI_Panel *pPanel)
{
    mNaviGui->m_panels[pPanel->getPanelName()] = pPanel;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::destroyAllRegisteredPanels()
{
	int l = mNaviGui->m_panels.size();
	std::map<std::string, GUI_Panel *>::iterator it = mNaviGui->m_panels.begin();
	while (it != mNaviGui->m_panels.end())
	{
        it->second->destroy();
		GUI_Panel *lPanel = it->second;
		delete lPanel;
		lPanel = NULL;
		it++;
    }
	mNaviGui->m_panels.clear();
	// The current panel has been destroyed
	mNaviGui->m_pCurrentPanel = NULL;
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::destroyCurrentPanel()
{
    if (mNaviGui->m_pCurrentPanel)
    {
        mNaviGui->m_pCurrentPanel->destroy();
		// unregister the current panel
		mNaviGui->m_panels.erase(mNaviGui->m_pCurrentPanel->getPanelName());
		delete mNaviGui->m_pCurrentPanel;
		mNaviGui->m_pCurrentPanel = NULL;
    }
}

//-------------------------------------------------------------------------------------
void NavigatorGUI::setCurrentPanel(GUI_Panel * pCurrentPanel)
{
    mNaviGui->m_pCurrentPanel = pCurrentPanel;
}
//-------------------------------------------------------------------------------------
