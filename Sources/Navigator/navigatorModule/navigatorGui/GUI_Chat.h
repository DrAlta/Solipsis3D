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

#ifndef __GUI_Chat_h__
#define __GUI_Chat_h__

#include "NavigatorGui.h"
#include "MainApplication/Navigator.h"
//#include "GUI_Panel.h"
#include "GUI_FromServer.h"

using namespace NaviLibrary;
using namespace std;

namespace Solipsis 
{
    class Navigator;

    //class GUI_Chat : public GUI_Panel
    class GUI_Chat : public GUI_FromServer
    {
    public:
        // switch between show and hide
        static void showHide();
        static void addText(const EntityUID& uuIDString, const std::wstring& txt);
        // check if there is an "emotion" tag in the mlif message (xml string)
        // return a string corresponding to the emotion detected in the mlif message
        static std::string checkEmotionTag(std::string mlif);

        virtual bool show();
		virtual bool showBrowser();
		virtual bool showMenu();

    protected:
        void onPageLoaded(Navi* caller, const Awesomium::JSArguments& args);
        void onPageClosed(Navi* caller, const Awesomium::JSArguments& args);
		void onBrowserClosed(Navi* caller, const Awesomium::JSArguments& args);
		void onMenuClosed(Navi* caller, const Awesomium::JSArguments& args);
        void onSendMessage(Navi* caller, const Awesomium::JSArguments& args);
		void onLoadWordUrl(Navi* caller, const Awesomium::JSArguments& args);
		void onLoadMenu(Navi* caller, const Awesomium::JSArguments& args);
        void onSwitchMode(Navi* caller, const Awesomium::JSArguments& args);
		void onEditXmlFile(Navi* caller, const Awesomium::JSArguments& args);
        // FromServer version
        void onNavigateComplete(Navi *caller, const std::string &url, int responseCode);

        NaviState m_curBrowserState;
		NaviState m_curMenuState;

        NaviLibrary::Navi* mNaviBrowser;
        NaviLibrary::Navi* mNaviMenu;

        GUI_Chat();
		~GUI_Chat();

        static GUI_Chat * stGUI_Chat;

   public:
        // select the type of analyse for the sent messages
        enum AnalyseMode {
            AM_EMOTION,         // select the detection of any emotion tags in the sent messages
            AM_GRAMMATICAL      // select the grammatical analyse of the sent messages
        };

        AnalyseMode m_curMode;

		/*le callback que l'on appelle pour recevoir les données */
		static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
		static void readLinguisticChatParameters();
		static std::string trim(const std::string& o);
		static char* textToMlifString(std::string who, const std::wstring& message, AnalyseMode analyseMode);
		static std::string getLinguisticChatPreference (std::string &prefName);

		static std::vector<std::pair<std::string,std::string>> mLinguisticPreferences;
		static std::string mLinguisticChatURL;
		static std::string mEmotionChatURL;
		static std::string mMyName;
	};
} // namespace Solipsis

#endif // #ifndef __GUI_Chat_h__