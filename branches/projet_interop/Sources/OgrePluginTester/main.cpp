
#include "OgrePluginTester.h"

int main(int argc, char **argv){

	if (argc < 1){
		//fprintf(stderr, "Usage: %s filename\n", argv[0]);
		fprintf(stderr, "Usage: %s -[option] engineDll filename\n", argv[0]);
		fprintf(stderr, "Option : \n");
		fprintf(stderr, "	-r : rendering engine \n");
		fprintf(stderr, "	-p : physics engine \n");
		exit(-1);
	}


	OgrePluginTester OPTester;


	try {
		OPTester.go(argc,argv);
		/*if(OPTester.kernel){
			int nbEngine = OPTester.kernel->getEngineWrapper().getWrapperCount(); 
			EngineWrapperFactory::EngineWrapper * eWrapper;
			for( int i = 0; i<nbEngine; i++){
				eWrapper = OPTester.kernel->getEngineWrapper().getWrapper(i);
				eWrapper->run();
			}	
		}*/
	} 
	catch( Ogre::Exception e ) { 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
		MessageBoxA( NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		fprintf(stderr, "An exception has occurred: %s\n",
			e.what());
#endif
	}
	
	exit(0);
}