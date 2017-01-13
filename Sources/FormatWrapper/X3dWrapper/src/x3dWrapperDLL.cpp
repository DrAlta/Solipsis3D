#include "x3dWrapper.h"
#include "x3dWrapperPrerequisites.h"


X3dWrapper* _x3dWrapper;

//-------------------------------------------------------------------------------------
//extern "C" _x3d_Wrapper_Export void dllStartPlugin(void)
//{
//    // Create new plugin
//    _x3dWrapper = new X3dWrapper();
//
//    // Register
//	Root::getSingleton().installPlugin(_x3dWrapper);
//}

//-------------------------------------------------------------------------------------
//extern "C" _Plugin_x3dExport void dllStopPlugin(void)
//{
//    // Unregister
//    Root::getSingleton().uninstallPlugin(_Plugin_x3d);
//
//    // Destroy plugin
//    delete _Plugin_x3d;
//}