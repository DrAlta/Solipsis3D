/**
* \file FormatAdapter.h
* \brief A virtual class that gives methods to adapt every format scene graph
* Details about FormatAdapter.h
*/
#ifndef __FormatAdapter_h__
#define __FormatAdapter_h__

#include "SceneGraphAdapter.h"
#include <string>
#include <vector>
//#include "EngineWrapperFactory.h"

using namespace std;
using namespace SceneGraphAdapter;

/**
* The FormatAdapter class is a pure virtual class that defines methods to read, access and modify a 3D format scene graph.
* @author Rozenn Bouville Berthelot
* @date 10/01/2011
* @version 0.0
*/

//typedef int (__stdcall *CallbackFunction)(const byte*, const byte*);

//namespace SceneGraphAdapter {

class FormatAdapter {

public:

	
	//FormatAdapter(string sInputFormatExtension){  inputFormatExtension = sInputFormatExtension; }
	
	/**
	* @fn FormatAdapter()
	* @brief Default constructor
	*/
	SCENEGRAPHADAPTER_API FormatAdapter(){
		//this->transformIdx = this->shapeIdx = 0;
	}
	
	/**
	* @fn ~FormatAdapter()
	* @brief Destructor
	*/
	SCENEGRAPHADAPTER_API virtual ~FormatAdapter(){};

	/**
	* @brief Read and access a file and render it using an engine of the Scene Graph Adapter
	* @todo do not use this method, use next one
	* @fn int adaptFormat( const string & fileName, const string & filePath, SGAKernel * sgaKernel)
	* @param fileName url of the input file
	* @param filePath path of the input file
	* @param sgaKernel the kernel of the Scene Graph Adapter
	* @return 0 if failed 1 otherwise
	*/
	SCENEGRAPHADAPTER_API virtual int adaptFormat( const string & fileName, const string & filePath, SGAKernel * sgaKernel) = 0;

	/**
	* @brief Read and access a file and render it using an engine of the Scene Graph Adapter
	* @fn int adaptSceneGraph( SGAKernel * sgaKernel, const string &nodeId = string() )
	* @param sgaKernel the kernel of the Scene Graph Adapter
	* @param nodeId the identifier of the root node of the scene graph to render through the Scene Graph Adapter
	* @return 0 if failed 1 otherwise
	*/
	SCENEGRAPHADAPTER_API virtual int adaptSceneGraph( SGAKernel * sgaKernel, const string &nodeId = string() ) = 0 ;

	/**
	* @brief Set the elapsed time for the scene
	* @fn void updateElapsedTime( const float elapsedTime )
	* @param elapsedTime the time 
	*/
	SCENEGRAPHADAPTER_API virtual void updateElapsedTime( const float elapsedTime ) = 0 ;

	/**
	* @fn void onPress ( const string &nodeId, const float time, const float x, const float y )
	* @brief onPress event on the node idendentified by nodeId
	* @param nodeId the node that has been clicked
	* @param time time of click event
	* @param x x position of the mouse on event
	* @param y y position of the mouse on event
	*/
	SCENEGRAPHADAPTER_API virtual void onPress ( const string &nodeId, const float time, const float x, const float y ) = 0 ;

	/**
	* @fn void onRelease ( const string &nodeId, const float time, const float x, const float y )
	* @brief onRelease event on the node idendentified by nodeId
	* @param nodeId the node that has been released
	* @param time time of release event
	* @param x x position of the mouse on event
	* @param y y position of the mouse on event
	*/
	SCENEGRAPHADAPTER_API virtual void onRelease ( const string &nodeId, const float time, const float x, const float y ) = 0 ;

	/**
	* @fn void onEnter ( const string &nodeId, const float time, const float x, const float y)
	* @brief onEnter event on the node idendentified by nodeId
	* @param nodeId the node that has been touched
	* @param time time of event
	* @param x x position of the mouse on event
	* @param y y position of the mouse on event
	*/
	SCENEGRAPHADAPTER_API virtual void onEnter ( const string &nodeId, const float time, const float x, const float y) = 0 ;

	/**
	* @fn void onExit ( const string &nodeId, const float time, const float x, const float y) 
	* @brief onExit event on the node idendentified by nodeId
	* @param nodeId the mouse cursor exits the unobscured part of the node
	* @param time time of event
	* @param x x position of the mouse on event
	* @param y y position of the mouse on event
	*/
	SCENEGRAPHADAPTER_API virtual void onExit ( const string &nodeId, const float time, const float x, const float y) = 0 ;

	/**
	* @fn void onDrag ( const string &nodeId, const float time, const float x, const float y )
	* @brief onDrag event on the node idendentified by nodeId
	* @param nodeId the node that has been dragged
	* @param time time of event
	* @param x x position of the mouse on event
	* @param y y position of the mouse on event
	*/
	SCENEGRAPHADAPTER_API virtual void onDrag ( const string &nodeId, const float time, const float x, const float y ) = 0 ;

	/**
	* @fn void updateNodePosition( const string &nodeId, const std::vector<float> position, const std::vector<float> rotation )
	* @brief update the position of a geometry identified by nodeId, generaly called after a physics engine update
	* @param nodeId the nodeId of the geometry whose transform needs to be updated
	* @param position the new translation of the node
	* @param rotation the new rotation of the node in quaternion value
	*/
	SCENEGRAPHADAPTER_API virtual void updateNodePosition( const string &nodeId, const std::vector<float> position, const std::vector<float> rotation ) = 0;

	/**
	* @fn std::string getNodeFieldValue ( const std::string &nodeId,  const string &fieldId )
	* @brief given a node identifier and a field name retrieve the value of the field 
	* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
	* @param fieldId the name of the field
	* @return the value of the field as a string
	*/
	SCENEGRAPHADAPTER_API virtual std::string getNodeFieldValue ( const std::string &nodeId,  const string &fieldId ) = 0;

	/**
	* @fn void setNodeFieldValue( const std::string &nodeId, const string &fieldId, const string &sValue )
	* @brief given a node identifier and a field name set the value of the field 
	* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
	* @param fieldId the name of the field
	*/
	SCENEGRAPHADAPTER_API virtual void setNodeFieldValue( const std::string &nodeId, const string &fieldId, const string &sValue ) = 0;

	/**
	* @fn void matchNodeFieldValue( const std::string &fromNodeId, const std::string &toNodeId, const std::string &toNodeFileName, const std::vector<std::string> fields )
	* @brief given a node identifier and a list of fields set its values to the node "toNode" of the file "toNodeFile"
	* @param fromNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
	* @param toNodeId the node identifier to set value of ( ex: "SPHERE_2" )
	* @param toNodeFileName the name of the file ( ex: "spheres.x3d" )
	* @param fields the name of the field ( ex: "dynamic mass" )
	*/
	SCENEGRAPHADAPTER_API virtual void matchNodeFieldValue( const std::string &fromNodeId, const std::string &toNodeId, const std::string &toNodeFileName, const std::vector<std::string> fields ) = 0;



protected:

	/*int transformIdx;
	int shapeIdx;*/

private:

	

};

//} //namespace SceneGraphAdapter
#endif // __FormatAdapter_h__