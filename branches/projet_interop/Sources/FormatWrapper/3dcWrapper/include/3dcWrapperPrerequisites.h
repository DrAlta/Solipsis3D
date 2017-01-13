#ifndef __3dcWrapperPrerequisites_h__
#define __3dcWrapperPrerequisites_h__


#	ifdef _3DC_WRAPPER_EXPORTS
#		define _3DC_WRAPPER_API __declspec(dllexport)
#	else
#    	define _3DC_WRAPPER_API __declspec(dllimport)
#   endif


    class _3dcWrapper;

#define INPUT_FILE_EXTENSION "3dc"
#define WRAPPER_TYPE "FormatWrapper"
#define WRAPPER_VERSION 0.1
    

#endif // __3dcWrapperPrerequisites_h__