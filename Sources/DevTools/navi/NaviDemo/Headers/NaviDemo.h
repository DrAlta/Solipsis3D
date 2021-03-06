#ifndef __NaviDemo_H__
#define __NaviDemo_H__
#if _MSC_VER > 1000
#pragma once
#endif

#ifdef _DEBUG
#define DEBUG_OVERLAY
#endif

#include "Ogre.h"
#include "InputManager.h"
#include "NaviManager.h"

class NaviDemo : public OIS::MouseListener, public OIS::KeyListener, public Ogre::WindowEventListener
{
	Ogre::RenderWindow* renderWin;
	Ogre::SceneManager* sceneMgr;
#ifdef DEBUG_OVERLAY
	Ogre::Overlay* dbgOverlay;
	Ogre::OverlayElement* dbgText;
#endif
	InputManager* inputMgr;
	Ogre::SceneNode* camNode;
	bool forward, left, right, back, up, down, turnL, turnR;
	unsigned long lastTime;
	Ogre::Timer timer;
	void parseResources();
	void loadInputSystem();
	void updateStats();
public:
	bool shouldQuit;
	NaviDemo();
	~NaviDemo();
	void Startup();
	void Update();
	void Shutdown();

	void turnOn(const NaviLibrary::NaviData &naviData);
	void turnOff(const NaviLibrary::NaviData &naviData);
	void hpChange(const NaviLibrary::NaviData &naviData);
	void messageSent(const NaviLibrary::NaviData &naviData);
	void itemEquipped(const NaviLibrary::NaviData &naviData);
	void levelChanged(const NaviLibrary::NaviData &naviData);
	
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	bool keyPressed( const OIS::KeyEvent &arg );
	bool keyReleased( const OIS::KeyEvent &arg );	

	void windowMoved(Ogre::RenderWindow* rw);
	void windowResized(Ogre::RenderWindow* rw);
	void windowClosed(Ogre::RenderWindow* rw);
	void windowFocusChange(Ogre::RenderWindow* rw);
};

#endif