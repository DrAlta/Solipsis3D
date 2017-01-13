#ifndef __colladaWrapperPrerequisites_h__
#define __colladaWrapperPrerequisites_h__


#	ifdef COLLADA_WRAPPER_EXPORTS
#		define COLLADA_WRAPPER_API __declspec(dllexport)
#	else
#    	define COLLADA_WRAPPER_API __declspec(dllimport)
#   endif

    class ColladaWrapper;
   
#define INPUT_FILE_EXTENSION "dae"
#define WRAPPER_TYPE "FormatWrapper"
#define WRAPPER_VERSION 0.1

#endif // __colladaWrapperPrerequisites_h__