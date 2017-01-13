#ifndef _OgrePluginTester_H_
#define _OgrePluginTester_H_

#include "Ogre.h"
#include "OgreConfigFile.h"
#include "ExampleFrameListener.h"

#include "SGAKernel.h"

#include "OgreWrapper.h"
//#include "BulletWrapper.h"
#include <windows.h>
#include <stdio.h>
#include <sys/types.h>
#include <locale>
#include <sstream>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUILogger.h>

//#include <RendererModules/Ogre/CEGUIOgreRenderer.h>
#include <OgreCEGUIRenderer.h>



#define FORMAT_WRAPPER_FOLDER "..\\..\\FormatWrapper\\lib\\Debug\\*.*"
#define RENDERER_WRAPPER_FOLDER "..\\..\\RendererWrapper\\lib\Debug\\*.*"
#define PHYSICS_WRAPPER_FOLDER "..\\..\\PhysicsWrapper\\lib\Debug\\*.*"

using namespace Ogre;
using namespace SceneGraphAdapter;

class OPTFrameListener : public ExampleFrameListener, public OIS::MouseListener//, public OIS::KeyListener
{
private:
	CEGUI::Renderer* mGUIRenderer;
	SGAKernel * mKernel;
	Ogre::RaySceneQuery* mRayScnQuery;
	SceneManager* mSceneMgr;
	Ogre::SceneNode *mCurrentObject;	//pointer to our currently selected object
	Ogre::MovableObject *curClikedMovableObject;
	Ogre::MovableObject *curTouchedMovableObject;
	Real elapsedTime;


public:
	// NB using buffered input, this is the only change
	OPTFrameListener(RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer, SGAKernel * kernel, SceneManager* sceneMgr)
		: ExampleFrameListener(win, cam, false, true, false), 
		mGUIRenderer(renderer),
		mKernel(kernel),
		mSceneMgr(sceneMgr), 
		mCurrentObject(0), curClikedMovableObject(0), curTouchedMovableObject(0)
	{
		mMouse->setEventCallback(this);
		mRayScnQuery = mSceneMgr->createRayQuery(Ogre::Ray());

		//mKeyboard->setEventCallback(this);
	}

	~OPTFrameListener(){
		mSceneMgr->destroyQuery(mRayScnQuery);
		delete mGUIRenderer;
		delete mCurrentObject;
	}

	bool frameRenderingQueued(const FrameEvent& evt){

		//Keep controls of ExampleFrameListener
		if( ExampleFrameListener::frameRenderingQueued(evt) == false )
			return false;

		static Real timeDelay = 0;
		timeDelay += evt.timeSinceLastFrame;
		mKernel->updateSceneTime((float)timeDelay);

		Ogre::Vector3 camPos = mCamera->getPosition();
		Ogre::Ray cameraRay(Ogre::Vector3(camPos.x, 5000.0f, camPos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);
		mRayScnQuery->setRay(cameraRay);

		return true;
	}

	bool frameEnded(const FrameEvent& evt){

		return ExampleFrameListener::frameEnded(evt);
	}
	//----------------------------------------------------------------//
	bool mouseMoved( const OIS::MouseEvent &arg ){

		CEGUI::System::getSingleton().injectMouseMove( arg.state.X.rel, arg.state.Y.rel );
		CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();

		/*if(mCurrentObject){
			mCurrentObject->showBoundingBox(false);
			mKernel->onExit(mCurrentObject->getName(), "ogre", (float)elapsedTime, (float) arg.state.X.abs, (float) arg.state.Y.abs );
			mCurrentObject = NULL;
		}*/

		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width), mousePos.d_y/float(arg.state.height));
		mRayScnQuery->setRay(mouseRay);
		mRayScnQuery->setSortByDistance(true);

		Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
		Ogre::RaySceneQueryResult::iterator iter = result.begin();

		if( result.empty() && curTouchedMovableObject!=NULL){
			if(mCurrentObject) mCurrentObject->showBoundingBox(false);
			mKernel->onExit(curTouchedMovableObject->getName(), "ogre", (float)elapsedTime, mousePos.d_x, mousePos.d_y );
			curTouchedMovableObject = NULL;
		}
		else if (iter != result.end() && iter->movable){

			for(iter; iter != result.end(); iter++){

				MovableObject * obj = iter->movable;
				if( curTouchedMovableObject!=NULL) {
					if(curTouchedMovableObject->getName().compare( obj->getName() ) !=0  ){
						if(mCurrentObject) mCurrentObject->showBoundingBox(false);
						mKernel->onExit(curTouchedMovableObject->getName(), "ogre", (float)elapsedTime, mousePos.d_x, mousePos.d_y );
					}
				}
				curTouchedMovableObject = obj;
				//curTouchedMovableObject = obj;
				//obj->setDebugDisplayEnabled(true);
				string oName = obj->getName();
				/*if( mSceneMgr->hasEntity(oName)){
					Entity * ent = mSceneMgr->getEntity(oName);
				}*/
				if(mCurrentObject){
					mCurrentObject = obj->getParentSceneNode();
					mCurrentObject->showBoundingBox(true);
				
					//Ogre::SceneNode * pNode = mCurrentObject->getParentSceneNode();
					mKernel->onEnter(oName, "ogre", (float)elapsedTime, mousePos.d_x, mousePos.d_y, mCurrentObject->getName() );
				}
				break;
			}
		}
		return true;
	}

	////----------------------------------------------------------------//
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		if(id == OIS::MB_Left){

			//find the current mouse position
			if(mCurrentObject)
			{
				//mCurrentObject->showBoundingBox(false);
			}
			CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();

			Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x/float(arg.state.width), mousePos.d_y/float(arg.state.height));
			mRayScnQuery->setRay(mouseRay);
			mRayScnQuery->setSortByDistance(true);

			Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
			Ogre::RaySceneQueryResult::iterator iter = result.begin();

			if (iter != result.end() && iter->movable){

				for(iter; iter != result.end(); iter++){

					MovableObject * obj = iter->movable;

					curClikedMovableObject = obj;
					string oName = obj->getName();
					mCurrentObject = obj->getParentSceneNode();
					//Ogre::SceneNode * pNode = obj->getParentSceneNode();
					//Entity * ent = mSceneMgr->getEntity(oName);
					//mCurrentObject = obj->getParentSceneNode();
					//Ogre::SceneNode * pNode = mCurrentObject->getParentSceneNode();
					mKernel->onPress(oName, "ogre", (float)elapsedTime, mousePos.d_x, mousePos.d_y, mCurrentObject->getName() );
					break;
				}
			}

		}
		return true;
	}

	////----------------------------------------------------------------//
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){

		CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
		
		if(mCurrentObject)
		{
			mCurrentObject->showBoundingBox(false);
			mCurrentObject = NULL;
		}
		if( curClikedMovableObject ){
			mKernel->onRelease(curClikedMovableObject->getName(),"ogre",(float)elapsedTime, mousePos.d_x, mousePos.d_y );
			curClikedMovableObject = NULL;
		}
		return true;
	}

};

class OgrePluginTester {

public:
	SGAKernel * kernel;

	/// Standard constructor
	OgrePluginTester()
	{
		mFrameListener = 0;
		mRoot = 0;
		mGUIRenderer = 0;
		mGUISystem = 0;
		mCamera = 0;
		mWindow = 0;

		kernel = NULL;

		mGUIRenderer = NULL;
		mGUISystem = NULL;

		// Provide a nice cross platform solution for locating the configuration files
		// On windows files are searched for in the current working directory, on OS X however
		// you must provide the full path, the helper function macBundlePath does this for us.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		mResourcePath = macBundlePath() + "/Contents/Resources/";
#else
		mResourcePath = "";
#endif
	}
	/// Standard destructor
	~OgrePluginTester()
	{
		if (mFrameListener)
			delete mFrameListener;
		if (mRoot)
			OGRE_DELETE mRoot;
		if(kernel) delete kernel;
		if(mFrameListener) delete mFrameListener;
	}

	void go(int argc, char **argv){

		std::vector<string> args(argv,argv+argc);

		if (!setup(args))
			return;

		try{
			loadScene(args);
			mRoot->startRendering();
			
		}
		catch( Ogre::Exception e ){
			MessageBoxA( NULL, e.what(), "An ogre exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		}

		// clean up
		destroyScene();
	}

protected:
	Root *mRoot;
	Camera* mCamera;
	SceneManager* mSceneMgr;
	ExampleFrameListener* mFrameListener;
	RenderWindow* mWindow;
	Ogre::String mResourcePath;
	CEGUI::Renderer* mGUIRenderer;		//our CEGUI renderer
	CEGUI::System* mGUISystem;


	// These internal methods package up the stages in the startup process
	/** Sets up the application - returns false if the user chooses to abandon configuration. */
	bool setup(std::vector<string> args)
	{

		string pluginsPath;
		// only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
		pluginsPath = mResourcePath + "plugins.cfg";
#endif

		mRoot = OGRE_NEW Root(pluginsPath, 
			mResourcePath + "ogre.cfg", mResourcePath + "Ogre.log");

		kernel = new SGAKernel();
		setupResources();

		bool carryOn = configure();
		if (!carryOn) return false;

		chooseSceneManager();
		createCamera();
		createViewports();

		// Set default mipmap level (NB some APIs ignore this)
		TextureManager::getSingleton().setDefaultNumMipmaps(5);

		// Create any resource listeners (for loading screens)
		createResourceListener();
		// Load resources
		loadResources();
		loadFormatWrappers();

		// Create the scene
		createScene(args);

		createFrameListener();

		return true;

	}


	/** Configures the application - returns false if the user chooses to abandon configuration. */
	bool configure(void)
	{
		// Show the configuration dialog and initialise the system
		// You can skip this and use root.restoreConfig() to load configuration
		// settings if you were sure there are valid ones saved in ogre.cfg
		if(mRoot->showConfigDialog())
		{
			// If returned true, user clicked OK so initialise
			// Here we choose to let the system create a default rendering window by passing 'true'
			mWindow = mRoot->initialise(true);
			return true;
		}
		else
		{
			return false;
		}
	}

	void chooseSceneManager(void)
	{
		// Create the SceneManager, in this case a generic one
		mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance");
	}
	void createCamera(void)
	{
		// Create the camera
		mCamera = mSceneMgr->createCamera("PlayerCam");

		// Position it at 500 in Z direction
		mCamera->setPosition(Vector3(0,70,150));
		// Look back along -Z
		mCamera->lookAt(Vector3(0,-7,-300));
		mCamera->setNearClipDistance(5);

	}
	void createFrameListener(void)
	{
		mFrameListener= new OPTFrameListener(mWindow, mCamera, mGUIRenderer, kernel, mSceneMgr);
		//mFrameListener->showDebugOverlay(true);
		mRoot->addFrameListener(mFrameListener);

	}

	std::string ToNarrow( const wchar_t *s, char dfault = '?', 
		const std::locale& loc = std::locale() )
	{
		std::ostringstream stm;

		while( *s != L'' ) {
			stm << std::use_facet< std::ctype<wchar_t> >( loc ).narrow( *s++, dfault );
		}
		return stm.str();
	}

	void loadFormatWrappers(void){

		WIN32_FIND_DATA data;
		HANDLE hfind;

		string sFilename= FORMAT_WRAPPER_FOLDER;

		std::wstring swFilename(sFilename.length(), L' ');
		std::copy(sFilename.begin(), sFilename.end(), swFilename.begin());

		hfind = FindFirstFile(swFilename.c_str(), &data );

		if ( hfind != INVALID_HANDLE_VALUE ){
			do{
				if( data.cFileName[0] != '.' ){
					if ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) continue;
					else{
						WCHAR * curFileName = data.cFileName;
						string sCurFileName = ToNarrow(curFileName,'?');
						size_t lastPoint = sCurFileName.find_last_of(".") + 1;
						string sFileExtension = sCurFileName.substr(lastPoint);
						if(sFileExtension=="dll"){
							kernel->registerPlugin(sCurFileName);
						}
					}
				}
			}
			while(FindNextFile( hfind, &data));
		}
	}

	void loadScene(std::vector<string> args){

		OgreWrapper * oWrapper;
		EngineWrapperFactory::PhysicsWrapper * bWrapper = NULL;
		size_t idxRenderer = -1;
		size_t idxPhysics = -1;

		int i;
		for( i = 1; i < args.size(); i++ ){

			std::string sFileName = args.at(i);
			///> option that indicates the next argument is the dll of the RendererWrapper
			if(sFileName=="-r"){
				i++;
				idxRenderer = kernel->loadPlugin(args.at(i));
				oWrapper =  static_cast<OgreWrapper *> (kernel->getEngineWrapper().getWrapper(idxRenderer));
				oWrapper->setOgreApplication(mRoot , mSceneMgr);
			}
			///> option that indicates the next argument is the dll of the PhysicsWrapper
			else if(sFileName=="-p"){
				i++;
				idxPhysics = kernel->loadPlugin(args.at(i));
				bWrapper = static_cast<EngineWrapperFactory::PhysicsWrapper *>(kernel->getEngineWrapper().getWrapper(idxPhysics)); 
				bWrapper->configureEngine(kernel);
			}
			/// > input files
			else{
				string sRendererName = kernel->getEngineWrapper().getWrapper(idxRenderer)->getWrapperConfiguration()->getIdentifier();
				kernel->loadFile( sFileName, args.at(0), sRendererName );
			}
			
		}
	}

	void createScene(std::vector<string> args){

		mSceneMgr->setSkyBox(true, "MySkyBox");

		mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr);
		mGUISystem = new CEGUI::System(mGUIRenderer);
		CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);
		CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"TaharezLookSkin.scheme");
		mGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
		CEGUI::MouseCursor::getSingleton().setImage(CEGUI::System::getSingleton().getDefaultMouseCursor());

		//loadScene(args);

		//OgreWrapper * oWrapper;
		//EngineWrapperFactory::PhysicsWrapper * bWrapper = NULL;
		////BulletWrapper * bWrapper;
		//size_t idxRenderer = -1;
		//size_t idxPhysics = -1;
		////size_t idxFormat;
		////FormatWrapperFactory *  m_wrapper;

		//int i;
		//for( i = 1; i < args.size(); i++ ){

		//	std::string sFileName = args.at(i);
		//	///> option that indicates the next argument is the dll of the RendererWrapper
		//	if(sFileName=="-r"){
		//		i++;
		//		idxRenderer = kernel->loadPlugin(args.at(i));
		//		oWrapper =  static_cast<OgreWrapper *> (kernel->getEngineWrapper().getWrapper(idxRenderer));
		//		oWrapper->setOgreApplication(mRoot , mSceneMgr);
		//	}
		//	///> option that indicates the next argument is the dll of the PhysicsWrapper
		//	else if(sFileName=="-p"){
		//		i++;
		//		idxPhysics = kernel->loadPlugin(args.at(i));
		//		bWrapper = static_cast<EngineWrapperFactory::PhysicsWrapper *>(kernel->getEngineWrapper().getWrapper(idxPhysics)); 
		//		bWrapper->configureEngine(kernel);
		//	}
		//	/// > input files
		//	else{
		//		string sRendererName = kernel->getEngineWrapper().getWrapper(idxRenderer)->getWrapperConfiguration()->getIdentifier();
		//		kernel->loadFile( sFileName, args.at(0), sRendererName );
		//	}
		//	
		//}

		setupLighting();

	}

	/*void setupLighting()
	{
		mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

		Light* l = mSceneMgr->createLight("MainLight");
		 l->setPosition(20,80,50);

	}*/

	void setupLighting(){

		//Ogre::SceneNode* lLightSceneNode = NULL;

		//Ogre::Light* lLight = mSceneMgr->createLight("MainLight");
		//lLight->setType( Light::LT_SPOTLIGHT );
		//lLight->setDiffuseColour(1.0f, 1.0f, 1.0f);
		////lLight->setSpecularColour(1.0f, 1.0f, 1.0f);// color of 'reflected' light
		//lLight->setDirection(0,50,50);
		//lLight->setPosition( Vector3( 300, 300, 0 ) );
		//lLight->setAttenuation(8000,1,0.0005,0);
		//lLightSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		//lLightSceneNode->attachObject(lLight);

		mSceneMgr->setAmbientLight(Ogre::ColourValue(0,0,0));
		//	mSceneMgr->setAmbientLight(ColourValue::Black);
		//	// Point light, movable, reddish
		Ogre::Light* mLight;
		mLight = mSceneMgr->createLight("Light2");
		mLight->setDiffuseColour(1.0f, 1.0f, 1.0f);
		mLight->setSpecularColour(1, 1, 1);
		mLight->setAttenuation(8000,1,0.0005,0);

		Light* ml = mSceneMgr->createLight("MainLight");
		ml->setPosition(20,80,50);

		Ogre::Light * sl = mSceneMgr->createLight("OtherLight");
		sl->setType(Light::LightTypes::LT_SPOTLIGHT);
		sl->setDiffuseColour( 1.0,1.0,1.0 );
		sl->setSpecularColour( 1.0, 1.0, 1.0 );
		sl->setDirection(0,0,50);
		sl->setPosition( Vector3( 300, 300, 0 ) );
		sl->setSpotlightRange( Degree(35), Degree(180) );


		Ogre::Light * light = mSceneMgr->createLight( "Light3" );
		light->setType( Light::LT_DIRECTIONAL );
		light->setDiffuseColour( ColourValue( 1, 1, 1 ) );
		light->setSpecularColour( ColourValue( 0.5, 0, 0 ) );
		light->setDirection(0,50,50);
		//light->setAttenuation(1,0.05,1,3);
		light->setAttenuation(8000,1,0.0005,0);
		light->setPosition(Vector3( 300, 300, 0 ));

		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);

	}

	void destroyScene(void){};    // Optional to override this

	void createViewports(void)
	{
		// Create one viewport, entire window
		Viewport* vp = mWindow->addViewport(mCamera);
		vp->setBackgroundColour(ColourValue(0,0,0));

		// Alter the camera aspect ratio to match the viewport
		mCamera->setAspectRatio(
			Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
	}

	/// Method which will define the source of resources (other than current folder)
	void setupResources(void)
	{
		// Load resource paths from config file
		ConfigFile cf;
		cf.load(mResourcePath + "resources.cfg");

		// Go through all sections & settings in the file
		ConfigFile::SectionIterator seci = cf.getSectionIterator();

		string secName, typeName, archName;
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			ConfigFile::SettingsMultiMap *settings = seci.getNext();
			ConfigFile::SettingsMultiMap::iterator i;
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				typeName = i->first;
				archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
				// OS X does not set the working directory relative to the app,
				// In order to make things portable on OS X we need to provide
				// the loading with it's own bundle path location
				ResourceGroupManager::getSingleton().addResourceLocation(
					String(macBundlePath() + "/" + archName), typeName, secName);
#else
				ResourceGroupManager::getSingleton().addResourceLocation(
					archName, typeName, secName);
#endif
			}
		}
	}

	/// Optional override method where you can create resource listeners (e.g. for loading screens)
	void createResourceListener(void)
	{

	}

	/// Optional override method where you can perform resource group loading
	/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	void loadResources(void)
	{
		// Initialise, parse scripts etc
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	}

};
#endif	// end _OgrePluginTester_H_