#ifndef __ogreWrapperPrerequisites_h__
#define __ogreWrapperPrerequisites_h__


#ifdef OGRE_WRAPPER_EXPORTS
    #define OGRE_WRAPPER  __declspec( dllexport )
#else
    #define OGRE_WRAPPER  __declspec( dllimport )
#endif

	class OgreWrapper;

#define RENDERER_NAME "ogre"
#define WRAPPER_TYPE "RendererWrapper"
#define WRAPPER_VERSION 0.1

#endif // __ogreWrapperPrerequisites_h__