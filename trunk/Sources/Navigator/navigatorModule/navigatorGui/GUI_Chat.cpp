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
#include <fstream>

#include "Prerequisites.h"
#include "Configuration\NavigatorConfiguration.h"

#include "tinyxml.h"
#include "GUI_Chat.h"
#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>
#include <curl/curl.h>

using namespace Solipsis;
using namespace CommonTools;
using namespace NaviLibrary::NaviUtilities;
using namespace std;

GUI_Chat * GUI_Chat::stGUI_Chat = NULL;

std::vector<std::pair<std::string,std::string>> GUI_Chat::mLinguisticPreferences;
std::string GUI_Chat::mLinguisticChatURL = "";
std::string GUI_Chat::mEmotionChatURL = "";
std::string GUI_Chat::mMyName = "";


//-------------------------------------------------------------------------------------
GUI_Chat::GUI_Chat() : GUI_FromServer("uichat")
{
    m_curBrowserState = NSNotCreated;
	m_curMenuState = NSNotCreated;
	m_curState = NSNotCreated;

    m_curMode = AM_EMOTION;

    mNaviBrowser = NULL;
    mNaviMenu = NULL;

	mLinguisticPreferences.resize(0);
}

GUI_Chat::~GUI_Chat()
{
	stGUI_Chat = NULL;
}

//-------------------------------------------------------------------------------------
void GUI_Chat::showHide()
{
	if (!stGUI_Chat)
	{
		stGUI_Chat = new GUI_Chat();
	}

	if (stGUI_Chat->isVisible())
		stGUI_Chat->hide();
	else
		stGUI_Chat->show();
}

//-------------------------------------------------------------------------------------
bool GUI_Chat::show()
{
    // Create Navi UI Chat panel bar
    // Lua
    if (m_curState == NSNotCreated)
    {
        createNavi(NaviPosition(BottomLeft), 512, 128);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_Chat::onPageLoaded));
        mNavi->loadFile("uichat.html");

        mNavi = NavigatorGUI::getNavi(mPanelName);
        mNavi->hide();
        mNavi->setMovable(true);
        mNavi->setIgnoreBounds(true);
        mNavi->setOpacity(0.75);

        mNavi->bind("pageClosed", NaviDelegate(this, &GUI_Chat::onPageClosed));
        mNavi->bind("sendMessage", NaviDelegate(this, &GUI_Chat::onSendMessage));
		mNavi->bind("loadWordUrl", NaviDelegate(this, &GUI_Chat::onLoadWordUrl));
		mNavi->bind("loadMenu", NaviDelegate(this, &GUI_Chat::onLoadMenu));
        mNavi->bind("switchMode", NaviDelegate(this, &GUI_Chat::onSwitchMode));
        m_curState = NSCreated;
    }
    else
        GUI_Panel::show();

   // FromServer version
   mNavi->addEventListener(this);

    return true;
}

//-------------------------------------------------------------------------------------
bool GUI_Chat::showBrowser()
{
    // Create Navi UI Chat panel bar
    // Lua
	if (m_curBrowserState == NSNotCreated)
    {
        mNaviBrowser = NaviLibrary::NaviManager::Get().createNavi("browser", 572, 440, TopRight);
		mNaviBrowser->loadURL(mLinguisticChatURL.c_str());
	
		mNaviBrowser->setMovable(true);
		mNaviBrowser->setIgnoreBounds(true);
		mNaviBrowser->bind("pageLoaded", NaviDelegate(this, &GUI_Chat::onPageLoaded));
		mNaviBrowser->bind("browserClosed", NaviDelegate(this, &GUI_Chat::onBrowserClosed));
		mNaviBrowser->setOpacity(0.90);
        // test
        mNaviBrowser->zoomOut();
        mNaviBrowser->zoomOut();

		m_curBrowserState = NSCreated;
	}
	else
    {
        if (mNaviBrowser->getVisibility())
            mNaviBrowser->hide(true);
        else
            mNaviBrowser->show(true);
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool GUI_Chat::showMenu()
{
    // Create Navi UI Chat panel bar
    // Lua
	if (m_curMenuState == NSNotCreated)
    {
		mNaviMenu = NaviLibrary::NaviManager::Get().createNavi("menu", 335, 505, TopRight);
		mNaviMenu->loadFile("menuLinguisticChat.html");
	
		mNaviMenu->setMovable(true);
		mNaviMenu->setIgnoreBounds(true);
		mNaviMenu->bind("pageLoaded", NaviDelegate(this, &GUI_Chat::onPageLoaded));
		mNaviMenu->bind("menuClosed", NaviDelegate(this, &GUI_Chat::onMenuClosed));
		mNaviMenu->bind("editXmlFile", NaviDelegate(this, &GUI_Chat::onEditXmlFile));
		mNaviMenu->setOpacity(0.75);

		m_curMenuState = NSCreated;
	}
	else
    {
        if (mNaviMenu->getVisibility())
            mNaviMenu->hide(true);
        else
            mNaviMenu->show(true);
    }

    return true;
}

//-------------------------------------------------------------------------------------
void GUI_Chat::addText(const EntityUID& uuIDString, const std::wstring& message)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorGUI::addChatText()");

	if (stGUI_Chat == 0) // No Chat UI => exit.
		return;

	Navigator* navigator = Navigator::getSingletonPtr();
	// Get the name of the entity by UUID
	string whoStr = navigator->getOgrePeerManager()->getOgrePeer(uuIDString)->getXmlEntity()->getName();
	whoStr = trim(whoStr);

	std::string getLinguisticActivationState = getLinguisticChatPreference(std::string("activate"));
	std::string getEmotionActivationState = getLinguisticChatPreference(std::string("activateEmotion"));

	std::string mlifLinguistic = "";
	std::string mlifEmotional ="";

	if (getEmotionActivationState == "true")
	{
		mlifEmotional = textToMlifString(whoStr, message, Solipsis::GUI_Chat::AM_EMOTION);
        if (mlifEmotional != "false")        // it's a MLIF message type
        {
            // check for an emotion tag in the MLIF message and apply it to the concerned avatar
            std::string emotion = checkEmotionTag(mlifEmotional);
            if (! emotion.empty() || (stGUI_Chat->m_curMode == AM_EMOTION))
            {
				navigator->applyNewEmotion( uuIDString, emotion );
			}
		}
}

	if (getLinguisticActivationState == "true")
	{
		mlifLinguistic = textToMlifString(whoStr, message, Solipsis::GUI_Chat::AM_GRAMMATICAL); 
		//// 20100806 begin
		{
			/*size_t start = mlifLinguistic.find("&lt;");
			size_t end = mlifLinguistic.find("</pre>");
			if ((start != std::string::npos) && (end != std::string::npos))
				if (start > 0 && end > 0)
				{   */
					// exemple de reference :
					//mlif = "<?xml version=\"1.0\" encoding=\"utf-8\"?><MLDC xmlns:mlif=\"http://www.iso.org/ns/MLIF\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"><GroupC><GI><sourceLanguage>en</sourceLanguage><targetLanguage>fr</targetLanguage><creationDate>2010-07-30T04:51:29-05:00</creationDate></GI><MultiC class=\"translation\"><MonoC xml:lang=\"en\" xlink:label=\"clicked_sentence\"><translationRole>sourceLanguage</translationRole><date>2010-07-30T04:51:29-05:00</date><author></author><SegC class=\"sentence\"><SegC class=\"word\" pos=\"PPSS\" xlink:href=\"BaseURL/getSolipsisWordInfo.php?action=getWordInfo&amp;lang=fr&amp;word=I&amp;sentence= I am happy&amp;wordPosition=1&amp;getSynonyms=on&amp;getTranslations=on&amp;getDefinitions=on&amp;user=didi&amp;langFrom=en&amp;langTo=fr&amp;getOriginalSentence=true&amp;closeButton=true\">I</SegC><SegC class=\"word\" pos=\"BEM\" xlink:href=\"BaseURL/getSolipsisWordInfo.php?action=getWordInfo&amp;lang=fr&amp;word=am&amp;sentence= I am happy&amp;wordPosition=2&amp;getSynonyms=on&amp;getTranslations=on&amp;getDefinitions=on&amp;user=didi&amp;langFrom=en&amp;langTo=fr&amp;getOriginalSentence=true&amp;closeButton=true\">am</SegC><SegC class=\"word\" pos=\"JJ\" xlink:href=\"BaseURL/getSolipsisWordInfo.php?action=getWordInfo&amp;lang=fr&amp;word=happy&amp;sentence= I am happy&amp;wordPosition=3&amp;getSynonyms=on&amp;getTranslations=on&amp;getDefinitions=on&amp;user=didi&amp;langFrom=en&amp;langTo=fr&amp;getOriginalSentence=true&amp;closeButton=true\">happy</SegC></SegC></MonoC><MonoC xml:lang=\"fr\"><translationRole>targetLanguage</translationRole><SegC class=\"sentence\"><SegC class=\"word\">Je</SegC><SegC class=\"word\">suis</SegC><SegC class=\"word\">heureux</SegC></SegC></MonoC></MultiC><MultiC class=\"wordInformation\"><MonoC lang=\"en\" xlink:label=\"clicked_word\" xlink:from=\"clicked_sentence\"><SegC class=\"word\"></SegC></MonoC></MultiC></GroupC></MLDC>";
					//mlifLinguistic = mlifLinguistic.substr(start, end-start);

					string sentence = "";
					size_t tag = mlifLinguistic.find("&");
					while (tag >= 0 && tag <= mlifLinguistic.size())
					{
						sentence = mlifLinguistic.substr(tag+1, tag+9);

						if(sentence.find("lt;") == 0) 
						{ 
							mlifLinguistic.replace(tag,4,"<"); 
						}
						if(sentence.find("gt;") == 0) { 
							mlifLinguistic.replace(tag,4,">"); 
						}
						if(sentence.find("amp;amp") == 0) { 
							mlifLinguistic.replace(tag,8,"&amp"); 
						}
						if(sentence.find("quot;") == 0) { 
							mlifLinguistic.replace(tag,6,"\""); 
						}
						tag = mlifLinguistic.find("&",tag+1);
				/*	}*/
				}
		}
	    // 20100806 end
		Awesomium::JSArguments args;
        args.push_back(Awesomium::JSValue(mlifLinguistic));
        args.push_back(Awesomium::JSValue(whoStr));
        stGUI_Chat->mNavi->evaluateJS("addTaggedText(?,?)", args);	
	}
	else
	{
	    // Navi MultiValue will encode the wstring in URI encoded string and add 1 call to decodeURIComponent on it
		// We simply add Text to the chat as it comes 
		Awesomium::JSArguments args;
	    args.push_back(Awesomium::JSValue(StringHelpers::convertWStringToString(message)));
	    args.push_back(Awesomium::JSValue(whoStr));
        stGUI_Chat->mNavi->evaluateJS("addText(?,?)", args);
	}
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onPageLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    GUI_Panel::onPanelLoaded(caller, args);
	// Initialize Linguistic parameters
	readLinguisticChatParameters();
	Configuration linguisticConfiguration;
	linguisticConfiguration.loadConfig("SolipsisConfiguration.xml");
	
	mMyName = linguisticConfiguration.findParam("Login", "")->getValueString();
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onPageClosed(Navi* caller, const Awesomium::JSArguments& args)
{
	mNavi->hide();
	// Temporary remove listener
	mNavi->removeEventListener(this);


	if (mNaviBrowser) mNaviBrowser->hide();
	if (mNaviMenu) mNaviMenu->hide();
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onBrowserClosed(Navi* caller, const Awesomium::JSArguments& args)
{
    mNaviBrowser->hide();
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onMenuClosed(Navi* caller, const Awesomium::JSArguments& args)
{
    mNaviMenu->hide();
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onSendMessage(Navi* caller, const Awesomium::JSArguments& args)
{
    // Reset input
    mNavi->evaluateJS("$('inputChat').value = ''");
    //           -- Send the message
    // navigator:sendMessage(param["msg"])
    Navigator::getSingletonPtr()->sendMessage(args[0].toString());
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onLoadWordUrl(Navi* caller, const Awesomium::JSArguments& args)
{
	showBrowser();

	if(args[0].toString()!="default")
	{
		mNaviBrowser->loadURL(args[0].toString());
	}
	else if(mNaviBrowser->getCurrentLocation() != mLinguisticChatURL)
    {
		mNaviBrowser->loadURL(mLinguisticChatURL.c_str());
    }
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onLoadMenu(Navi* caller, const Awesomium::JSArguments& args)
{
	showMenu();	
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onSwitchMode(Navi* caller, const Awesomium::JSArguments& args)
{
    if (m_curMode == AM_EMOTION)
        m_curMode = AM_GRAMMATICAL;
    else
        m_curMode = AM_EMOTION;
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onEditXmlFile(Navi* caller, const Awesomium::JSArguments& args)
{
    ofstream fichier("NaviLocal/lingchatpref.xml", ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert

    if(fichier)
    {
        fichier << args[0].toString()<< endl;
        fichier.close();
    }
    else
        cerr << "Impossible d'ouvrir le fichier !" << endl;

	// ReRead Linguistic parameters
	readLinguisticChatParameters();
}

//-------------------------------------------------------------------------------------
void GUI_Chat::onNavigateComplete(Navi *caller, const std::string &url, int responseCode)
{
    if (responseCode >= 400 && responseCode < 600)
    {
        if (responseCode == 409)
        {
            //serverCompatibilityError();
        }
        else if (responseCode == 404)
        {
            //mNavi->evaluateJS("window.location.replace('http://www.google.jp');");
            mNavi->evaluateJS("document.write('<body><b>Unable to load the chat!</b></br></br>can't access to the chat or maybe</br>your username is already used.</body>');");
            mNavi->hide(true, 5000);
        }
        else
        {
            //serverError();
        }
    }

    mNavi->setFocus(false);
}

//-------------------------------------------------------------------------------------
struct MemoryStruct {
	char *memory;
	size_t size;
};

//-------------------------------------------------------------------------------------
size_t GUI_Chat::WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)/*le callback que l'on appelle pour recevoir les données */
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;
	
	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

//-------------------------------------------------------------------------------------
void GUI_Chat::readLinguisticChatParameters()
{
	TiXmlDocument doc("NaviLocal/lingchatpref.xml");
	mLinguisticPreferences.clear();

	doc.LoadFile();
    if (doc.Error())
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "File 'NaviLocal/lingchatpref.xml' not found");
		return;
    }

	TiXmlHandle hdl(&doc);
	TiXmlElement* elem = hdl.FirstChild().FirstChildElement("preference").FirstChildElement("option").Element() ;
    while (elem)
    {
		// "synonyms", "definitions","translations","save","language","activate","me","others","linguisticServer"
		mLinguisticPreferences.push_back(make_pair(std::string(elem->Attribute("id")),elem->GetText()));
		elem = elem->NextSiblingElement(); // iteration 
    }

	elem = hdl.FirstChild().FirstChildElement("emotion").FirstChildElement("option").Element() ;
    while (elem)
    {
		// "activateEmotion", "emotionsServer"
		mLinguisticPreferences.push_back(make_pair(std::string(elem->Attribute("id")),elem->GetText()));
		elem = elem->NextSiblingElement(); // iteration 
    }

	// Initialize our static vars
	std::vector<std::pair<std::string,std::string>>::iterator itPrefs = mLinguisticPreferences.begin();
	while (itPrefs != mLinguisticPreferences.end())
	{
		if ((*itPrefs).first == "linguisticServer")
			mLinguisticChatURL = (*itPrefs).second;

		if ((*itPrefs).first == "emotionsServer")
			mEmotionChatURL = (*itPrefs).second;

		itPrefs++;
	}

}

std::string GUI_Chat::getLinguisticChatPreference (std::string &prefName)
{
	std::vector<std::pair<std::string,std::string>>::iterator itPrefs = mLinguisticPreferences.begin();
	while (itPrefs != mLinguisticPreferences.end())
	{
		if ((*itPrefs).first == prefName)
			return (*itPrefs).second;

		itPrefs++;
	}
	return std::string("");
}

//-------------------------------------------------------------------------------------
string GUI_Chat::trim(const string& o) {
    string ret = o;
    const char* chars = "\n\t\v\f\r ";
    ret.erase(ret.find_last_not_of(chars)+1);
    ret.erase(0, ret.find_first_not_of(chars));
    return ret;
}

//-------------------------------------------------------------------------------------
char* GUI_Chat::textToMlifString(string who, const std::wstring& message, Solipsis::GUI_Chat::AnalyseMode analyseMode)
{
    if (mLinguisticPreferences.empty()) 
        return "false";

	std::string resultURL = "";
	std::string utf8text2 = StringHelpers::convertWStringToString(message);
	std::string chaine;

	for(int j=0; j<(int)utf8text2.size(); j++){
		if(utf8text2[j]==' ') chaine+="%20";
		else chaine+=utf8text2[j];
	}
	std::string getLanguage = getLinguisticChatPreference(std::string("language"));

	if (analyseMode == Solipsis::GUI_Chat::AM_EMOTION)
    {
        // exemple de reference :
        //str = "http://talc.loria.fr/~metaverse/detectionWS.php?sentence=I%20am%20happy&lang=english&author=Tarik";
        resultURL = mEmotionChatURL;
        resultURL += "?sentence="+chaine;
        if (getLanguage == "en") resultURL += "&lang=english";
        else if (getLanguage == "fr") resultURL += "&lang=french";
        else resultURL += "&lang=english"; // default value for getLanguage is english
        resultURL += "&author="+who;
    }

	bool forMe = (getLinguisticChatPreference(std::string("me"))=="true" && (who == mMyName));
	bool forOthers = (getLinguisticChatPreference(std::string("others"))=="true" && (who != mMyName));
    // ----------------
	if((analyseMode == Solipsis::GUI_Chat::AM_GRAMMATICAL) && (forOthers || forMe))
	{
		std::string getSavePref= getLinguisticChatPreference(std::string("save"));
		std::string getSynonyms = getLinguisticChatPreference(std::string("synonyms"));
		std::string getTranslations = getLinguisticChatPreference(std::string("translations"));
		std::string getDefinitions = getLinguisticChatPreference(std::string("definitions"));
	
		resultURL = mLinguisticChatURL + "mlifToSecondLife.php?lang=fr&closeButton=true&author="+who+"&langFrom="+getLanguage;
		//resultURL = mLinguisticChatURL + "index.php?lang=fr&closeButton=true&user="+who+"&langFrom=en";

		if(getSavePref == "true") resultURL+= "&save=on";
		if(getSynonyms == "true") resultURL+= "&getSynonyms=on";
		if(getTranslations == "true") resultURL+= "&getTranslations=on";
		if(getDefinitions == "true") resultURL+= "&getDefinitions=on";
		resultURL += "&langTo="+getLanguage;
		resultURL += "&action=getWordInfos";
		resultURL += "&sentence="+chaine;
	}

	if (resultURL == "") // Linguistic is not activated for me or for the others.
		return "false";

	struct MemoryStruct chunk;
		
	chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
	chunk.size = 0;    /* no data at this point */
	
	//CURL -------------------------------------------------------
	CURL *session = curl_easy_init();
	curl_easy_setopt(session, CURLOPT_URL, resultURL.c_str());
	/* send all data to this function  */
	curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, &Solipsis::GUI_Chat::WriteMemoryCallback);/*on définit le callback*/
	/* we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(session, CURLOPT_WRITEDATA, (void *)&chunk);/*on lui passe la structure*/
	/* some servers don't like requests that are made without a user-agent field, so we provide one */
	curl_easy_setopt(session, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_perform(session);
	curl_easy_cleanup(session);

    return chunk.memory;	
}

//-------------------------------------------------------------------------------------
std::string GUI_Chat::checkEmotionTag(std::string mlif)
{
    // 20100802 begin
    if (mlif[0] != '<')
        mlif = "<?xm" + mlif;
    // 20100802 end

    TiXmlDocument doc;
    doc.Parse(mlif.c_str());
    if (doc.Error())
        return "";

    TiXmlElement* elem = doc.FirstChildElement("MLDC")->FirstChildElement("GroupC")->FirstChildElement("MultiC")->FirstChildElement("MonoC");
    if (elem != NULL)
    {
        elem = elem->FirstChildElement("emotion");
        if (elem != NULL)
        {
            elem = elem->FirstChildElement("category");
            if (elem != NULL)
                return elem->Attribute("name");
        }
    }

    // detect the last smiley in the message
    // TODO détecter la présence d'un "smiley" dans le message :          :)   ;)   :D   :P   :(   :'(
    // en prendre en compte que le dernier smiley détecté

    // reset the last emotion pose on the avatar
    // TODO remettre à 0 le poids du/des dernier morph d'émotion appliqué

    return "";
}
//-------------------------------------------------------------------------------------
