#ifndef __bulletWrapperPrerequisites_h__
#define __bulletWrapperPrerequisites_h__


#ifdef BULLET_WRAPPER_EXPORTS
    #define BULLET_WRAPPER  __declspec( dllexport )
#else
    #define BULLET_WRAPPER  __declspec( dllimport )
#endif

	class BulletWrapper;

#define ENGINE_NAME "bullet"
#define WRAPPER_TYPE "PhysicsWrapper"
#define WRAPPER_VERSION 0.1

#endif // __bulletWrapperPrerequisites_h__