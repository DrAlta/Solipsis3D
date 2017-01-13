#include <ogre.h>
#include "ExampleFrameListener.h"
#include "OgreConfigFile.h"
using namespace Ogre; 

#ifndef _OgrePluginTester_H_
#define _OgrePluginTester_H_

class OgrePluginTester
	{
	public:
		Root* pRoot;
		Ogre::SceneManager* pSceneManager;
		RenderWindow* pRenderWindow;
		Viewport* pViewport;
		Camera* pCamera;
		Entity* pEntity;
		SceneNode* pNode;

		ExampleFrameListener* pFrameListener;

		// OIS Input devices
		/*OIS::InputManager* mInputManager;
		OIS::Mouse*    mMouse;
		OIS::Keyboard* mKeyboard;*/


	public:
		OgrePluginTester(void){
			pRoot = 0;
			pSceneManager = 0;
			pRenderWindow = 0;
			pViewport = 0;
			pCamera = 0;
			pFrameListener = 0;
		};
		~OgrePluginTester(void);
		bool start();
		void run();
		void exit();
		//void createObject(String name);
		void loadResources();
		void setupResources(void);
		void chooseSceneManager(void);
		void createCamera(void);
		void createViewports(void);
		void createFrameListener(void);

	
	};
#endif	// end _OgrePluginTester_H_