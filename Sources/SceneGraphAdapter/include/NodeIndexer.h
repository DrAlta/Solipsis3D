#ifndef __NodeIndexer_h__
#define __NodeIndexer_h__

/**
* \file NodeIndexer.h
* \brief The NodeIndexer: provides a map for all nodes process by the SceneGraphAdapter and keeps information about them
*/

#include "SceneGraphAdapter.h"
#include <map>
#include <string>

using namespace std;

/**
* @class NodeIndexer
* @author Rozenn Bouville Berthelot
* @date 14/01/2011
* @version 0.0
*/
namespace SceneGraphAdapter {

	struct NodeCompClass {
		bool operator() (std::pair< string , string > node1 , std::pair< string , string > node2 ) const {
			bool res1 = node1.first.compare(node2.first);
			bool res2 = node1.first.compare(node2.first);
			bool res3 = ( node1.first.compare(node2.first)<0 ) && ( node1.first.compare(node2.first)<0 );
			return res3;
		}
	};

	class NodeIndexer {

		class SGAKernel;

	private:

		static bool instanceFlag;
		static NodeIndexer * nodeIndexInstance; ///< the instance of the NodeIndexer
		/**
		* @fn NodeIndexer() 
		* @brief Default constructor
		*/
		NodeIndexer(){
			m_EngineToFormatMap = std::map< NodeIdentifier,NodeIdentifier,NodeCompClass >();
			m_FormatToEngineMap = std::multimap< NodeIdentifier,NodeIdentifier,NodeCompClass >();
		}

		/**
		* @fn void clean()
		* @brief clean NodeIndexer object
		*/
		void clean(){
			m_EngineToFormatMap.clear();
			m_FormatToEngineMap.clear();
		}


	public:
		/**
		* @fn ~NodeIndexer() 
		* Destructor
		*/
		~NodeIndexer(){
			instanceFlag = false;
			clean();
		}
		/**
		* @fn SCENEGRAPHADAPTER_API static NodeIndexer * getInstance()
		* @brief get NodeIndexer current instance
		* @return the current NodeIndexer instance
		*/
		SCENEGRAPHADAPTER_API static NodeIndexer * getInstance();

		/**
		* @fn void addNode( const string &sFormatId, const string &sFileName, const string &sEngineId,  const string &sEngine)
		* @brief add a node and its information in the NodeIndexer
		* @param sFormatId the unique identifier for the node in the format scene graph ( ex: "SPHERE" )
		* @param sFileName the input file name of the node  ( ex: "scene.x3d" )
		* @param sEngineId the unique identifier for the node in the engine scene graph  ( ex: "scene_SPHERE_6" )
		* @param sEngine the engine name  ( ex: "ogre" )
		*/
		SCENEGRAPHADAPTER_API void addNode( const string &sFormatId, const string &sFileName, const string &sEngineId,  const string &sEngine);

		/**
		* @fn void deleteNode ( const string &sId , const string &sSceneGraphId, bool isFormatNode = true )
		* @brief delete a node in the NodeIndexer
		* @param sId the unique identifier for the node
		* @param sSceneGraphId the unique identifier for the node
		* @param isFormatNode if isFormatNode=true the node to be deleted is an format scene graph node else it is an engine scene graph node
		* @return 1 if succeed, 0 instead
		*/
		SCENEGRAPHADAPTER_API int deleteNode ( const string &sId , const string &sSceneGraphId, bool isFormatNode = true );	

		/**
		* @fn void setNode ( const string &sNodeId, const string &sSceneGraphId )
		* @brief set node information in NodeIndexer
		* @param sNodeId the unique identifier of the node to set
		* @param sSceneGraphId the identifier of the scene graph of the node (an engine scene graph or a format scene graph)
		*/
		SCENEGRAPHADAPTER_API void setNode ( const string &sNodeId, const string &sSceneGraphId );

		/**
		* @fn string getEngineNodeInputFile( const string &sEngineNodeId , const string &sEngineId )
		* @brief get the matching node input file name given an engine scene graph node
		* @param sEngineNodeId the unique identifier of the node in the engine scene graph ( ex: "SGA_SPHERE_ogre" )
		* @param sEngineId the identifier of the engine ( ex: "ogre" )
		* @return the input file name ( ex: "..\samples\myscene.x3d" ) 
		*/
		SCENEGRAPHADAPTER_API string getEngineNodeInputFile( const string &sEngineNodeId , const string &sEngineId );


		//SCENEGRAPHADAPTER_API string getNodeInputFormat( const string sNodeId ) const;

		/**
		* @fn string getEngineNodeInputIdentifier( const string &sEngineNodeId , const string &sEngineId )
		* @brief get the matching node identifier in a format scene graph given an engine scene graph node
		* @param sEngineNodeId the unique identifier of the node in the engine scene graph ( ex: "SGA_SPHERE_ogre" )
		* @param sEngineId the identifier of the engine scene graph of the node ( ex: "ogre" )
		* @return the identifier ( ex: "SPHERE" ) 
		*/ 
		SCENEGRAPHADAPTER_API string getEngineNodeInputIdentifier( const string &sEngineNodeId , const string &sEngineId );

		/**
		* @fn string getFormatNodeOutputIdentifier( const string &sFormatNodeId , const string &sInputFile, const string &sEngineId )
		* @brief get the matching node identifier in an engine scene graph given a format scene graph node and an engine identifier
		* @param sFormatNodeId the unique identifier of the node in the format scene graph ( ex: "SPHERE" )
		* @param sInputFile  the input file name ( ex: "..\samples\myscene.x3d" )
		* @param sEngineId the identifier of the engine ( ex: "ogre" )
		* @return the identifier ( ex: "SGA_SPHERE_ogre" ) 
		*/ 
		SCENEGRAPHADAPTER_API string getFormatNodeOutputIdentifier( const string &sFormatNodeId , const string &sInputFile, const string &sEngineId );



	private:

		/** \typedef typedef std::pair< string , string > NodeIdentifier
		*	a node identifier is a pair of string that identify a node in SGA
		*	NodeIndexMap.first =  the node unique identifier ( ex: "box_T0" )
		*	NodeIndexMap.second = the identifier of the scene graph of the node ( ex: "box.x3d" or "ogre" )
		*/
		typedef std::pair< std::string , std::string > NodeIdentifier;

		/** \typedef typedef std::map< nodeIdentifier , nodeIdentifier, NodeCompClass > EngineToFormatMap
		*	a map that gives for each engine scene graph node the matching node in a format scene graph
		*	NodeIndexMap.first =  the node identifier in the engine scene graph ( ex: < "box_T0" , "ogre" > )
		*	NodeIndexMap.second = the node identifier in the format scene graph ( ex: < "MY_BOX" , "scene.x3d" > )
		*/
		typedef std::map< NodeIdentifier , NodeIdentifier, NodeCompClass > EngineToFormatMap;
		/** \typedef typedef std::multimap< nodeIdentifier , nodeIdentifier, NodeCompClass > FormatToEngineMap
		*	a multimap that gives for a format scene graph node the matching node in an engine scene graph
		*	NodeIndexMap.first =  the node identifier in the format scene graph ( ex: < "MY_BOX" , "scene.x3d" > ) 
		*	NodeIndexMap.second = the node identifier in the engine scene graph ( ex: < "box_T0" , "ogre" > )
		*/
		typedef std::multimap< NodeIdentifier , NodeIdentifier, NodeCompClass > FormatToEngineMap; 

		//NodeIndexMap m_nodeIndexMap; ///> the map that contains all nodes and their associated information
		EngineToFormatMap m_EngineToFormatMap;
		FormatToEngineMap m_FormatToEngineMap;

	};
};// namespace SceneGraphAdapter

#endif // #ifndef __NodeIndexer_h__