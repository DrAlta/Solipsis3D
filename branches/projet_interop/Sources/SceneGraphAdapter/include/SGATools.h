/**
* \file SGATools.h
* \brief This class gives tools to work with the SceneGraphAdapter
*/
#ifndef _SGATools_H
#define _SGATools_H

#include <string>
#include <sstream>  
#include <iostream> 
#include <fstream>
#include "SceneGraphAdapter.h"
#include "NodeIndexer.h"

/**
* The SGATools class gives tools to work with the SceneGraphAdapter 
* @class SGATools
* @author Rozenn Bouville Berthelot
* @date 11/01/2011
* @version 0.0
*/

using namespace std;

namespace SceneGraphAdapter {

	class SGATools {

	public:

		enum NodeType {
			GeometryNode,		/*!<  a geometry node */
			MaterialNode,		/*!<  a material node */
			TextureNode,		/*!<  a texture node */
			ViewpointNode,		/*!<  a viewpoint node */
			TransformNode,		/*!<  a transform node */
			GroupNode,			/*!<  a group node */
			SensorNode,			/*!<  a sensor node (TimeSensor, TouchSensor, ...) */
			InlineNode,			/*!<  an inline node */
			GenericNode,		/*!<  a generic node */
			UnknownNode			/*!< default node type */
		};

		/**
		* @fn static std::string getUniqueFormatNodeId( int nodeIdx = -1, , NodeType nType = NodeType::UnknownNode )
		* @brief get a unique string that identify a node in a format scene graph if it has not already an identifier
		* @param nodeIdx an int that is the node index in the format scene graph, this index depend on the traversal used by the parser in the FormaWrapper ( ex: 5 )
		* @param nType the type of the node among SGATools::NodeType
		* @return a string that gives a unique identifier for the node of index nodeIdx ( ex: "SGA_Transform_5" )
		*/
		SCENEGRAPHADAPTER_API static std::string getUniqueFormatNodeId( int nodeIdx = -1, NodeType nType = NodeType::UnknownNode ){

			ostringstream oss;
			string res = "";
			if(nType==NodeType::GeometryNode) oss << "SGA_Geometry_";
			else if(nType == NodeType::ViewpointNode) oss << "SGA_Viewpoint_";
			else if(nType == NodeType::TransformNode) oss << "SGA_Transform_";
			else if(nType == NodeType::GroupNode) oss << "SGA_Group_";
			else if(nType == NodeType::SensorNode) oss << "SGA_Sensor_";
			else if(nType == NodeType::MaterialNode) oss << "SGA_Material_";
			else if(nType == NodeType::TextureNode) oss << "SGA_Texture_";
			else if(nType == NodeType::GenericNode) oss << "SGA_Generic_";
			else oss << "SGA_";
			oss << nodeIdx;
			res = oss.str();	
			return res;
		}

		/**
		* @fn static string getUniqueEngineNodeId( string sInputFileName, string sNodeFormatId, string sEngineId )
		* @brief get a unique string that identify a node in an engine scene graph
		* @param sInputFileName the input file name of the format scene graph that contain the matching node ( ex: "duck.dae" )
		* @param sNodeFormatId the matching node identifier in the format scene graph ( ex: "SGA_Transform_5" )
		* @param sEngineId the identifier of the engine ( ex: "ogre" )
		* @return a string that gives a unique identifier for the node in the engine scene graph ( ex: "duck_SGA_Transform_5_ogre" )
		*/
		SCENEGRAPHADAPTER_API static string getUniqueEngineNodeId( string sInputFileName, string sNodeFormatId, string sEngineId ){

			string sShortFileName;
			size_t lastSlashFile = sInputFileName.find_last_of("/\\") + 1;
			//size_t tempSlash = sInputFileName.find_last_of("/\\") + 1;
			//if(lastSlashFile<tempSlash) lastSlashFile = tempSlash;

			string szMeshPrefix = sInputFileName.substr(0,sInputFileName.length()-4);
			size_t lastSlash = szMeshPrefix.find_last_of("/\\");
			sShortFileName = szMeshPrefix.substr(lastSlash+1);

			string res = sShortFileName + "_" + sNodeFormatId + "_" + sEngineId;	
			return res;
		}


		/**
		* @fn void addInNodeIndexer(const string &sFormatNodeId, const string &sFileName, const string &sEngineNodeId,  const string &sEngine )
		* @brief update the NodeIndexer for the SceneGraphAdapter application with the node specified by its FormatId, sFileName, sEngineId, sEngine
		* @param sFormatNodeId the identifier of the node in the format scene graph ( ex: "MY_SPHERE" )
		* @param sFileName the input file name of the node ( ex: "..\samples\scene.x3d" )
		* @param sEngineNodeId the identifier of the node in the engine scene graph ( ex: "scene_MY_SPHERE_ogre" )
		* @param sEngine the engine identifier ( ex: "ogre" )
		*/
		SCENEGRAPHADAPTER_API static int addInNodeIndexer(const string &sFormatNodeId, const string &sFileName, const string &sEngineNodeId,  const string &sEngine ){
			if(sFormatNodeId.empty() || sFileName.empty() || sEngineNodeId.empty() || sEngine.empty() ) return 0;
			else {
				NodeIndexer::getInstance()->addNode(sFormatNodeId,sFileName,sEngineNodeId,sEngine);
				return 1;
			}
		}

		SCENEGRAPHADAPTER_API static int deleteInNodeIndexer(const string &sNodeId, const string &sSceneGraphId, bool isFormatNode = true ){
			if(sNodeId.empty() || sSceneGraphId.empty()) return 0;
			return NodeIndexer::getInstance()->deleteNode(sNodeId,sSceneGraphId,isFormatNode);
		}

	private:

		//NodeIndexer * m_nodeIndex;


	};
} // namespace SceneGraphAdapter

#endif // _SGATools_H