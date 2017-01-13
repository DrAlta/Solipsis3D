#ifndef __x3dWrapperPrerequisites_h__
#define __x3dWrapperPrerequisites_h__


#	ifdef X3D_WRAPPER_EXPORTS
#		define X3D_WRAPPER_API __declspec(dllexport)
#	else
#    	define X3D_WRAPPER_API __declspec(dllimport)
#   endif


    class X3dWrapper;

#define INPUT_FILE_EXTENSION "x3d"
#define WRAPPER_TYPE "FormatWrapper"
#define WRAPPER_VERSION 0.1
    

#endif // __x3dWrapperPrerequisites_h__