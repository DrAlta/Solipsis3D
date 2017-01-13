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
#include "Navigator.h"
#include "NavigatorGui/GUI_MessageBox.h"
#include "navigatorGui/GUI_login.h"
#include "navigatorGui/GUI_ContextMenu.h"
#include "navigatorGui/GUI_MainMenu.h"
#include "navigatorGui/GUI_StatusBar.h"
#include "navigatorGui/GUI_Modeler.h"
#include "navigatorGui/GUI_Avatar.h"
#include "navigatorGui/GUI_AvatarProperties.h"
#include "navigatorGui/GUI_Chat.h"
#include "navigatorGui/GUI_Debug.h"
#include "navigatorGui/GUI_About.h"
#include "navigatorGui/GUI_Commands.h"

// - KH - adding retrieval of SNids from the world server via GUI
#include "navigatorGui/GUI_AuthentWorldServer.h"

#include "NavigatorFrameListener.h"
#include "OgreTools/OgreHelpers.h"
#include <OgreExternalTextureSourceManager.h>
#include "Tools/DebugHelpers.h"
#include <CTSystem.h>
#include <CTStringHelpers.h>
#include <CTLog.h>
#include <CTIO.h>
#include "Navi.h"
#include "Cameras/FirstPersonCameraSupport.h"
#include "Cameras/OrbitalCameraSupport.h"
#include <World/Modeler.h>
#include <AvatarEditor.h>
#include <CharacterManager.h>
#include <CharacterInstance.h>
#include <VoiceEngineManager.h>

#include "Facebook.h"
//#include "TwitterCon.h"

// ASA/LF
#include <windows.h>
#include <process.h>
#include <commdlg.h>
#include <shellapi.h>

// For debugging purpose
//#include <crtdbg.h>
//#include <vld.h>

using namespace Solipsis;
using namespace CommonTools;
using namespace std;

Navigator* Navigator::ms_singletonPtr = 0;

// The following macros set and clear, respectively, given bits
// of the C runtime library debug flag, as specified by a bitmask.
//#ifdef   _DEBUG
//#define  SET_CRT_DEBUG_FIELD(a) \
//            _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
//#define  CLEAR_CRT_DEBUG_FIELD(a) \
//            _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
//#else
//#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
//#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
//#endif

//-------------------------------------------------------------------------------------
Navigator::Navigator(const String name, IApplication* application) :
Instance(name, application),
NodeEventListener(mXmlRpcClient),
mState(SLogin),
mPeerAddress("localhost:8880"),
mLocalWorldAddress("localhost:8660"),
mWorldAddress(""),
mWorldsServerAddress("localhost:8550"),
mWorldsServerTimeoutSec(8),
mLogin("me"),
mPwd(""),
mAuthentType(ATSolipsis),
mFacebookApiKey(""),
mFacebookSecret("695a02e3645bed085e1802c7e9952d73"), // for api key "8d81e4c64ac0039b209c4a53b21ba220"
mFacebookServer("api.facebook.com/restserver.php"),
mFacebookLoginUrl("http://api.facebook.com/login.php"),
mFixedNodeId(""),
mNodeId(""),
mVoIPServerAddress("localhost:30000"),
mVoIPSilenceLevel(5.0),
mVoIPSilenceLatency(5),
mCastShadows(false),
mNavigationInterface(NIMouseKeyboard),
mXmlRpcClient(0),
mOgrePeerManager(0),
mNavigatorGUI(0),
mAvatarEditor(0),
mMaxNaviPickingDistance(10),
mMaxVLCPickingDistance(10),
mMaxVNCPickingDistance(8),
mMaxSWFPickingDistance(8),

//BM ASA added hybridcom plugin
mMaxHybridcomPickingDistance(10),
//EM ASA added hybridcom plugin

mMaxAvatarPickingDistance(10),
mMaxObjectPickingDistance(20),
mRaySceneQuery(0),
mPickedMovable(0),
mUserAvatar(0),
mNavigatorSound(0),
mMainCameraSupportMgr(0),
mModeler(0),
isOnLeftCTRL(false),
isOnRightCTRL(false),
isOnGizmo(false),
mFriendUserDB(0), // - KH - add on to handle SN identities
mUseFacebookSN(false), // - KHbis - new authentication method
mUseTwitterSN(false),
mTwitterConsumerKey("kvnRgUAU0KHTVwTdMIh5tQ"),
mTwitterConsumerSecret("zUF31CetVqFjBfQSXkXQdxBrQKUyxeDygucYs"),
mOauthAccessToken(""),
mOauthAccessTokenSecret(""),
mFacebook(0),
mFacebookUid(""),
// mTwitterCon(0),
mTwitterUid(""),
mGuestAvatarsList(),
mModuloFriend(1),
mShowFriendModeEnabled(false)
{

	   // Send all reports to STDOUT
   //_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
   //_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
   //_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
   //_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
   //_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
   //_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );

   //////////////////////////////////////////////////////////////////

    ms_singletonPtr = this;

    LogHandler::setLogHandler(&mOgreLogger);
    LogHandler::getLogHandler()->setVerbosityLevel(LogHandler::VL_DEBUG);
    String logFilename = IO::getCWD() + "\\" + LogManager::getSingletonPtr()->getDefaultLog()->getName();
    LogHandler::getLogHandler()->setLogFilename(logFilename);

	// - KH - Create the SN Friend List storage
	mFriendUserDB = new FriendUserDB();

    // Lua initialization
    mLuaState = lua_open();
    luaL_openlibs(mLuaState);
    //with LuaPlus    mLuaPlusState = LuaPlus::LuaState::Create(true);
    //with LuaPlus    mLuaState = mLuaPlusState->GetCState();
    loadConfigurationValues();
}

//-------------------------------------------------------------------------------------
Navigator::~Navigator()
{
    if (mState != SLogin)
        disconnect(true);


#ifdef   _DEBUG
	//// Check for memory leaks
	//OutputHeading( "Examine outstanding allocations (dump memory leaks)" );
	//_CrtDumpMemoryLeaks( );
#endif

   // Set the debug-heap flag so that memory leaks are reported when
   // the process terminates. Then, exit.
   //OutputHeading( "Program exits without freeing a memory block" );
   //SET_CRT_DEBUG_FIELD( _CRTDBG_LEAK_CHECK_DF );
    // Stop the node events listener thread
    NodeEventListener::stop();
    NodeEventListener::finalize();

    // Destroy XMLRPC client
    delete mXmlRpcClient;

    // Destroy OgrePeer manager
    delete mOgrePeerManager;

    if (mSceneMgr)
    {
        // Destroy the ray scene query
        if (mRaySceneQuery != 0)
            mSceneMgr->destroyQuery(mRaySceneQuery);
    }

    // Destroy the avatar editor
    delete mAvatarEditor;

    // Destroy the modeler
    delete mModeler;

	// - KH - Destroy the SN Friend List storage
	delete mFriendUserDB;
	mGuestAvatarsList.clear();

    // Destroy the GUI
    delete mNavigatorGUI;

    // Lua finalization
    lua_close(mLuaState);
    //with LuaPlus    LuaPlus::LuaState::Destroy(mLuaPlusState);

    if (mNavigatorSound != 0)
    {
        // Shutdown sound system
        mNavigatorSound->shutdown();
        delete mNavigatorSound;
        mNavigatorSound = 0;
    }

    // Destroy Main Camera Manager
    if (mMainCameraSupportMgr != 0)
        delete mMainCameraSupportMgr;

    mConfiguration.saveConfig();
	String lockf = mMetaverseHome + "metaverse_lock";
	//remove("C:\\WINDOWS\\Temp\\metaverse_lock");
	remove(lockf.c_str());

#ifdef _DEBUG
	// Set the debug-heap flag so that memory leaks are reported when
	// the process terminates. Then, exit.
   //OutputHeading( "Program exits without freeing a memory block" );
   //SET_CRT_DEBUG_FIELD( _CRTDBG_LEAK_CHECK_DF );
#endif

}

//-------------------------------------------------------------------------------------
Navigator* Navigator::getSingletonPtr()
{
    return ms_singletonPtr;
}

//-------------------------------------------------------------------------------------
unsigned int Navigator::getVersion()
{
    return NAVIGATOR_VERSION;
}

//-------------------------------------------------------------------------------------
bool Navigator::isConnected()
{
    return ((mXmlRpcClient != 0) && (mXmlRpcClient->isConnected()));
}

//-------------------------------------------------------------------------------------
Navigator::State Navigator::getState()
{
    return mState;
}

//-------------------------------------------------------------------------------------
void Navigator::setState(State newState)
{
    mState = newState;
}

//-------------------------------------------------------------------------------------
const String& Navigator::getPeerAddress()
{
    return mPeerAddress;
}

//-------------------------------------------------------------------------------------
void Navigator::setPeerAddress(const String& address)
{
    mPeerAddress = address;
    mConfiguration.findParam("PeerAddress")->setValueString(mPeerAddress);

}

//-------------------------------------------------------------------------------------
const String& Navigator::getLocalWorldAddress()
{
    return mLocalWorldAddress;
}

//-------------------------------------------------------------------------------------
void Navigator::setLocalWorldAddress(const String& address)
{
    mLocalWorldAddress = address;
    mConfiguration.findParam("LocalWorldAddress")->setValueString(mLocalWorldAddress);
}

//-------------------------------------------------------------------------------------
const String& Navigator::getWorldAddress()
{
    return mWorldAddress;
}

//-------------------------------------------------------------------------------------
void Navigator::setWorldAddress(const String& address)
{
    mWorldAddress = address;
    mConfiguration.findParam("WorldAddress")->setValueString(mWorldAddress);
}

//-------------------------------------------------------------------------------------
const String& Navigator::getWorldsServerAddress()
{
    return mWorldsServerAddress;
}

//-------------------------------------------------------------------------------------
void Navigator::setWorldsServerAddress(const String& address)
{
    mWorldsServerAddress = address;
    mConfiguration.findParam("WorldsServerAddress")->setValueString(mWorldsServerAddress);
}

//-------------------------------------------------------------------------------------
unsigned short Navigator::getWorldsServerTimeout()
{
    return mWorldsServerTimeoutSec;
}

//-------------------------------------------------------------------------------------
void Navigator::setWorldsServerTimeout(unsigned short timeoutSec)
{
    mWorldsServerTimeoutSec = timeoutSec;
    mConfiguration.findParam("WorldsServerTimeout")->setValueInt(mWorldsServerTimeoutSec);
}

//-------------------------------------------------------------------------------------
const String& Navigator::getLogin()
{
    return mLogin;
}

//-------------------------------------------------------------------------------------
void Navigator::setLogin(const String& login)
{
    mLogin = login;
    mConfiguration.findParam("Login")->setValueString(mLogin);
}

//-------------------------------------------------------------------------------------
const String& Navigator::getPwd()
{
    return mPwd;
}

//-------------------------------------------------------------------------------------
void Navigator::setPwd(const String& pwd, bool bSave)
{
    mPwd = pwd;

    // Password is saved set but can be manually fixed in the configuration file
    if (bSave)
        mConfiguration.findParam("Password")->setProtectedValueString(mPwd);
    else
        mConfiguration.findParam("Password")->setProtectedValueString("");
}

//-------------------------------------------------------------------------------------
AuthentType Navigator::getAuthentType()
{ 
    return mAuthentType;
}

//-------------------------------------------------------------------------------------
void Navigator::setAuthentType(AuthentType authentType)
{
    mAuthentType = authentType;
}

//-------------------------------------------------------------------------------------
const NodeId& Navigator::getFixedNodeId()
{
    return mFixedNodeId;
}

//-------------------------------------------------------------------------------------
void Navigator::setFixedNodeId(const NodeId& nodeId)
{
    mFixedNodeId = nodeId;
    mConfiguration.findParam("FixedNodeId")->setValueString(mFixedNodeId);
}

//-------------------------------------------------------------------------------------
const NodeId& Navigator::getNodeId()
{
    return mNodeId;
}

//-------------------------------------------------------------------------------------
void Navigator::setNodeId(const NodeId& nodeId)
{
    mNodeId = nodeId;
}

//-------------------------------------------------------------------------------------
const String& Navigator::getMediaCachePath()
{
    return mMediaCachePath;
}

//-------------------------------------------------------------------------------------
void Navigator::setMediaCachePath(const String& mediaCachePath)
{
    mMediaCachePath = mediaCachePath;
    mConfiguration.findParam("MediaCachePath")->setValueString(mMediaCachePath);
}

//-------------------------------------------------------------------------------------
const String& Navigator::getVoIPServerAddress()
{
    return mVoIPServerAddress;
}

//-------------------------------------------------------------------------------------
void Navigator::setVoIPServerAddress(const String& address)
{
    mVoIPServerAddress = address;
    mConfiguration.findParam("VoIPServerAddress")->setValueString(mVoIPServerAddress);
}

//-------------------------------------------------------------------------------------
float Navigator::getVoIPSilenceLevel()
{
    return mVoIPSilenceLevel;
}

//-------------------------------------------------------------------------------------
void Navigator::setVoIPSilenceLevel(float VoIPSilenceLevel)
{
    mVoIPSilenceLevel = VoIPSilenceLevel;
    mConfiguration.findParam("VoIPSilenceLevel")->setValueInt(mVoIPSilenceLatency);
}

//-------------------------------------------------------------------------------------
unsigned int Navigator::getVoIPSilenceLatency()
{
    return mVoIPSilenceLatency;
}

//-------------------------------------------------------------------------------------
void Navigator::setVoIPSilenceLatency(unsigned int VoIPSilenceLatencySec)
{
    mVoIPSilenceLatency = VoIPSilenceLatencySec;
    mConfiguration.findParam("VoIPSilenceLatency")->setValueInt(mVoIPSilenceLatency);
}

//-------------------------------------------------------------------------------------
void Navigator::setCastShadows(bool castShadows)
{
    mCastShadows = castShadows;
    mConfiguration.findParam("CastShadows" )->setValueBool(mCastShadows);
}

bool Navigator::getCastShadows()
{
    return mCastShadows;
}

void Navigator::setNavigationInterface(NavigationInterface ni) 
{ 
    mNavigationInterface=ni; 
    mConfiguration.findParam("NavigationInterface", "0")->setValueInt(mNavigationInterface);
}

Navigator::NavigationInterface Navigator::getNavigationInterface() 
{ 
    return mNavigationInterface; 
}

//////// - KH - Adding handling of SN identities
//-------------------------------------------------------------------------------------
// Will be called when the user wants to bring his SN 
void Navigator::computeFacebookFriendList()
{
	//	- get list of friend from Facebook site
	if (mFacebook != 0)
	{
		const string& fcbkUId = mFacebook->getUid();
		// the Facebook instance is created: user is authenticated  and a session is created
		storeFCBKFriendList(fcbkUId, mFacebook->getFriendList());
	}
}

//-------------------------------------------------------------------------------------
// Will be called when the user wants to bring his SN 
void Navigator::computeTwitterFriendList()
{
	//	- login to twitter site
	//	- get list of Friends
	// - store it 
	// - KH - trials with Twitter
	if (!mOauthAccessToken.empty() && !mOauthAccessTokenSecret.empty())
	{
		//const string& twtrUId = mTwitterCon->getUserIdString();
		//storeTWTRFriendList(twtrUId, mTwitterCon->getFriendList());
	}
}


void Navigator::loadConfigurationValues()
{
    mConfiguration.loadConfig("SolipsisConfiguration.xml");

    mPeerAddress = mConfiguration.findParam("PeerAddress", "localhost:8880")->getValueString();
    mLocalWorldAddress = mConfiguration.findParam("LocalWorldAddress", "")->getValueString();
    mWorldAddress = mConfiguration.findParam("WorldAddress", "")->getValueString();

    if (mWorldAddress.empty())
        mWorldAddress = mLocalWorldAddress;

    mWorldsServerAddress = mConfiguration.findParam("WorldsServerAddress", "80.13.207.29:8550")->getValueString();
    mWorldsServerTimeoutSec = mConfiguration.findParam("WorldsServerTimeout", "8")->getValueInt();

    mLogin = mConfiguration.findParam("Login", "")->getValueString();
    mPwd = mConfiguration.findParam("Password", "")->getProtectedValueString();
 
    mFacebookApiKey = mConfiguration.findParam("FacebookApiKey", "8d81e4c64ac0039b209c4a53b21ba220")->getValueString();
    mFacebookSecret = mConfiguration.findParam("FacebookSecret", "695a02e3645bed085e1802c7e9952d73")->getValueString();
    mFacebookServer = mConfiguration.findParam("FacebookServer", "api.facebook.com/restserver.php")->getValueString();
    mFacebookLoginUrl = mConfiguration.findParam("FacebookLoginUrl", "http://api.facebook.com/login.php")->getValueString();

    mFixedNodeId = mConfiguration.findParam("FixedNodeId", "")->getValueString();
    if (!mFixedNodeId.empty())
        mAuthentType = ATFixed;

    mMediaCachePath = mConfiguration.findParam("MediaCachePath", "")->getValueString();
    mVoIPServerAddress = mConfiguration.findParam("VoIPServerAddress", "localhost:30000")->getValueString();
    mVoIPSilenceLevel = mConfiguration.findParam("VoIPSilenceLevel", "5.0")->getValueFloat();
    mVoIPSilenceLatency = mConfiguration.findParam("VoIPSilenceLatency", "5")->getValueInt();

    mCastShadows = mConfiguration.findParam("CastShadows", "false")->getValueBool();
    mNavigationInterface = (NavigationInterface ) mConfiguration.findParam("NavigationInterface", "false")->getValueInt();

	// - KH - add SN identities handling
	// - KHbis- Should save Twitter oauth authentication !!!!
	//mFacebookLogin = mConfiguration.findParam("FacebookLogin","")->getValueString();
	// for hidden passwd, use : valuesgetProtectedValueString instead
	//mFacebookPassword = mConfiguration.findParam("FacebookPassword","")->getValueString();
	//mTwitterLogin = mConfiguration.findParam("TwitterLogin","")->getValueString();
	// for hidden passwd, use : valuesgetProtectedValueString instead
	//mTwitterPassword = mConfiguration.findParam("TwitterPassword","")->getValueString();
	// - KHbis - adding twitter authentication parameters
	mTwitterConsumerKey = mConfiguration.findParam("TwitterConsumerKey", "kvnRgUAU0KHTVwTdMIh5tQ")->getValueString();
	mTwitterConsumerSecret = mConfiguration.findParam("TwitterConsumerSecret", "zUF31CetVqFjBfQSXkXQdxBrQKUyxeDygucYs")->getValueString();
    mOauthAccessToken = mConfiguration.findParam("TwitterOauthAccessToken", "")->getValueString();
    mOauthAccessTokenSecret = mConfiguration.findParam("TwitterOauthAccessTokenSecret", "")->getValueString();

	// For simulation purpose
	mModuloFriend = mConfiguration.findParam("ModuloFriend", "1")->getValueInt();
	// mTwitterAuthorizeUrl = mConfiguration.findParam("TwitterAuthorizeUrl", "http://api.twitter.com/oauth/authorize")->getValueString();

	// BM ASA/LF HybridCom add conf params
	mMetaverseHome = mConfiguration.findParam("MetaverseHome","")->getValueString();
	mMediaServer = mConfiguration.findParam("MediaServer","")->getValueString();
	mAppName = mConfiguration.findParam("AppName","")->getValueString();	
	mStreamName= "as_" + mConfiguration.findParam("MetaverseVid","")->getValueString();

	char *bufferParams = (char *)malloc (256);
	memset(bufferParams, '\0', 256);
	String jmc = mMetaverseHome + "MetaverseRtmpClient.jar";
	//sprintf (bufferParams, " -jar C:\\Development\\HybridCom\\MetaverseRtmpClient.jar %s %s %s %s", 
	//	mMediaServer.c_str(), mAppName.c_str(), mStreamName.c_str(), mMetaverseHome.c_str());
	sprintf (bufferParams, " -jar %s %s %s %s %s", 
		jmc.c_str(), 
		mMediaServer.c_str(), 
		mAppName.c_str(), 
		mStreamName.c_str(),
		mMetaverseHome.c_str());
		
	ShellExecute(NULL, NULL, 
		"java",
		bufferParams, //" -jar C:\\Development\\HybridCom\\MetaverseRtmpClient.jar",
		NULL, SW_SHOWNORMAL);
	
	// EM ASA/LF HybridCom add conf params
}


//-------------------------------------------------------------------------------------
OgrePeerManager* Navigator::getOgrePeerManager()
{
    return mOgrePeerManager;
}

//-------------------------------------------------------------------------------------
NavigatorGUI* Navigator::getNavigatorGUI()
{
    return mNavigatorGUI;
}

//-------------------------------------------------------------------------------------
Modeler* Navigator::getModeler()
{
    return mModeler;
}

//-------------------------------------------------------------------------------------
AvatarEditor* Navigator::getAvatarEditor()
{
    return mAvatarEditor;
}

//-------------------------------------------------------------------------------------
lua_State* Navigator::getLuaState()
{
    return mLuaState;
}

//-------------------------------------------------------------------------------------
void Navigator::setNavigatorLua(NavigatorLua* navigatorLua)
{
    mNavigatorLua = navigatorLua;
}

//-------------------------------------------------------------------------------------
NavigatorLua* Navigator::getNavigatorLua()
{
    return mNavigatorLua;
}

//-------------------------------------------------------------------------------------
Avatar* Navigator::getUserAvatar()
{
    return mUserAvatar;
}

#ifdef DEMO_NAVI1
//-------------------------------------------------------------------------------------
void Navigator::demoNavi1()
{
    static bool active = false;
    if (active) return;
    active = true;

    SceneNode* userAvatarSceneNode = mUserAvatar->getSceneNode();

    // Create a plane
    Plane plane(Vector3::NEGATIVE_UNIT_Z, -1.1);
    MeshManager::getSingleton().createPlane("demoNavi1Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1.5, 1.5, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    // Creates the Video Plane and subsequent NaviMaterial
    Entity* vidEnt = mSceneMgr->createEntity("demoNavi1Video", "demoNavi1Plane");
    vidEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* vidNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(getEntityNaviName(*vidEnt), 512, 512);
    vidNavi->loadURL( "http://www.youtube.com/watch?v=066_q4DIeqk");
    vidNavi->show(true);
    vidNavi->setMaxUPS(15);
    // Awesomium ?
    //vidNavi->setForceMaxUpdate(true);
    vidNavi->setOpacity(0.75f);
    vidEnt->setMaterialName(vidNavi->getMaterialName());
    //http://www.youtube.com/watch?v=ZQcUS4chhc4
    //http://fr.youtube.com/watch?v=u5WIEep8DJg
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1VideoNode", userAvatarSceneNode->getPosition() + Vector3(1, 1.5, 1));
    videoNode->attachObject(vidEnt);
    videoNode->yaw(Degree(45), Node::TS_WORLD);

    // Creates the Text Plane and subsequent NaviMaterial
    Entity* txtEnt = mSceneMgr->createEntity("demoNavi1Text", "demoNavi1Plane");
    txtEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* txtNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(getEntityNaviName(*txtEnt),  512, 512);
    txtNavi->loadURL("lgpl-3.0.txt");
    txtNavi->show(true);
    txtNavi->setMaxUPS(8);
    txtEnt->setMaterialName(txtNavi->getMaterialName());
    SceneNode* txtNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1TextNode", userAvatarSceneNode->getPosition() + Vector3(1, 1.5, 1));
    txtNode->attachObject(txtEnt);
    txtNode->yaw(Degree(-25), Node::TS_WORLD);

    // web knot
    Entity* knotEnt = mSceneMgr->createEntity("demoNavi1WebKnot", "knot.mesh");
    knotEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* knotNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(getEntityNaviName(*knotEnt),  512, 512);
    knotNavi->loadURL("http://www.google.com");
    knotNavi->show(true);
    knotNavi->setMaxUPS(8);
    std::string googleMtlName = knotNavi->getMaterialName();
    MaterialPtr googleMtl = (MaterialPtr)MaterialManager::getSingletonPtr()->getByName(googleMtlName);
    googleMtl->setDepthWriteEnabled(true);
    knotEnt->setMaterialName(googleMtlName);
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoNavi1WebKnotNode", userAvatarSceneNode->getPosition() + Vector3(-3, 1.75, 4));
    node->setScale(Vector3(0.015f, 0.015f, 0.015f));
    node->attachObject(knotEnt);
}
#endif
#ifdef DEMO_NAVI2
//-------------------------------------------------------------------------------------
void Navigator::demoNavi2(const String params)
{
    static bool active = false;
    String url2go("");
    String strPosition("(17, -108, 96.5)");
    Vector3 position;
    std::string::size_type strPos;
    strPos = params.find_first_of(";");
    if (strPos == std::string::npos)
        url2go = params;
    else
    {
        url2go = params.substr(0, strPos);
        if (strPos + 1 < params.length())
            strPosition = params.substr(strPos + 1, params.length() - (strPos + 1));
    }
    if (!OgreHelpers::convertString2Vector3(strPosition, position))
        position = Vector3(17, -108, 96.5);
    if (url2go.length() == 0)
        url2go = "http://fr.youtube.com/watch?v=u5WIEep8DJg";
    if (active)
    {
        NaviLibrary::NaviManager::Get().getNavi("demoNavi2Video")->loadURL(url2go);
        return;
    }
    active = true;

    // Create a plane
    Plane plane(Vector3::NEGATIVE_UNIT_Z, 0);
    MeshManager::getSingleton().createPlane("demoNavi2Plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 4, 4, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    // Creates the Video Plane and subsequent NaviMaterial
    Entity* vidEnt = mSceneMgr->createEntity("demoNavi2Video", "demoNavi2Plane");
    vidEnt->setQueryFlags(QFNaviPanel);
    NaviLibrary::Navi* vidNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(getEntityNaviName(*vidEnt), 512, 512);
    vidNavi->show(true);
    vidNavi->setMaxUPS(15);
    // Awesomium ?
    //vidNavi->setForceMaxUpdate(true);
    vidNavi->setOpacity(0.75f);
    vidEnt->setMaterialName(vidNavi->getMaterialName());
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demo2VideoNode");
    videoNode->attachObject(vidEnt);
    videoNode->setPosition(position);
    videoNode->yaw(Degree(180), Node::TS_WORLD);
    // Add 1 listener to follow URL changes
    vidNavi->addEventListener(this);
    vidNavi->loadURL(url2go);
}
#endif
#ifdef DEMO_VNC
//-------------------------------------------------------------------------------------
void Navigator::demoVNC(const String params)
{
    static bool active = false;

    if (active)
    {
        SceneNode* vncNode = mSceneMgr->getSceneNode("demoVNCNode");
        MovableObject* vncEnt = vncNode->getAttachedObject("demoVNC");
        vncNode->detachObject(vncEnt);
        vncNode->getCreator()->destroyMovableObject(vncEnt);
        vncNode->getCreator()->destroySceneNode(vncNode->getName());
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
        ExternalTextureSource* vncExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc");
        vncExtTextSrc->destroyAdvancedTexture("demoVNCMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        MaterialManager::getSingleton().remove("demoVNCMaterial");
    }
    else
    {
        String url2go("");
        String pwd("");
        std::string::size_type strPos;
        strPos = params.find_first_of(";");
        if (strPos == std::string::npos)
            url2go = params;
        else
        {
            url2go = params.substr(0, strPos);
            if (strPos + 1 < params.length())
                pwd = params.substr(strPos + 1, params.length() - (strPos + 1));
        }

        // Creates the VNC Plane and subsequent NaviMaterial
        Entity* vncEnt = mSceneMgr->createEntity("demoVNC", "demoVNCPlane.mesh");
        vncEnt->setQueryFlags(QFVNCPanel);
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
        ExternalTextureSource* vncExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc");
        vncExtTextSrc->setParameter("address", "vnc://" + url2go);
        vncExtTextSrc->setParameter("password", "vncpwd:" + pwd);
        MaterialManager::getSingleton().create("demoVNCMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        vncExtTextSrc->createDefinedTexture("demoVNCMaterial");
        vncEnt->setMaterialName("demoVNCMaterial");
        SceneNode* vncNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoVNCNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(2.5, 1.5, 0));
        vncNode->attachObject(vncEnt);
        vncNode->yaw(Degree(90), Node::TS_WORLD);
    }

    active = !active;
}
#endif
#ifdef DEMO_VLC
//-------------------------------------------------------------------------------------
void Navigator::demoVLC(const String params)
{
    static bool active = false;

    if (active)
    {
        SceneNode* vlcNode = mSceneMgr->getSceneNode("demoVLCNode");
        MovableObject* vlcEnt = vlcNode->getAttachedObject("demoVLC");
        vlcNode->detachObject(vlcEnt);
        vlcNode->getCreator()->destroyMovableObject(vlcEnt);
        vlcNode->getCreator()->destroySceneNode(vlcNode->getName());
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vlc");
        ExternalTextureSource* vlcExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vlc");
        vlcExtTextSrc->destroyAdvancedTexture("demoVLCMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        MaterialManager::getSingleton().remove("demoVLCMaterial");
    }
    else
    {
        String mrl(params);
        if (mrl.empty()) return;

        // Creates the VLC Plane and subsequent NaviMaterial
        Entity* vlcEnt = mSceneMgr->createEntity("demoVLC", "demoVNCPlane.mesh");
        vlcEnt->setQueryFlags(QFVLCPanel);
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vlc");
        ExternalTextureSource* vlcExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vlc");
        vlcExtTextSrc->setParameter("mrl", mrl);
        vlcExtTextSrc->setParameter("width", "256");
        vlcExtTextSrc->setParameter("height", "256");
        vlcExtTextSrc->setParameter("frames_per_second", "25");
        MaterialManager::getSingleton().create("demoVLCMaterial", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        vlcExtTextSrc->createDefinedTexture("demoVLCMaterial");
        vlcEnt->setMaterialName("demoVLCMaterial");
        SceneNode* vlcNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoVLCNode", mUserAvatar->getSceneNode()->getPosition() + Vector3(2.5, 1.5, 0));
        vlcNode->attachObject(vlcEnt);
        vlcNode->yaw(Degree(90), Node::TS_WORLD);
    }
    active = !active;
}
#endif
#ifdef DEMO_VOICE
//-------------------------------------------------------------------------------------
void Navigator::demoVoice(const String params)
{
    if (params.find("SilenceParams") == 0)
    {
        std::vector<std::string> tokens;
        StringHelpers::tokenize(params, " ", tokens);
        if (tokens.size() != 3)
            return;
        // get voice engine
        IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
        if (voiceEngine != 0)
            voiceEngine->setSilenceParams(atof(tokens[1].c_str()), atoi(tokens[2].c_str()));
        return;
    }

    // set voice server address
    setVoIPServerAddress(params);

    // start/stop speaking
   
	toggleVoIP();
}
#endif

// BM ASA added hybridcom plugin
//-------------------------------------------------------------------------------------
#ifdef DEMO_HYBRIDCOM

void Navigator::demoHybridcom()
{
    static bool active = false;
    if (active) return;
    active = true;

    SceneNode* userAvatarSceneNode = mUserAvatar->getSceneNode();

    // Create a plane
    Plane plane(Vector3::NEGATIVE_UNIT_Z, -1.1);
    MeshManager::getSingleton().createPlane("demoHybridcomPlane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1.5, 1.5, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    // Creates the Video Plane and subsequent HybridcomMaterial
    Entity* vidEnt = mSceneMgr->createEntity("demoHybridcomVideo", "demoHybridcomPlane");
    vidEnt->setQueryFlags(QFHybridcomPanel);
    NaviLibrary::Navi* vidHybridcom = NaviLibrary::NaviManager::Get().createNaviMaterial(getEntityNaviName(*vidEnt), 512, 512);
    vidHybridcom->loadURL( "http://www.dundal.com");
    vidHybridcom->show(true);
    vidHybridcom->setMaxUPS(15);
    // Awesomium ?
    //vidHybridcom->setForceMaxUpdate(true);
    vidHybridcom->setOpacity(0.75f);
    vidEnt->setMaterialName(vidHybridcom->getMaterialName());
    SceneNode* videoNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoHybridcomVideoNode", userAvatarSceneNode->getPosition() + Vector3(1, 1.5, 1));
    videoNode->attachObject(vidEnt);
    videoNode->yaw(Degree(45), Node::TS_WORLD);

    // Creates the Text Plane and subsequent NaviMaterial
    Entity* txtEnt = mSceneMgr->createEntity("demoHybridcomText", "demoHybridcomPlane");
    txtEnt->setQueryFlags(QFHybridcomPanel);
    NaviLibrary::Navi* txtNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(getEntityNaviName(*txtEnt),  512, 512);
    txtNavi->loadURL("lgpl-3.0.txt");
    txtNavi->show(true);
    txtNavi->setMaxUPS(8);
    txtEnt->setMaterialName(txtNavi->getMaterialName());
    SceneNode* txtNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoHybridcomTextNode", userAvatarSceneNode->getPosition() + Vector3(1, 1.5, 1));
    txtNode->attachObject(txtEnt);
    txtNode->yaw(Degree(-25), Node::TS_WORLD);

    // web knot
    Entity* knotEnt = mSceneMgr->createEntity("demoHybridcomWebKnot", "knot.mesh");
    knotEnt->setQueryFlags(QFHybridcomPanel);
    NaviLibrary::Navi* knotNavi = NaviLibrary::NaviManager::Get().createNaviMaterial(getEntityNaviName(*knotEnt),  512, 512);
    knotNavi->loadURL("http://www.dundal.com");
    knotNavi->show(true);
    knotNavi->setMaxUPS(8);
 	std::string dundalMtlName = knotNavi->getMaterialName();
    MaterialPtr dundalMtl = (MaterialPtr)MaterialManager::getSingletonPtr()->getByName(dundalMtlName);
    dundalMtl->setDepthWriteEnabled(true);
    knotEnt->setMaterialName(dundalMtlName);
    SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("demoHybridcomWebKnotNode", userAvatarSceneNode->getPosition() + Vector3(-3, 1.75, 4));
    node->setScale(Vector3(0.015f, 0.015f, 0.015f));
    node->attachObject(knotEnt);
}

#endif
// EM ASA added hybridcom plugin

//-------------------------------------------------------------------------------------
String Navigator::getEntityNaviName(const Entity& entity)
{
    return "WWW_" + entity.getName();
}

//-------------------------------------------------------------------------------------
Entity* Navigator::getNaviEntity(const String& naviName)
{
    if (naviName.find("WWW_") != 0)
        return 0;
    String entityName = naviName.substr(4, naviName.length() - 4);
    if (mSceneMgr->hasEntity(entityName))
        return mSceneMgr->getEntity(entityName);
    return 0;
}

//-------------------------------------------------------------------------------------
void Navigator::resetMousePicking()
{
    mRaySceneQuery->clearResults();
    mClosestDistance = -1.0f;
    mPickedMovable = 0;
}

//-------------------------------------------------------------------------------------
bool Navigator::computeMousePicking(Ray& mouseRay)
{
    mRaySceneQuery->clearResults();
    mRaySceneQuery->setRay(mouseRay);
    mRaySceneQuery->setSortByDistance(true);
    mRaySceneQuery->setQueryTypeMask(SceneManager::ENTITY_TYPE_MASK);
    RaySceneQueryResult &queryResult = mRaySceneQuery->execute();
    mClosestDistance = -1.0f;
    mPickedMovable = 0;
    String movablesList;
    isOnGizmo = false;

    for (RaySceneQueryResult::iterator it = queryResult.begin(); it != queryResult.end(); ++it )
    {
        if (it->movable && (it->distance > 0))
        {
            if (movablesList.length() > 0) movablesList += ", ";
            movablesList += it->movable->getName() + ":" + StringConverter::toString(it->distance) + ":" + StringConverter::toString(it->movable->getQueryFlags());
            // stop checking if we have found a raycast hit that is closer
            // than all remaining entities
            if ((mClosestDistance >= 0.0f) && (mClosestDistance < it->distance))
                break;
            if (mState == SModeling)
            {
                // stop checking if we are too far
                if (it->distance >= mMaxObjectPickingDistance)
                    break;
                // only check this result if its a hit against an entity
                /* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
                //          if ((it->movable != 0) && (it->movable->getMovableType().compare("Entity") == 0))
                if ((it->movable != 0) &&
                    ((it->movable->getMovableType().compare("Entity") == 0) ||
                    (it->movable->getMovableType().compare("ManualObject") == 0)))
                {
                    if (it->movable->getQueryFlags() & QFAvatar)
                        continue;

                    // Gizmo ?
                    else if (mModeler->isOnGizmo())
                    {
                        mModeler->getSelection()->mTransformation->releasedClickForTransformation();
                        //						mModeler->getSelection()->mTransformation->mDragNDrop = Vector3::ZERO;

                        //mModeler->getSelection()->mTransformation->getMode();
                        //mModeler->deselectNode();

                        string name = it->movable->getName();
                        if( name.substr(0,4) == "move")
                        {
                            isOnGizmo = true;
                            mModeler->getSelection()->mTransformation->onClickToTransformObject( queryResult, "moveX","moveY","moveZ" );
                            break;
                        }
                        else if( name.substr(0,6) == "rotate")
                        {
                            isOnGizmo = true;
                            mModeler->getSelection()->mTransformation->onClickToTransformObject( queryResult, "rotateX","rotateY","rotateZ" );
                            break;
                        }
                        else if( name.substr(0,5) == "scale")
                        {
                            isOnGizmo = true;
                            mModeler->getSelection()->mTransformation->onClickToTransformObject( queryResult, "scaleX","scaleY","scaleZ" );
                            break;
                        }
                    }
                    // Object3D
                    else if (it->movable->getQueryFlags() & QFObject)
                    {
                        if( mModeler && !mModeler->isSelectionLocked() )
                        {
                            String name = it->movable->getName();

                            // Link mode ?
                            if (mModeler->isInLinkMode())
                            {
                                Entity* ent = mSceneMgr->getEntity(name);
                                Object3D * obj = mModeler->getSelection()->get3DObject( ent );
                                //if it is a selectionnable object ...
                                if( obj && obj != mModeler->getSelection()->getFirstSelectedObject() ) //if it is not itself ...
                                {
                                    if( ! obj->getShowBoundingBox()) //if this object is seleted :
                                        if(obj->getParent())
                                            obj = obj->getParent();

                                    mModeler->getSelection()->getFirstSelectedObject()->showBoundingBox(false);
                                    mModeler->getSelection()->getFirstSelectedObject()->linkObject( obj, mSceneMgr);
                                    mModeler->getSelection()->getFirstSelectedObject()->showBoundingBox(true);
                                    mModeler->lockLinkMode(false);
                                }
                            }

                            // Object3D ?
                            else
                            {
                                if (!isOnLeftCTRL)
                                    mModeler->deselectNode();

                                mModeler->lockGizmo(false);
                                if (it->movable->getQueryFlags() & QFObject)
                                {
                                    mPickedMovable = it->movable;
                                    Entity* ent = mSceneMgr->getEntity(name);
                                    mModeler->selectNode(ent);
                                }
                            }
                        }

                        // if we found a new closest raycast for this object, update the
                        // mPickedMovable before moving on to the next object.
                        if (OgreHelpers::isEntityHitByMouse(mouseRay, static_cast<Entity*>(it->movable),
                            mClosestDistance,
                            mClosestUV,
                            mClosestTriUV0, mClosestTriUV1, mClosestTriUV2))
                        {
                            if ((it->movable->getQueryFlags() & QFNaviPanel) && (it->distance < mMaxNaviPickingDistance))
                                mPickedMovable = it->movable;
                        }
                    }
                    else
                        mModeler->deselectNode();
                }
                mModeler->lockLinkMode(false);            
            }

            else // SInWorld
            {
                // stop checking if we are too far
                if (it->distance >= std::max(std::max(std::max(mMaxNaviPickingDistance, mMaxVLCPickingDistance), mMaxVNCPickingDistance), mMaxAvatarPickingDistance))
                    break;
                // only check this result if its a hit against an entity
                /* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
                //if ((it->movable != 0) && (it->movable->getMovableType().compare("Entity") == 0))
                if ((it->movable != 0) &&
                    ((it->movable->getMovableType().compare("Entity") == 0) ||
                    (it->movable->getMovableType().compare("ManualObject") == 0)))
                {
                    if (it->movable->getQueryFlags() & QFAvatar)
                    {
                        mPickedMovable = it->movable;
                        break;
                    }                    

                    // if we found a new closest raycast for this object, update the
                    // mPickedMovable before moving on to the next object.
                    if (OgreHelpers::isEntityHitByMouse(mouseRay, static_cast<Entity*>(it->movable),
                        mClosestDistance,
                        mClosestUV,
                        mClosestTriUV0, mClosestTriUV1, mClosestTriUV2))
                    {
                        if (((it->movable->getQueryFlags() & QFNaviPanel) && (it->distance < mMaxNaviPickingDistance)) ||
                            ((it->movable->getQueryFlags() & QFVNCPanel) && (it->distance < mMaxVNCPickingDistance)) ||
                            ((it->movable->getQueryFlags() & QFSWFPanel) && (it->distance < mMaxSWFPickingDistance)) ||
                            ((it->movable->getQueryFlags() & QFVLCPanel) && (it->distance < mMaxVLCPickingDistance)) ||
                            ((it->movable->getQueryFlags() & QFHybridcomPanel) && (it->distance < mMaxHybridcomPickingDistance))							
							)
                            mPickedMovable = it->movable;
                    }
                }
            }
        }
    }

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeMousePicking() found movables %s", movablesList.c_str());
    // if 1 entity hit
    if ((mClosestDistance >= 0.0f) && (mPickedMovable != 0))
    {
        mClosestHitPoint = mouseRay.getPoint(mClosestDistance);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeMousePicking() found movable=%s, mClosestDistance=%.2f, mClosestHitPoint=%s", mPickedMovable->getName().c_str(), mClosestDistance, StringConverter::toString(mClosestHitPoint).c_str());
        return true;
    }

    return false;
}

#if 1 // GILLES MDLR
//-------------------------------------------------------------------------------------
bool Navigator::computeGizmo()
{
    // increase Object Picking Distance
    if(!mModeler->isSelectionEmpty() && mModeler->isOnGizmo())
        mMaxObjectPickingDistance = 100.0;
    else
        mMaxObjectPickingDistance = 20.0;

    // scale and rotate Gizmo 
    if(!mModeler->isSelectionEmpty())
    {
        //Calculate scale
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
        Vector3 camWorldPos = getCameraPtr()->getWorldPosition();
#else
        Vector3 camWorldPos = getCameraPtr()->getDerivedPosition();
#endif
        //Ogre::LogManager::getSingleton().logMessage("Cam World Pos : " + Ogre::StringConverter::toString(camWorldPos));
        Vector3 objSelWorldPos = mModeler->getSelected()->getPosition(true);
        Real distObjToCam = (camWorldPos - objSelWorldPos).length();
        Real gizmoScale = std::min((float)10.0, std::max((float)1.0, distObjToCam/(float)5.0));
        Vector3 currentScale = getSceneMgrPtr()->getSceneNode("NodeSelection")->getScale(); 

        //Calculate rotation
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
        Radian yawGizmo = getSceneMgrPtr()->getSceneNode("NodeSelection")->getWorldOrientation().getYaw();
        Radian yawCam = getCameraPtr()->getWorldOrientation().getYaw();
#else
        Radian yawGizmo = getSceneMgrPtr()->getSceneNode("NodeSelection")->_getDerivedOrientation().getYaw();
        Radian yawCam = getCameraPtr()->getDerivedOrientation().getYaw();
#endif
        // if gizmo is MOVE, then lock it on the camera's orientation
        if(mModeler->isOnGizmo() == 1)
        {
            Radian halfPi(Math::HALF_PI);
            Radian quaterPi(Math::HALF_PI/2.0);
            Quaternion newYawGizmo = Quaternion(yawCam,Vector3::UNIT_Y)*Quaternion(-quaterPi,Vector3::UNIT_Y);
            getSceneMgrPtr()->getSceneNode("NodeSelection")->setOrientation(newYawGizmo);
        } 
        else // else lock it on the selection orientation
        {
            Radian Pi(Math::PI);
            Quaternion orientation = mModeler->getSelected()->getSceneNode()->getOrientation();
            getSceneMgrPtr()->getSceneNode("NodeSelection")->setOrientation(orientation);
            getSceneMgrPtr()->getSceneNode("NodeSelection")->yaw(Pi);
        }
        getSceneMgrPtr()->getSceneNode("NodeSelection")->scale(gizmoScale/currentScale.x,gizmoScale/currentScale.y,gizmoScale/currentScale.z);
       
        return true;
    }
    return false;
}
#endif
//-------------------------------------------------------------------------------------
bool Navigator::is1NaviHitByMouse(String& naviName, int& naviX, int& naviY)
{
    // if 1 Navi entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFNaviPanel))
    {
        Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        naviName = NaviLibrary::NaviManager::Get().getNaviFromMtlName(mtlName)->getName();
        // compute texture coordinates of the hit
        computeNaviHit(naviName,
            mClosestUV,
            mClosestTriUV0, mClosestTriUV1, mClosestTriUV2,
            naviX, naviY);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1NaviHitByMouse() found Navi movable=%s, naviName=%s, (naviX, naviY)=(%d, %d)", mPickedMovable->getName().c_str(), naviName.c_str(), naviX, naviY);
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
void Navigator::computeNaviHit(const String& naviName,
                               Vector2& closestUV,
                               Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                               int& naviX, int& naviY)
{
    // uv computation found into the "Pick" sample of MS Direct SDK
    Vector2 dt1 = closestTriUV1 - closestTriUV0;
    Vector2 dt2 = closestTriUV2 - closestTriUV0;
    Vector2 closestResultUV;
    closestResultUV.x = closestTriUV0.x + closestUV.x*dt1.x + closestUV.y*dt2.x;
    closestResultUV.y = closestTriUV0.y + closestUV.x*dt1.y + closestUV.y*dt2.y;
    // Navi textures are repeated not clamped so bound results to [0..1]
    unsigned short naviWidth, naviHeight;
    NaviLibrary::Navi* navi = NaviLibrary::NaviManager::Get().getNavi(naviName);
    if (navi == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeNaviHit() naviName=%s not found !", naviName.c_str());
        return;
    }
    navi->getExtents(naviWidth, naviHeight);
    naviX = ((int)(closestResultUV.x*naviWidth))%naviWidth;
    naviY = ((int)(closestResultUV.y*naviHeight))%naviHeight;
    //    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeNaviHit() uv=%s, dt1=%s, dt2=%s, closestResultUV=%s", StringConverter::toString(Vector2(closestUV.x, closestUV.y)).c_str(), StringConverter::toString(dt1).c_str(), StringConverter::toString(dt2).c_str(), StringConverter::toString(closestResultUV).c_str());
}

//-------------------------------------------------------------------------------------
bool Navigator::is1SWFHitByMouse(MovableObject*& swfMovableObj, Vector2& swfXY)
{
    // if 1 SWF entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFSWFPanel))
    {
        swfMovableObj = mPickedMovable;

        Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        // compute texture coordinates of the hit
        computeSwfHit(mClosestUV,
            mClosestTriUV0, mClosestTriUV1, mClosestTriUV2,
            swfXY);

        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1SWFHitByMouse() found SWF movable=%s, swfXY=%s", mPickedMovable->getName().c_str(), StringConverter::toString(swfXY).c_str());
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
void Navigator::computeSwfHit(Vector2& closestUV,
                              Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                              Vector2& swfXY)
{
    // uv computation found into the "Pick" sample of MS Direct SDK
    Vector2 dt1 = closestTriUV1 - closestTriUV0;
    Vector2 dt2 = closestTriUV2 - closestTriUV0;

    swfXY.x = closestTriUV0.x + closestUV.x*dt1.x + closestUV.y*dt2.x;
    swfXY.y = closestTriUV0.y + closestUV.x*dt1.y + closestUV.y*dt2.y;

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeSwfHit() uv=%s, dt1=%s, dt2=%s, swfXY=%s", StringConverter::toString(Vector2(closestUV.x, closestUV.y)).c_str(), StringConverter::toString(dt1).c_str(), StringConverter::toString(dt2).c_str(), StringConverter::toString(swfXY).c_str());
}

//-------------------------------------------------------------------------------------
bool Navigator::is1VLCHitByMouse(MovableObject*& vlcMovableObj)
{
    // if 1 Navi entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFVLCPanel))
    {
        vlcMovableObj = mPickedMovable;
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1VLCHitByMouse() found VLC movable=%s", mPickedMovable->getName().c_str());
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::is1VNCHitByMouse(MovableObject*& vncMovableObj, Vector2& vncXY)
{
    // if 1 VNC entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFVNCPanel))
    {
        vncMovableObj = mPickedMovable;
        // compute texture coordinates of the hit
        computeVncHit(mClosestUV,
            mClosestTriUV0, mClosestTriUV1, mClosestTriUV2,
            vncXY);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1VNCHitByMouse() found VNC movable=%s, vncXY=%s", mPickedMovable->getName().c_str(), StringConverter::toString(vncXY).c_str());
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
void Navigator::computeVncHit(Vector2& closestUV,
                              Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                              Vector2& vncXY)
{
    // uv computation found into the "Pick" sample of MS Direct SDK
    Vector2 dt1 = closestTriUV1 - closestTriUV0;
    Vector2 dt2 = closestTriUV2 - closestTriUV0;
    vncXY.x = closestTriUV0.x + closestUV.x*dt1.x + closestUV.y*dt2.x;
    vncXY.y = closestTriUV0.y + closestUV.x*dt1.y + closestUV.y*dt2.y;
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeVncHit() uv=%s, dt1=%s, dt2=%s, vncXY=%s", StringConverter::toString(Vector2(closestUV.x, closestUV.y)).c_str(), StringConverter::toString(dt1).c_str(), StringConverter::toString(dt2).c_str(), StringConverter::toString(vncXY).c_str());
}


//-------------------------------------------------------------------------------------
// BM ASA added hybridcom plugin
bool Navigator::is1HybridcomHitByMouse(String& naviName, int& naviX, int& naviY)
{
    // if 1 Navi entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFHybridcomPanel))
    {
        Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        naviName = NaviLibrary::NaviManager::Get().getNaviFromMtlName(mtlName)->getName();
        // compute texture coordinates of the hit
        computeNaviHit(naviName,
            mClosestUV,
            mClosestTriUV0, mClosestTriUV1, mClosestTriUV2,
            naviX, naviY);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1HybridcomHitByMouse() found Navi movable=%s, naviName=%s, (naviX, naviY)=(%d, %d)", mPickedMovable->getName().c_str(), naviName.c_str(), naviX, naviY);
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
void Navigator::computeHybridcomHit(const String& naviName,
                               Vector2& closestUV,
                               Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2,
                               int& naviX, int& naviY)
{
    // uv computation found into the "Pick" sample of MS Direct SDK
    Vector2 dt1 = closestTriUV1 - closestTriUV0;
    Vector2 dt2 = closestTriUV2 - closestTriUV0;
    Vector2 closestResultUV;
    closestResultUV.x = closestTriUV0.x + closestUV.x*dt1.x + closestUV.y*dt2.x;
    closestResultUV.y = closestTriUV0.y + closestUV.x*dt1.y + closestUV.y*dt2.y;
    // Navi textures are repeated not clamped so bound results to [0..1]
    unsigned short naviWidth, naviHeight;
    NaviLibrary::Navi* navi = NaviLibrary::NaviManager::Get().getNavi(naviName);
    if (navi == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeHybridcomHit() naviName=%s not found !", naviName.c_str());
        return;
    }
    navi->getExtents(naviWidth, naviHeight);
    naviX = ((int)(closestResultUV.x*naviWidth))%naviWidth;
    naviY = ((int)(closestResultUV.y*naviHeight))%naviHeight;
    //    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::computeNaviHit() uv=%s, dt1=%s, dt2=%s, closestResultUV=%s", StringConverter::toString(Vector2(closestUV.x, closestUV.y)).c_str(), StringConverter::toString(dt1).c_str(), StringConverter::toString(dt2).c_str(), StringConverter::toString(closestResultUV).c_str());
}

// EM ASA added hybridcom plugin

//-------------------------------------------------------------------------------------
bool Navigator::is1AvatarHitByMouse(Avatar*& avatar)
{
    // if 1 Avatar entity hit
    if ((mPickedMovable != 0) && (mPickedMovable->getQueryFlags() & QFAvatar))
    {
        // retrieve avatar instance
//         for (OgrePeerManager::OgrePeersMap::iterator ogrePeer = mOgrePeerManager->getOgrePeersIteratorBegin();ogrePeer != mOgrePeerManager->getOgrePeersIteratorEnd();ogrePeer++)
//         {
//             if (ogrePeer->second->getXmlEntity()->getType() != ETAvatar) continue;
//             /* instead of using the TOO big entity's bounding box, we will create 1 ManualObject's bbox smaller */
//             //            if (((Avatar*)ogrePeer->second)->getEntity() != static_cast<Entity*>(mPickedMovable)) continue;
//             Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
//             if (((Avatar*)ogrePeer->second)->getEntity() != pickedEntity) continue;
//             avatar = (Avatar*)ogrePeer->second;
//             if ((avatar == mUserAvatar) && (getCameraMode() == CM1stPersonWithMouse)) continue;
//             LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1AvatarHitByMouse() found Avatar movable=%s, Entity:Uid=%s, Entity:Name=%s", mPickedMovable->getName().c_str(), avatar->getXmlEntity()->getUid().c_str(), avatar->getEntity()->getName().c_str());
//             return true;
//         }

        String pickedName = mPickedMovable->getName();
        String userAvatarName = mUserAvatar->getCharacterInstance()->getUid();

        if (pickedName.find(userAvatarName) != string::npos)
        {
            return true;
        }

        // just compares names
        



    }

    return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::is1ObjectHitByMouse(Object*& object)
{
    // if 1 Object entity hit
    if ((mPickedMovable == 0) || (mPickedMovable->getQueryFlags() & QFObject))
        return false;

    Entity* pickedEntity = static_cast<Entity*>(mPickedMovable->getParentSceneNode()->getAttachedObject(0));
    Object3D* obj = mModeler->getSelection()->get3DObject(pickedEntity);
    object = (Object*)mOgrePeerManager->getOgrePeer(obj->getEntityUID());
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::is1ObjectHitByMouse() found Object movable=%s, Entity:Uid=%s, Entity:Name=%s", mPickedMovable->getName().c_str(), object->getXmlEntity()->getUid().c_str(), pickedEntity->getName().c_str());
    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::initialize()
{
    // call inherited
    if (!Instance::initialize())
        return false;

    // COLOR PICKING
    /*
    mColorPickingRT = TextureManager::getSingleton().createManual("ColorPickingRT", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TextureType::TEX_TYPE_2D, 256, 256, 0, PixelFormat::PF_B8G8R8, TU_RENDERTARGET, NULL);
    /// Set up viewport over entire texture
    RenderTexture *rtt = mColorPickingRT->getBuffer()->getRenderTarget();
    rtt->setAutoUpdated(false);
    //    rtt->addListener(&mColorPickingRTListener);
    Camera* camera = mWindow->getViewport(0)->getCamera();
    // Save last viewport and current aspect ratio
    Viewport* oldViewport = camera->getViewport();
    Real aspectRatio = camera->getAspectRatio();
    Viewport* v = rtt->addViewport(camera);
    v->setClearEveryFrame(false);
    v->setOverlaysEnabled(false);
    v->setBackgroundColour(ColourValue(0, 0, 0, 0));
    // Should restore aspect ratio, in case of auto aspect ratio
    // enabled, it'll changed when add new viewport.
    camera->setAspectRatio(aspectRatio);
    // Should restore last viewport, i.e. never disturb user code
    // which might based on that.
    camera->_notifyViewport(oldViewport);
    */
    // COLOR PICKING

    // Create Main Camera Support Manager
    mMainCameraSupportMgr=new CameraSupportManager(mCamera);

    if (!mNaviSupported)
    {
        // Call connect
        mNodeId = XmlHelpers::convertAuthentTypeToRepr(ATFixed) + mFixedNodeId;
        return connect();
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::initPostOgreCore()
{
    // call inherited
    if (!Instance::initPostOgreCore())
        return false;

    // boot Lua
    DebugHelpers::initLua(mLuaState);
    Lunar<NavigatorLua>::Register(mLuaState);
    if (luaL_loadfile(mLuaState, "lua\\boot.lua") != 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::initPostOgreCore() Unable to load boot.lua, error: %s", lua_tostring(mLuaState, -1));
        return false;
    }
    if (lua_pcall(mLuaState, 0, LUA_MULTRET, 0))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::initPostOgreCore() Unable to run boot.lua, error: %s", lua_tostring(mLuaState, -1));
        return false;
    }


    // Initialize sound system
    mNavigatorSound = new NavigatorSound();
    if (!mNavigatorSound->initialize(mVoIPSilenceLevel, mVoIPSilenceLatency))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::initPostOgreCore() Unable to initialize sound");
        return false;
    }
	
    // Register our sound system as sound handler for "vlc" external texture source plugin
    ExternalTextureSource *vlcExtTextSrc = ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vlc");
    if (vlcExtTextSrc != 0)
    {
        ExternalTextureSourceEx *vlcExtTextSrcEx = dynamic_cast<ExternalTextureSourceEx*>(vlcExtTextSrc);
        vlcExtTextSrcEx->setSoundHandler(mNavigatorSound);
    }

    // Retrieve Media/Cache path (either set by lua either found from cwd)
    if (mMediaCachePath.empty())
        mMediaCachePath = IO::getCWD() + "\\" + IO::retrieveRelativePathByDescendingCWD(std::string("Media\\cache"));

    return true;
}

//-------------------------------------------------------------------------------------
void Navigator::createSceneManager()
{
    // Use the terrain scene manager.
    mSceneMgr = Root::getSingletonPtr()->createSceneManager(ST_EXTERIOR_CLOSE);

    // Create the ray scene query
    mRaySceneQuery = mSceneMgr->createRayQuery(Ray());
}

//-------------------------------------------------------------------------------------
void Navigator::createCamera()
{
    // call inherited
    Instance::createCamera();

    // Set the sound listener camera
    if (mNavigatorSound != 0)
        mNavigatorSound->setSoundListenerCamera(mCamera);
}

//-------------------------------------------------------------------------------------
void Navigator::createFrameListener()
{
    // Navigator frame listener
    mFrameListener = new NavigatorFrameListener(this);
}

//-------------------------------------------------------------------------------------
void Navigator::createScene()
{
    mSceneMgr->setAmbientLight(ColourValue(0.20, 0.20, 0.20));

    // Create OgrePeer manager
    mOgrePeerManager = new OgrePeerManager(mSceneMgr, this);

    // Create the Modeler 
    mModeler = new Modeler(mSceneMgr, mCamera, mOgrePeerManager);
    mModeler->init(mMediaCachePath);

    // Create the avatar editor
    mAvatarEditor = new AvatarEditor(mMediaCachePath, mSceneMgr);
    mAvatarEditor->buildListSAF();
}

//-------------------------------------------------------------------------------------
bool Navigator::createGUI()
{
    if (!Instance::createGUI())
        return false;

    if (!mNaviSupported)
        return true;

    // Create the GUI
    mNavigatorGUI = new NavigatorGUI(this);
    if (!mNavigatorGUI->startup())
        return false;

    GUI_Login::createAndShowPanel();
    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::setWindow(IWindow* w)
{
    if (!Instance::setWindow(w))
        return false;

    return true;
}

//-------------------------------------------------------------------------------------
void Navigator::OgreLogger::log(VerbosityLevel level, const char* msg)
{ 
    if (level > mVerbosity) return;
    OGRE_LOG(std::string(msg));
}

//-------------------------------------------------------------------------------------
bool Navigator::quit()
{
    mFrameListener->requestShutDown();
	//LF
	remove("C:\\Development\\HybridCom\\metaverse_lock");
    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::connect()
{
    if (mLogin.empty() || mNodeId.empty() || mWorldAddress.empty())
        return false;

	// - KH - be sure not trying to connect twice if already connected 
	if (isConnected())
		return true;

    std::string peerHost;
    unsigned short peerPort;
    StringHelpers::getURLHostPort(mPeerAddress, peerHost, peerPort);
    mXmlRpcClient = new NavigatorXMLRPCClient(peerHost, peerPort, 0, "nattempts=2");

    //Try connection

    RefCntPoolPtr<XmlLogin> xmlLogin;
    xmlLogin->setUsername(mLogin);
    std::string worldHost;
    unsigned short worldPort;
    StringHelpers::getURLHostPort(mWorldAddress, worldHost, worldPort);
    xmlLogin->setWorldHost(worldHost);
    xmlLogin->setWorldPort(worldPort);
    xmlLogin->setNodeId(mNodeId);

    bool nodeResponse = mXmlRpcClient->login(*xmlLogin, mNodeId);

    if (!nodeResponse)
    {
        //Failed..
        return false;
    }
    else
    {
        // Connected !

        // Set my node identifier
        mOgrePeerManager->setNodeId(mNodeId);

        // Stop the node events listener thread
        NodeEventListener::stop();
        NodeEventListener::finalize();
        // Start listener
        if (!NodeEventListener::start())
        {
            mXmlRpcClient->logout();
            return false;
        }

        if (mNavigatorGUI != 0)
            mNavigatorGUI->inWorld();

        mState = SInWorld;

        // Create and add all needed Camera Supports to the Main Camera Support Manager
        mMainCameraSupportMgr->addCameraSupport(new FPCameraSupport("DetachedCameraSupport", CMDetached, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new FPCameraSupport("FirstPersonCameraSupport", CM1stPerson, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new FPCameraSupport("FirstPersonWithMouseCameraSupport", CM1stPersonWithMouse, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new OrbitalCameraSupport("ThirdPersonCameraSupport", CM3rdPerson, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new OrbitalCameraSupport("AroundPersonCameraSupport", CMAroundPerson, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new OrbitalCameraSupport("ModelingCameraSupport", CMModeling, mSceneMgr));
        mMainCameraSupportMgr->addCameraSupport(new OrbitalCameraSupport("AroundObjectCameraSupport", CMAroundObject, mSceneMgr));
        return true;
    }
}

//-------------------------------------------------------------------------------------
void Navigator::DisconnectMsgBoxResponse::onResponse(Navi* caller, const Awesomium::JSArguments& args)
{
    if ((args.size() > 0) && (args[0].toString() == "yes"))
        Navigator::getSingletonPtr()->disconnect(true);
}

//-------------------------------------------------------------------------------------
void Navigator::disconnect(bool force)
{
    if (!force)
    {
        // check if there are pending upload
        if (mOgrePeerManager->havePendingUpload())
        {
            GUI_MessageBox::getMsgBox()->show("Pending upload",
                "Still pending upload, are you sure you want to disconnect ?<br/>Uploading entities will be lost !",
                GUI_MessageBox::MBB_YESNO,
                GUI_MessageBox::MBB_QUESTION,
                &disconnectMsgBoxResponse);
            return;
        }
    }

    if (mXmlRpcClient == 0)
        return;

    // voice engine : stop speaking
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if ((voiceEngine != 0) && voiceEngine->isRecording())
    {
        voiceEngine->stopRecording();
        voiceEngine->disconnect();
    }

    // Unload avatar/modeler panels
    NavigatorFrameListener* navigatorFrameListener = (NavigatorFrameListener*)mFrameListener;
    if (mState == SAvatarEdit)
    {
        GUI_Avatar::unload();
        setCameraMode(getLastCameraMode());
    }
    else if (mState == SModeling)
    {
        GUI_Modeler::unload();
        setCameraMode(getLastCameraMode());
    }

    // Stop the node events listener thread
    NodeEventListener::stop();
    NodeEventListener::finalize();

    // Destroy XMLRPC client
    delete mXmlRpcClient;
    mXmlRpcClient = 0;

    // reset the camera mode
    setCameraMode(CMDetached);

    // Suppress all camera supports
    mMainCameraSupportMgr->suppressAllCameraSupports();

    // reset mouse picking
    resetMousePicking();

    // Clean up allocated peers datas
    mOgrePeerManager->cleanUp();
    mUserAvatar = 0;

    // Clean up modeler
    mModeler->cleanUp();

    if (mSceneMgr)
    {
        // destroy the skybox
        mSceneMgr->setSkyBox(false, "");
    }

    if (!force)
    {
        mState = SLogin;
        GUI_Login::createAndShowPanel();
    }
}

//-------------------------------------------------------------------------------------
bool Navigator::contextItemSelected(const String& item)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::contextItemSelected() item=%s", item.c_str());

    if (mNavigatorGUI == 0) return true;

    GUI_ContextMenu::destroyMenu();

    // Perform action associated to item selected
    if (item == "config")
    {
        setCameraMode(CMAroundPerson);
        GUI_Avatar::createAndShowPanel();
    }
    else if (item == "create")
    {
        setCameraMode(CMModeling);
        GUI_Modeler::createAndShowPanel();
    }
    else if (item == "chat")
    {
       GUI_Chat::showHide();
    }
    else if (item == "talk")
    {
        toggleVoIP();
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::sendMessage(const String& message)
{
    if (mXmlRpcClient == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Attempt to send message without XMLRPC client", "Navigator::sendMessage");

    // decode URI encoded string into a wide-char string
    std::wstring messageWStr = NaviUtilities::decodeURIComponent(message);
    // log with locale string
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::sendMessage(%s)", StringHelpers::convertWStringToString(messageWStr).c_str());


    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETActionOnEntity);
    RefCntPoolPtr<XmlAction> xmlAction;
    xmlAction->setSourceEntityUid(mUserAvatar->getXmlEntity()->getUid());
    xmlAction->setTargetEntityUid(mUserAvatar->getXmlEntity()->getUid());
    xmlAction->setType(ATChat);
    xmlAction->setBroadcast(true);
    xmlAction->setDesc(messageWStr);
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlAction));
    std::string xmlResp;
    return mXmlRpcClient->sendEvt(*xmlEvt, xmlResp);

}

//-------------------------------------------------------------------------------------
bool Navigator::addNaviURLUpdatePending(const String& naviName, const String& url)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::addNaviURLUpdatePending(%, %s)", naviName.c_str(), url.c_str());

    mNaviURLUpdatePending[naviName] = url;
    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::sendURLUpdate(const EntityUID& entityUID, const String& naviName, const String& url)
{
    if (mXmlRpcClient == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Attempt to send URL update without XMLRPC client", "Navigator::sendURLUpdate");

    std::wstring actionWStr = StringHelpers::convertStringToWString(naviName + "," + url);
    // log with locale string
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::sendURLUpdate() %s", StringHelpers::convertWStringToString(actionWStr).c_str());


    RefCntPoolPtr<XmlEvt> xmlEvt;
    xmlEvt->setType(ETActionOnEntity);
    RefCntPoolPtr<XmlAction> xmlAction;
    xmlAction->setSourceEntityUid(mUserAvatar->getXmlEntity()->getUid());
    xmlAction->setTargetEntityUid(entityUID);
    xmlAction->setType(ATURLUpdate);
    xmlAction->setBroadcast(true);
    xmlAction->setDesc(actionWStr);
    xmlEvt->setDatas(RefCntPoolPtr<XmlData>(xmlAction));
    std::string xmlResp;
    return mXmlRpcClient->sendEvt(*xmlEvt, xmlResp);

}

//-------------------------------------------------------------------------------------

void Navigator::onPeerNew(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
	if (xmlEntity.isNull())
		return;
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::onPeerNew() uid:%s", xmlEntity->getUid().c_str());

#ifdef UIDEBUG
    GUI_Debug::setTreeDirty(true);
#endif

    if (!mOgrePeerManager->load(xmlEntity))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::onPeerNew() Unable to load entity !");
    }
}

//-------------------------------------------------------------------------------------

void Navigator::onPeerLost(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
	if (xmlEntity.isNull())
		return;

	const String& uid = xmlEntity->getUid().c_str();
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::onPeerLost() uid:%s", uid);

	// First : Delete avatar from local guest avatars List
	for (list<Avatar*>::iterator guestAvatar_ptr = mGuestAvatarsList.begin(); guestAvatar_ptr != mGuestAvatarsList.end(); ++guestAvatar_ptr){
		// Get vId equivalent for snid and check if present in current pair elt
		Avatar* gAvatar_ptr(*guestAvatar_ptr);
		RefCntPoolPtr<XmlEntity>& gxmlEntity = gAvatar_ptr->getXmlEntity();
		const String& gUid = gxmlEntity->getUid().c_str();
		if (uid.compare(gUid) == 0){
			mGuestAvatarsList.erase(guestAvatar_ptr);
			break;
		}
	}

	// 2.nd Remove associated peer
    if (!mOgrePeerManager->remove(xmlEntity->getUid()))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR,  "Navigator::onPeerLost() Unable to remove lost peer %s !", uid);
        //     throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to remove lost peer " + xmlEntity->getUid() + " !", "Navigator::onPeerLost");
    }
}

//-------------------------------------------------------------------------------------

void Navigator::onPeerUpdated(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
	if (xmlEntity.isNull())
		return;

	if (!mOgrePeerManager->updateEntity(xmlEntity))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR,  "Navigator::onPeerUpdated() Unable to update peer %s !", xmlEntity->getUid().c_str());
        //     throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to remove lost peer " + xmlEntity->getUid() + " !", "Navigator::onPeerUpdated");
    }
	       
	LOGHANDLER_LOGF(LogHandler::VL_DEBUG,  "Navigator::onPeerUpdated() : update peer %s !", xmlEntity->getUid().c_str());
}

//-------------------------------------------------------------------------------------

void Navigator::onPeerAction(RefCntPoolPtr<XmlAction>& xmlAction)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::onPeerAction()");

    if (!mOgrePeerManager->action(xmlAction))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR,  "Navigator::onPeerAction() Unable to do action on peer %s !", xmlAction->getSourceEntityUid().c_str());
        //     throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to do action on peer " + xmlEntity->getUid() + " !", "Navigator::onPeerAction");
    }
}

//-------------------------------------------------------------------------------------
void Navigator::onLocationChange(Navi *caller, const std::string &url)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::onLocationChange(%s, %s)", caller->getName().c_str(), url.c_str());

#ifdef DEMO_NAVI2
    if (caller->getName() == "demoNavi2Video")
        GUI_Debug::refreshUrl();
#endif

    std::map<String, String>::iterator it = mNaviURLUpdatePending.find(caller->getName());
    if (it != mNaviURLUpdatePending.end())
    {
        mNaviURLUpdatePending.erase(it);
        return;
    }
    Entity* entity = getNaviEntity(caller->getName());
    if (entity == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::onLocationChange() No entity found for navi %s !", caller->getName().c_str());
        return;
    }
    Object3D* object3D = mModeler->getSelection()->get3DObject(entity);
    if (object3D == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::onLocationChange() No object3D found for navi %s !", caller->getName().c_str());
        return;
    }
    Object* object = (Object*)mOgrePeerManager->getOgrePeer(object3D->getEntityUID());
    if (object == 0)
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::onLocationChange() No object found for navi %s !", caller->getName().c_str());
        return;
    }
    sendURLUpdate(object->getXmlEntity()->getUid(), caller->getName(), url);
}

//-------------------------------------------------------------------------------------
void Navigator::processEvents()
{
    //    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::processEvents()");

    // Process each event
    NodeEventListener::EvtsList* nodeEvents = beginProcessEvents();
    for (NodeEventListener::EvtsList::iterator xmlEvt = nodeEvents->begin();xmlEvt != nodeEvents->end();++xmlEvt)
    {
        switch ((*xmlEvt)->getType())
        {
        case ETNewEntity:
            onPeerNew(RefCntPoolPtr<XmlEntity>((*xmlEvt)->getDatas()));
            break;
        case ETLostEntity:
            onPeerLost(RefCntPoolPtr<XmlEntity>((*xmlEvt)->getDatas()));
            break;
        case ETUpdatedEntity:
            onPeerUpdated(RefCntPoolPtr<XmlEntity>((*xmlEvt)->getDatas()));
            break;
        case ETActionOnEntity:
            onPeerAction(RefCntPoolPtr<XmlAction>((*xmlEvt)->getDatas()));
            break;
        case ETConnectionLost:
            mNavigatorGUI->connectionLostError();
            if (mUserAvatar)
                mUserAvatar->setGhost(true);
            break;
        case ETConnectionRestored:
            if (mUserAvatar)
                mUserAvatar->setGhost(false);
            break;
        case ETConnectionFailed:
            mNavigatorGUI->connectionServerError();
        default: // Caller already check type consistency
            break;
        }

    }
    endProcessEvents();
}

//-------------------------------------------------------------------------------------
void Navigator::sendEvents()
{
    //    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Navigator::sendEvents()");

    if (mXmlRpcClient == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "Attempt to send events without XMLRPC client", "Navigator::sendEvents");

    // Send each event
    std::string xmlResp;
    OgrePeerManager::EvtsList& evtsList = mOgrePeerManager->getEvtsToSendList();
    for (OgrePeerManager::EvtsList::iterator xmlEvt = evtsList.begin(); xmlEvt != evtsList.end(); ++xmlEvt)
    {
        if (!mXmlRpcClient->sendEvt(*(*xmlEvt), xmlResp))
            LOGHANDLER_LOGF(LogHandler::VL_ERROR, "Navigator::sendEvents() Unable to send event !");
    }
    evtsList.clear();
}

//-------------------------------------------------------------------------------------
void Navigator::onAvatarNodeCreate(OgrePeer* ogrePeer)
{
    // User Avatar ?
    if (ogrePeer->isLocal())
    {
        mUserAvatar = (Avatar*)ogrePeer;

        // Set the ThirdPersonCam as active
        setCameraMode(CM3rdPerson);
	} else {
		// - KH - store guest avatars in a local list for future SN management purpose
		mGuestAvatarsList.push_back((Avatar*)ogrePeer);
	}
}

//-------------------------------------------------------------------------------------
void Navigator::onSceneNodeCreate(OgrePeer* ogrePeer)
{
    // create the sun light
    Light *sunLight = mSceneMgr->createLight("SunLight");
    sunLight->setType(Light::LT_DIRECTIONAL);
    Vector3 sunLightDir = Vector3(0.5f, -1.0f, 0.25f);
    sunLightDir.normalise();
    sunLight->setDirection(sunLightDir);
    sunLight->setDiffuseColour(ColourValue::White);
    sunLight->setSpecularColour(ColourValue::White);
    sunLight->setCastShadows(getCastShadows());
}

//-------------------------------------------------------------------------------------
void Navigator::onSceneNodeDestroy(OgrePeer* ogrePeer)
{
    // destroy the sun light
    if (mSceneMgr->hasLight("SunLight"))
        mSceneMgr->destroyLight("SunLight");
}

//-------------------------------------------------------------------------------------
bool Navigator::startModeling()
{
    mState = SModeling;

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::endModeling()
{	
    // Go back in world
    mState = SInWorld;

    // Save all owned objects
    mdlrXMLSave(true);

    if (mModeler)
        mModeler->deselectNode();

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::createPrimitive(Object3D::Type type)
{
    Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
    Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
    Radian angle = pldir.getYaw();
    Ogre::Vector3 dep = Vector3(1.5,0,0);

    Real cosY = Math::Cos(angle);
    Real sinY = Math::Sin(angle);

    Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
    //y = point.y;							//		y' = y  
    dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
    dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
    return mModeler->createPrimitive(type, entityUID, entityUID, plpos + dep, pldir, true);
}

//-------------------------------------------------------------------------------------
#ifdef DECLARATIVE_MODELER
bool Navigator::createSceneFromText( const std::string& s, std::string& errMsg, std::string& warnMsg )
{
    Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
    Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
    Radian angle = pldir.getYaw();
    Ogre::Vector3 dep = Vector3(1.5,0,0);

    Real cosY = Math::Cos(angle);
    Real sinY = Math::Sin(angle);

    Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
    //y = point.y;							//		y' = y  
    dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
    dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
    //String name = XmlHelpers::convertUIntToHexString(entityUID);

    return mModeler->createSceneFromText( entityUID, entityUID, plpos + dep, pldir, s, errMsg, warnMsg );
}

#endif

//-------------------------------------------------------------------------------------
#ifdef TERRAIN_MODELER
bool Navigator::createTerrain(double steepness,double noiseScale,double granularity)
{
	Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
	Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
	Radian angle = pldir.getYaw();
	Ogre::Vector3 dep = Vector3(1.5,0,0);

	Real cosY = Math::Cos(angle);
	Real sinY = Math::Sin(angle);

	Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
	//y = point.y;							//		y' = y  
	dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
	dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
    //String name = XmlHelpers::convertEntityUIDToHexString(entityUID);
	return mModeler->createTerrain(entityUID, entityUID, plpos + dep,steepness,noiseScale,granularity);
}
#endif

//-------------------------------------------------------------------------------------
bool Navigator::createMesh()
{
    Vector3 plpos = mUserAvatar->getSceneNode()->getPosition();
    Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
    Radian angle = pldir.getYaw();
    Ogre::Vector3 dep = Vector3(1.5,0,0);

    Real cosY = Math::Cos(angle);
    Real sinY = Math::Sin(angle);

    Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
    //y = point.y;							//		y' = y  
    dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
    dep.x = x;

    EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
    return mModeler->createMesh(entityUID, entityUID, plpos + dep, pldir);
}

//-------------------------------------------------------------------------------------
bool Navigator::startAvatarEdit()
{
    mState = SAvatarEdit;

    return true;
}

//-------------------------------------------------------------------------------------
bool Navigator::endAvatarEdit()
{	
    // Go back in world
    mState = SInWorld;

    // Save avatar
    avatarXMLSave();

    // Attach all camera supports to the new avatar 
    mMainCameraSupportMgr->attachAllCameraSupportsToNode(mUserAvatar->getSceneNode());

    return true;
}

//-------------------------------------------------------------------------------------
void Navigator::onMouseMoved(const MouseEvt& evt)
{
}

//-------------------------------------------------------------------------------------
void Navigator::onMousePressed(const MouseEvt& evt)
{
}

//-------------------------------------------------------------------------------------
void Navigator::onMouseReleased(const MouseEvt& evt)
{
    if (mModeler && !mModeler->isSelectionEmpty())
    {
        if (mModeler->isOnGizmo())
        {
            if(isOnGizmo) 
                isOnGizmo = false;
        }
    }
}

//-------------------------------------------------------------------------------------
void Navigator::MdlrModifGizmo(Vector3 dep)
{
    SceneNode* node = mSceneMgr->getSceneNode("NodeSelection");
    Vector3 vec;

#if 1 // GILLES MDLR
    if(mModeler->getSelected() == NULL)
        return;
#endif

    switch (mModeler->getSelection()->mTransformation->getMode())
    {
    case Transformations::MOVE:
        mModeler->updateCommand( Object3D::TRANSLATE, mModeler->getSelected() );
        mModeler->getSelection()->move(dep.x, dep.y, dep.z);
        node->translate(dep);
        break;
    case Transformations::ROTATE:
        mModeler->updateCommand( Object3D::ROTATE, mModeler->getSelected() );
        dep *= 5.;
        //vec = mModeler->getSelected()->getRotate();
        //vec += dep;
        //mModeler->getSelection()->rotate(vec.x, vec.y, vec.z);
        mModeler->getSelection()->rotate(dep.x, dep.y, dep.z);
        break;
    case Transformations::SCALE:
        mModeler->updateCommand( Object3D::SCALE, mModeler->getSelected() );

        vec = mModeler->getSelected()->getScale();
        //vec = Vector3::UNIT_SCALE;

        vec += dep;
        //mModeler->getSelection()->scaleTo(vec.x, vec.y, vec.z);
        mModeler->getSelection()->scale(vec.x, vec.y, vec.z);
        break;
    case Transformations::SELECT:
        break;
    }
}

//-------------------------------------------------------------------------------------
bool Navigator::mdlrXMLImport()
{
    if (mModeler)
    {
        Quaternion pldir = mUserAvatar->getSceneNode()->getOrientation();
        Radian angle = pldir.getYaw();
        Ogre::Vector3 dep = Vector3(2.0,0,0);

        Real cosY = Math::Cos(angle);
        Real sinY = Math::Sin(angle);

        Real x = dep.x * cosY + dep.z * sinY;	//		x' = x*cos(a) + z*sin(a)  
        //y = point.y;							//		y' = y  
        dep.z = -dep.x * sinY + dep.z * cosY;	//		z' = -x*sin(a) + z*cos(a)
        dep.x = x;

        EntityUID entityUID = mOgrePeerManager->getNewEntityUID();
        //return mModeler->XMLImport(entityUID, entityUID, "", mUserAvatar->getSceneNode()->getPosition() + dep );
        return mModeler->XMLImport(entityUID, entityUID, "", mUserAvatar->getSceneNode()->getPosition() + dep, mUserAvatar->getSceneNode()->getOrientation() );
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::mdlrXMLSave(bool all)
{
    if (mModeler)
        if (all || !mModeler->isSelectionEmpty())
            return mModeler->XMLSave(all);
        else
            GUI_MessageBox::getMsgBox()->show("Modeler information",
                GUI_Modeler::ms_ModelerErrors[GUI_Modeler::ME_NOOBJECTSELECTED],
                GUI_MessageBox::MBB_OK,
                GUI_MessageBox::MBB_INFO);
    return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::mdlrXMLSaveAs(const String& pDestination)
{
    if (mModeler)
        if (!mModeler->isSelectionEmpty())
            return mModeler->XMLSaveAs( pDestination );
        else
            //mNavigatorGUI->showMessageBox("Modeler information", NavigatorGUI::ms_ModelerErrors[NavigatorGUI::ME_NOOBJECTSELECTED], NavigatorGUI::MBB_OK, NavigatorGUI::MBB_INFO);
            GUI_MessageBox::getMsgBox()->show("Modeler information",
                GUI_Modeler::ms_ModelerErrors[GUI_Modeler::ME_NOOBJECTSELECTED],
                GUI_MessageBox::MBB_OK, 
                GUI_MessageBox::MBB_INFO);
    return false;
}

//-------------------------------------------------------------------------------------
bool Navigator::avatarXMLLoad()
{
    if (mAvatarEditor)
        //if( mAvatarEditor->XMLLoad() )
    {
        // 0. store the old avatar' parameters (camera mode ...)
        Quaternion dir = mUserAvatar->getSceneNode()->getOrientation();
        Vector3 pos = mUserAvatar->getSceneNode()->getPosition();

        // 1. remove the current avatar' mesh + skeleton
        // -> it's not necessary to unload because this avatar can be used by another user in the current scene !
        // -> so just remove the Entity / Mesh from the SceneNode

        // 2. update the avatar' mesh + skeleton

        // 3. update his position & orientation

        // 4. set the old parameters to the new avatar (camera mode ...)

        return true;
    }

    return false;
}
//-------------------------------------------------------------------------------------
bool Navigator::avatarXMLSave()
{
    if (mAvatarEditor)
    {
        mAvatarEditor->getManager()->getCurrentInstance()->saveModified();
        mOgrePeerManager->onUserAvatarSave();
    }

    return true;
}
//-------------------------------------------------------------------------------------
bool Navigator::avatarXMLSaveAs()
{
    if (mAvatarEditor)
    {}

    return true;
}

//-------------------------------------------------------------------------------------
void Navigator::setCameraMode(int mode)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "NavigatorFrameListener::setCameraMode(%d)", mode);

    if (mode == mMainCameraSupportMgr->getActiveCameraSupportIndex() && (mode != CMAroundPerson && mode != CMModeling && mode != CM3rdPerson)) 
        return;

    if (((mState == SAvatarEdit) && (mMainCameraSupportMgr->getActiveCameraSupportIndex() == CMAroundPerson)) ||
        ((mState == SModeling) && (mMainCameraSupportMgr->getActiveCameraSupportIndex() == CMModeling)))
        return;

    if (mUserAvatar == 0) return;
    Vector3 pos;
    Quaternion orientation;
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
    pos = mUserAvatar->getSceneNode()->getWorldPosition();
    orientation = mUserAvatar->getSceneNode()->getWorldOrientation();
#else
    pos = mUserAvatar->getSceneNode()->_getDerivedPosition();
    orientation = mUserAvatar->getSceneNode()->_getDerivedOrientation();
#endif
    Vector3 size = mUserAvatar->getEntity()->getBoundingBox().getSize();

    switch (mode)
    {
    case CMDetached:
        {
            mMainCameraSupportMgr->activeCameraSupport(CMDetached);
            break;
        }
    case CM1stPerson:
        {
            FPCameraSupport* FPSupportCam = (FPCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CM1stPerson);
            FPSupportCam->resetCameraSupport();
            CameraSupport* currentCamera=mMainCameraSupportMgr->getActiveCameraSupport();

            if (mMainCameraSupportMgr->getActiveCameraSupportIndex()!=CM1stPersonWithMouse)
            {
                // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
                FPSupportCam->yaw(Radian(-Math::PI/2));
            }
            else
            {
                // Set the camera orientation as the last First Person Camera with mouse
                FPSupportCam->yaw(currentCamera->getYaw());
                FPSupportCam->pitch(currentCamera->getPitch());
            }

            // Translate the origin of the camera support along the y axis to the eyes of the avatar (85% of the bbox)
            FPSupportCam->setCameraSupportNodePosition(Ogre::Vector3(0.0, 0.85*size.y, 0));

            mMainCameraSupportMgr->activeCameraSupport(CM1stPerson);
            mUserAvatar->setMvtType(Avatar::MT1stPerson);
            break;
        }
    case CM1stPersonWithMouse:
        {
            FPCameraSupport* FPWMSupportCam = (FPCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CM1stPersonWithMouse);
            FPWMSupportCam->resetCameraSupport();
            CameraSupport* currentCamera=mMainCameraSupportMgr->getActiveCameraSupport();

            if (mMainCameraSupportMgr->getActiveCameraSupportIndex()!=CM1stPerson)
            {
                // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
                FPWMSupportCam->yaw(Radian(-Math::PI/2)); 
            }
            else
            {   
                // Set the camera orientation as the last First Person Camera
                FPWMSupportCam->yaw(currentCamera->getYaw());
                FPWMSupportCam->pitch(currentCamera->getPitch());
            }
            // Translate the origin of the camera support along the y axis to the eyes of the avatar (85% of the bbox)  
            FPWMSupportCam->setCameraSupportNodePosition(Ogre::Vector3(0.0, 0.85*size.y, 0));

            mMainCameraSupportMgr->activeCameraSupport(CM1stPersonWithMouse);
            mUserAvatar->setMvtType(Avatar::MT1stPerson);
            break;
        }
    case CM3rdPerson:
        {
            mMainCameraSupportMgr->activeCameraSupport(CM3rdPerson);
            OrbitalCameraSupport* TPSupportCam = (OrbitalCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CM3rdPerson);
            TPSupportCam->resetCameraSupport();
            // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
            TPSupportCam->yaw(Radian(-Math::PI/2));
            // Translate the origin of the camera support along the y axis to the middle of the avatar bbox
            TPSupportCam->setCameraSupportNodePosition(Ogre::Vector3(0.0, 0.5*size.y, 0));
            TPSupportCam->pitch(Degree(-15.0));
            TPSupportCam->setDistanceFromTarget(2.0*size.y);
            mUserAvatar->setMvtType(Avatar::MT3rdPerson);
            break;
        }

    case CMAroundPerson:
        {
            mMainCameraSupportMgr->activeCameraSupport(CMAroundPerson);
            OrbitalCameraSupport* APSupportCam = (OrbitalCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CMAroundPerson);
            APSupportCam->resetCameraSupport();
            mCamera->setOrientation(Quaternion::IDENTITY);
            // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
            APSupportCam->yaw(Radian(-Math::PI/2));
            // Put camera on the front of the avatar 
            APSupportCam->yaw(Radian(Math::PI));
            // Translate the origin of the camera support along the y axis to the middle of the avatar bbox
            APSupportCam->translateCameraSupport(0.0, 0.5*size.y, 0.0);
            APSupportCam->pitch(Degree(-15.));
            APSupportCam->setDistanceFromTarget(2*size.y);
            mUserAvatar->setMvtType(Avatar::MT3rdPerson);
            break;
        }

    case CMModeling:
        {
            mMainCameraSupportMgr->activeCameraSupport(CMModeling);
            OrbitalCameraSupport* MSupportCam = (OrbitalCameraSupport*)mMainCameraSupportMgr->getCameraSupport(CMModeling);
            MSupportCam->resetCameraSupport();
            // Translate the origin of the camera support along the y axis to the middle of the avatar bbox, and 1,5m ahead
            MSupportCam->translateCameraSupport(DIST_AVATAR_OBJECT, 0.5*size.y, 0.0);
            // As x-axis is in front of the avatar, we need to put the z-axis of the camera along it
            MSupportCam->yaw(Radian(-Math::PI/2));
            // Put camera on the front of the avatar (45�)
            MSupportCam->yaw(Degree(45.0));
            MSupportCam->pitch(Degree(-35.));
            MSupportCam->setDistanceFromTarget(2.5*size.y);
            mUserAvatar->setMvtType(Avatar::MT3rdPerson);
            break;
        }
    case CMAroundObject:
        {
            break;
        }
    }
    bool userAvatarVisible = ((mode == CM3rdPerson) || (mode == CMAroundPerson) || (mode == CMModeling));
    mUserAvatar->getEntity()->setVisible(userAvatarVisible);
    mUserAvatar->enableDisableSelection(userAvatarVisible);
    //mUserAvatar->setNameVisibility(mode == CM3rdPerson || mode == CMAroundPerson || mode == CMModeling);
    if (mNavigatorGUI != 0)
    {
        // Hide mouse only on 1st person camera mode
        mNavigatorGUI->SetMouseVisibility(mode != CM1stPerson);
        // Set mouse exclusive mode in windowed mode (exclusive only on 1st person camera mode)
        if (!mIWindow->isFullscreen())
            mIWindow->setMouseExclusive(mode == CM1stPerson);

        GUI_MainMenu::showHide(mode != CM1stPerson);

        NaviManager::Get().deFocusAllNavis();
        if (mode == CM1stPerson)
            GUI_StatusBar::setStatusBarText("Press 2,3 or 4 to return to a view with mouse ...");
        else if (mode == CMAroundPerson || mode == CMModeling)
            GUI_StatusBar::setStatusBarText("Click/Drag middle button to rotate ...");
    }
}

//-------------------------------------------------------------------------------------
void Navigator::toggleVoIP()
{
    // get voice engine
    IVoiceEngine* voiceEngine = VoiceEngineManager::getSingleton().getSelectedEngine();
    if (voiceEngine == 0)
        return;

    // start/stop recording
    if (voiceEngine->isRecording())
    {
        voiceEngine->stopRecording();
        voiceEngine->disconnect();
    }
    else
    {
        // connect to voice server
        EntityUID avatarUid = getUserAvatar()->getCharacterInstance()->getUid();
        std::string voipSrvHost;
        unsigned short voipSrvPort;
        StringHelpers::getURLHostPort(getVoIPServerAddress(), voipSrvHost, voipSrvPort);
        bool connectionSuccess = voiceEngine->connect(voipSrvHost.c_str(), voipSrvPort, avatarUid);
        if (connectionSuccess)
        {
            voiceEngine->startRecording();
        }
        else
        {
            GUI_MessageBox::getMsgBox()->show("Voice engine",
                "Unable to connect to the Voice Server !<br/>Check your Internet connection and configure your firewall<br/>(UDP port " + StringHelpers::toString(voipSrvPort) + ").", 
                GUI_MessageBox::MBB_OK, 
                GUI_MessageBox::MBB_ERROR);
        }
    }

    GUI_Debug::refreshDemoVoiceTalkButtonName();
}


//----------------------------------------------------------------------
//  - KH - Extract the user Id from a node Id (uid_avatarindex)
String Navigator::getId(const String& pUid)
{
	size_t ind_ = pUid.find_last_of('_');
	// TBC :  the first letter is S or F or ...
	//		-> causes an identification pb in the world server
	//		-> Remove also last _
	string id = pUid.substr(1, ind_ -  1); 

	return id;
}

//-------------------------------------------------------------------------------------
//  - KH - Check if guestAvatarSnid ( = uid in Social networks of a guest avatar = non local) 
//         is a friend of the local user 
bool Navigator::checkIsFriendOfLocal(const SNTypeEnum snId, const String& guestAvatarSnid)
{
	bool isFriendOfLocal = false;
	
	if (mUserAvatar) {
		// be sure an avatar for localUser is built
		const String& localUserId = mUserAvatar->getCharacterInstance()->getUid();

		// - KH - DEMO fake for demo purpose only  
		//isFriendOfLocal = mFriendUserDB->matchFriend(snId, getId(localUserId), getId(guestAvatarSnid));
		const String& lId = getId(localUserId);
		const String& gId = getId(guestAvatarSnid);
		isFriendOfLocal = ( (lId.compare("100000990433545") == 0) && (gId.compare("100000995083531") == 0)) || 
						  ( (lId.compare("100000995083531") == 0) && (gId.compare("100000990433545") == 0));
	}
	return isFriendOfLocal;
}

//-------------------------------------------------------------------------------------
//  - KH - Switch to Showfriends mode related to SN handling 
void Navigator::toggleShowFriendsMode()
{
	const string& userId = getId(mUserAvatar->getCharacterInstance()->getUid());
	mShowFriendModeEnabled = !mShowFriendModeEnabled;

	// Iterate through the list of guest avatars to display or not the SN friendship icon
	// For each avatar : 
	//	- check if is a friend of local (done in methods of avatars) 
	//	- if yes ->  display SN friend icon  
	//  - if no -> normal display
	for (list<Avatar*>::iterator guestAvatar_ptr = mGuestAvatarsList.begin(); guestAvatar_ptr != mGuestAvatarsList.end(); ++guestAvatar_ptr){
		// Get vId equivalent for snid and check if present in current pair elt
		Avatar* gAvatar_ptr(*guestAvatar_ptr);

		// If Mode is on -> display SN identifiers above head
		if (!mShowFriendModeEnabled) {
			// Reset Social Radar Display Mode == hide SN icons
			gAvatar_ptr->onSocialModeDisabled();
		} else {
			// Display SN identifiers above head (for the moment only twitter)
			gAvatar_ptr->onSocialModeEnabled();
		}
	}
}

//-------------------------------------------------------------------------------------
bool Navigator::stubbedCheckIsFriendOfLocal(const String& pUid)
{
	return checkIsFriendOfLocal(SNFacebook, pUid);

	//const string& userId = getId(pUid);
	//int n = atoi(userId.c_str());
	//return (n % mModuloFriend == 0);
}

//  - KH - Switch to Showfriends mode related to SN handling 
void Navigator::toggleStubbedShowFriendsMode()
{	
	// Perform a memory check...//
	//OutputHeading( "Perform a memory check before modifying social display mode" );
	//_CrtCheckMemory( );

	// Iterate through the list of guest avatars to display or not the SN friendship icon
	//list<avatar *> mGuestAvatarsList;
	// For each avatar : 
	//	- check if a friend
	//	- if yes ->  display SN friend icon  
	//  - if no -> normal display
	for (list<Avatar*>::iterator guestAvatar_ptr = mGuestAvatarsList.begin(); guestAvatar_ptr != mGuestAvatarsList.end(); ++guestAvatar_ptr){
		// Get vId equivalent for snid and check if present in current pair elt
		Avatar* gAvatar_ptr(*guestAvatar_ptr);

		// If ShowFriend mode is disabled re-enable again all non friend avatars that 
		// have been previously disabled
		if (!mShowFriendModeEnabled) {
			// Reset Social Radar Display Mode == hide SN icons
			gAvatar_ptr->onSocialModeDisabled();                 //onStubbedSocialModeDisabled();
			// Enable normal avatar display for those that have been hidden
			if (gAvatar_ptr->getStubbedFriendOfLocal()){
				//onPeerNew(gAvatar_ptr->getXmlEntity()); 
				//onPeerUpdated(gAvatar_ptr->getXmlEntity());
			}
		} else {
			// If ShowFriend mode is enabled disable all avatars not friend of the local
			gAvatar_ptr->onSocialModeEnabled(); //onStubbedSocialModeEnabled();
			// if avatar is not a friend of the local hide it = disconnect from the current scen navigator
			if (!gAvatar_ptr->getStubbedFriendOfLocal()){
				//onPeerLost(gAvatar_ptr->getXmlEntity()); 
				//Perhaps has to update immediately current navigator ???
			}
		}
	}
}

//-------------------------------------------------------------------------------------
//  - KH - Check and Set friendship of the given nodeId with the local user
void Navigator::setFriendOfLocal(const void* avatarPtr, const String& uid, const String& fcbkUid, const String& twtrUid)
{
	// Check if this user identified by uid and (SN)LoginName is a friend of the local user
	if (getUseFacebookSN())
		((Avatar*)avatarPtr)->setFacebookFriendOfLocal(checkIsFriendOfLocal(SNFacebook, fcbkUid)); 
	if (getUseTwitterSN())
		((Avatar*)avatarPtr)->setTwitterFriendOfLocal(checkIsFriendOfLocal(SNTwitter, twtrUid));

	// KH - Demo - Fake for demo purpose only
	bool isFriendOfLocal = checkIsFriendOfLocal(SNFacebook, fcbkUid);
	((Avatar*)avatarPtr)->setFacebookFriendOfLocal(checkIsFriendOfLocal(SNFacebook, fcbkUid));
}

//-------------------------------------------------------------------------------------
//  - KH - Store the list of friends for this user relative to Facebook 
void Navigator::storeFCBKFriendList(const string& nId, const list<String_Pair>& fcbkfl)
{
	mFriendUserDB->addUserFriendList(nId, SNFacebook, fcbkfl);
}
//-------------------------------------------------------------------------------------
// Store the list of friends for this user relative to Twitter 
void Navigator::storeTWTRFriendList(const string& nId, const list<String_Pair>& twtrfl)
{
	mFriendUserDB->addUserFriendList(nId, SNTwitter, twtrfl);
}
//-------------------------------------------------------------------------------------
// Store the list of friends for this user relative to Opensocial base SN 
void Navigator::storePNSCLFriendList(const string& nId, const list<String_Pair>& opnsclfl)
{
	mFriendUserDB->addUserFriendList(nId, SNOpensocial, opnsclfl);
}

void Navigator::OutputHeading( const char * explanation )
{
//   _RPT1( _CRT_WARN, "\n\n%s:\n**************************************\
//************************************\n", explanation );
}

