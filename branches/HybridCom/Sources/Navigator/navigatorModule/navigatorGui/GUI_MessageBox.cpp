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

#include "GUI_MessageBox.h"
#include <CTLog.h>
#include <CTStringHelpers.h>
#include <Navi.h>

// #ifdef _MSC_VER
//  #pragma warning (disable:4355)
// #endif // _MSC_VER

using namespace Solipsis;
using namespace CommonTools;

GUI_MessageBox * GUI_MessageBox::st_GUI_MessageBox = NULL;

//-------------------------------------------------------------------------------------
bool GUI_MessageBox::show(const std::string& titleText, 
                            const std::string& msgText, 
                            MsgBoxButtons buttons, 
                            MsgBoxIcon icon,
                            GUI_MessageBoxResponse *msgBoxResponse)
{
    if (!st_GUI_MessageBox)
    {
        st_GUI_MessageBox = new GUI_MessageBox();
    }

    return st_GUI_MessageBox->protectedShow(titleText, msgText, buttons, icon, msgBoxResponse);
}

//-------------------------------------------------------------------------------------
bool GUI_MessageBox::protectedShow(const std::string& titleText,
                                   const std::string& msgText,
                                   MsgBoxButtons buttons,
                                   MsgBoxIcon icon,
                                   GUI_MessageBoxResponse *msgBoxResponse)
{
    if (m_curState == GUI_Panel::NSNotCreated)
    {
        // Create Navi panel
        // Lua
        createNavi(NaviPosition(Center), 512, 128);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_MessageBox::onPageLoaded));
        mNavi->loadFile("uimsgbox.html");
        mNavi = NavigatorGUI::getNavi(mPanelName);
        mNavi->hide();
        mNavi->setMask("uimsgbox.png");
        mNavi->setModal(true);

        mNavi->bind("response", NaviDelegate(this, &GUI_MessageBox::onResponse));

        m_curState = NSCreated;
    }

    mMsgBoxTitleText = titleText;
    mMsgBoxMsgText = msgText;
    mMsgBoxButtons = buttons;
    mMsgBoxIcon = icon;
    mMsgBoxResponse = msgBoxResponse;

    return true;
}

//-------------------------------------------------------------------------------------
void GUI_MessageBox::onPageLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::messageBoxPageLoaded()");

    mNavi->evaluateJS("$('titleText').innerHTML = '" + mMsgBoxTitleText + "'");
    mNavi->evaluateJS("$('msgText').innerHTML = '" + mMsgBoxMsgText + "'");
    mNavi->evaluateJS("setButtons(" + StringHelpers::toString(mMsgBoxButtons) + ")");
    mNavi->evaluateJS("setIcon(" + StringHelpers::toString(mMsgBoxIcon) + ")");

    // Show Navi UI message box
    GUI_Panel::onPanelLoaded(caller, args);
}

//-------------------------------------------------------------------------------------
void GUI_MessageBox::onResponse(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::messageBoxResponse()");

    if (mMsgBoxResponse != 0)
        mMsgBoxResponse->onResponse(caller, args);
    destroy();
}

//-------------------------------------------------------------------------------------
