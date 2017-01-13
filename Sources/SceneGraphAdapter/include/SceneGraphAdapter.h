/**
* \file SceneGraphAdapter.h
* \brief The main class of the SceneGraphAdapter API
*
* Details about SceneGraphAdapter.h
*/

#ifndef _SCENEGRAPHADAPTER_H
#define _SCENEGRAPHADAPTER_H

#ifdef SCENEGRAPHADAPTER_EXPORTS
#define SCENEGRAPHADAPTER_API __declspec(dllexport)
#else
#define SCENEGRAPHADAPTER_API __declspec(dllimport)
#endif

#include <string>

//namespace SceneGraphAdapter {
//
//class FormatSceneGraphAdapter{
//
//	public: 
//
//		std::string createGroupNode( const std::string &sNodeId, const std::string &sFileName, const std::string &sEngineId );
//
//	};
//}


#endif // _SCENEGRAPHADAPTER_H