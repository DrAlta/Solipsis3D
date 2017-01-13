#include "OgrePluginTester.h"

#define KEY_PRESSED(_key,_timeDelay, _macro) \
{ \
    if (mKeyboard->isKeyDown(_key) && timeDelay <= 0) \
    { \
		timeDelay = _timeDelay; \
        _macro ; \
    } \
}

using namespace Ogre;

	OgrePluginTester::OgrePluginTester(){
		pApplication = new ExampleApplication();
		pRoot =  pApplication->mRoot;;
		pCamera = pApplication->mCamera;
		pSceneManager = pApplication->mSceneMgr;
		pFrameListener = pApplication->mFrameListener;
		pRenderWindow = pApplication->mWindow;
		pResourcePath = pApplication->mResourcePath;
	}

	OgrePluginTester::~OgrePluginTester(){
		delete pApplication;
	}

	bool OgrePluginTester::start()
	{
		pApplication->setup
		/*pRoot = OGRE_NEW Root( "plugins.cfg", "ogre.cfg", "Ogre.log");

		if(pRoot->showConfigDialog())
		{
			pRenderWindow = pRoot->initialise(TRUE,"Plugin Tester");
			pSceneManager = pRoot->createSceneManager(Ogre::ST_GENERIC, "MonGestionnaireDeScene");

			pCamera = pSceneManager->createCamera("MaCamera");
			pCamera->setNearClipDistance(5);
			pCamera->setFarClipDistance(1000);
			
			pCamera->setPosition(Ogre::Vector3(0,0,400));
			pCamera->lookAt(Ogre::Vector3(0,0,-300));

			pViewport = pRenderWindow->addViewport(pCamera);
			pViewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 0.5));

			pCamera->setAspectRatio(Real(pViewport->getActualWidth())/Real(pViewport->getActualHeight()));

			loadResources();

			createFrameListener();

			return true;
		}
		else return false;*/
	}

	void OgrePluginTester::run()
	{
		if(pRoot!=0) pRoot->startRendering();
	}

	
	
	void OgrePluginTester::exit(){
		if (pFrameListener) delete pFrameListener;
		if(pRoot!=0) OGRE_DELETE pRoot;
	}

	void OgrePluginTester::loadResources()
		{
			//Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media","FileSystem","General");
			ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		}

	void OgrePluginTester::createFrameListener(void)
    {
        pFrameListener= new ExampleFrameListener(pRenderWindow, pCamera);
		pFrameListener->showDebugOverlay(true);
        pRoot->addFrameListener(pFrameListener);
    }
