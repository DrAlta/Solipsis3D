/**
* \file SGAKernel.h
* \brief The kernel of the scene graph adapter
*
* Details about SGAKernel.h
*/


#ifndef _SGA_KERNEL_H
#define _SGA_KERNEL_H

#include "WrapperPlugin.h"
#include "SceneGraphAdapter.h"
#include "FormatWrapperFactory.h"
#include "EngineWrapperFactory.h"

#include "SGATools.h"
#include "NodeIndexer.h"


#include <string>
#include <map>

#define SGA_3DCONTAINER_EXTENSION "3dc"
#define SGA_UNCHANGED_FLOAT -1

namespace SceneGraphAdapter {

	/**
	* The SGAKernel class is the entry point of the SceneGraphAdapter API
	* @class SGAKernel
	* @author Rozenn Bouville Berthelot
	* @date 13/01/2011
	* @version 0.0
	*/
	class SGAKernel  {

	public:

		enum TransformSpace {
			WORLD,		/*!< transform relative to the world */
			PARENT,		/*!< transform relative to parent node */
			LOCAL		/*!< local transform */
		};

		SCENEGRAPHADAPTER_API SGAKernel(){
			m_LoadedWrapper = std::map< std::string, WrapperPlugin > ();
			m_AvailableFormatWrapperMap = std::map <std::string, std::string> ();

			m_EngineWrapper = EngineWrapperFactory(); 
			m_FormatWrapper = FormatWrapperFactory();

			m_sCurrentFileName = string();

		}

		SCENEGRAPHADAPTER_API ~SGAKernel(){
			m_LoadedWrapper.clear();
			m_AvailableFormatWrapperMap.clear();
			m_sCurrentFileName.clear();
		}

		
		/// Access the format wrapper list
		SCENEGRAPHADAPTER_API FormatWrapperFactory &getFormatWrappers() { return m_FormatWrapper; }
		/// Access the engine wrapper list
		SCENEGRAPHADAPTER_API EngineWrapperFactory &getEngineWrapper() { return m_EngineWrapper; }

		SCENEGRAPHADAPTER_API string getCurrentFileName() { 
			if(m_sCurrentFileName.empty()) return NULL;
			else return m_sCurrentFileName; 
		}


		/**
		* @fn void loadFile( const std::string &sFileName, const std::string &sFilePath, const std::string &sRendererIdentifier )
		* @brief given an input file, load the appropriate plugin and render it in the first rendering engine set
		* @param sFileName the input file ( ex: "box.x3d" or "duck.dae" )
		* @param &sFilePath the current path ( mainly for setting ogre ressources ) ( ex: "..\medias\" )
		* @param &sRendererIdentifier  the identifier of the rendering engine to use  ( ex: "ogre" )
		* @todo: suppress param sFilePath
		*/
		SCENEGRAPHADAPTER_API void loadFile( const std::string & sFileName, const std::string &sFilePath, const std::string &sRendererIdentifier = string() );


		/**
		* @fn size_t loadPlugin(const std::string &sPluginName)
		* @brief Loads a plugin
		* @param sPluginName the plugin file name ( ex: "X3dWrapper.dll" or "OgreWrapper.dll" )
		* @return the index of the Wrapper associated in m_LoadedWrapper
		*/
		SCENEGRAPHADAPTER_API int loadPlugin( const std::string &sPluginName );

		/**
		* @fn size_t downloadPlugin(const std::string &sUrl)
		* @brief Download a plugin given its url
		* @param sUrl the plugin url
		* @return the index of the Wrapper associated in m_LoadedWrapper
		*/
		SCENEGRAPHADAPTER_API int downloadPlugin( const std::string &sUrl );


		/**
		* @fn size_t loadAppropriatePlugin (const std::string &sFilename)
		* @brief given an input file, load the appropriate plugin ( ex: X3dWrapper.dll for an X3D file )
		* @param sFilename the input file ( ex: "..\box.x3d" or "duck.dae" )
		* @return the index of the WrapperPlugin in m_FormatWrapper; -1 if no appropriate plugin has been found
		*/
		SCENEGRAPHADAPTER_API int loadAppropriatePlugin (const std::string &sFilename);


		/**
		* @fn  void registerPlugin (const std::string &sFilename)
		* @param sFilename the dll file name to register
		* @brief check if the dll is valid, in that case takes its configuration info to check its type, its input file format and so on, finally add an entry in SGAKernel::m_AvailableFormatWrapperMap
		*/
		SCENEGRAPHADAPTER_API void registerPlugin (const std::string &sFilename);

		/**
		* @fn  void updateSceneTime (const float elapsedTime)
		* @param elapsedTime time elapsed since the beginning of the rendering
		* @brief update the time and notify all FormatWrappers
		*/
		SCENEGRAPHADAPTER_API void updateSceneTime (const float elapsedTime);


		/**
		* @fn void onPress(const std::string &nodeName, const std::string &engineId, const float clickTime, const float x, const float y, const std::string & parentNodeName = std::string() )
		* @param nodeName the name of the node in the renderer scene graph
		* @param engineId the identifier of the renderer scene graph
		* @param clickTime the time
		* @param x x position of mouse pointer
		* @param y y position of mouse pointer
		* @param parentNodeName the name of the node parent from the clicked node; if the parentNodeName and nodeName are not from the same format scene graph, then the sub-scene graph of parentNodeName 
		* has been included through an inline mechanism
		* @brief notify the FormatWrapper concerned that the node nodeName has been clicked
		*/
		SCENEGRAPHADAPTER_API void onPress(const std::string &nodeName, const std::string &engineId, const float clickTime, const float x, const float y, const std::string & parentNodeName = std::string() );

		/**
		* @fn void onRelease(const std::string &nodeName, const std::string &engineId, const float clickTime, const float x, const float y, const std::string & parentNodeName = std::string() )
		* @param nodeName the name of the node in the renderer scene graph
		* @param engineId the identifier of the renderer scene graph
		* @param clickTime the time
		* @param x x position of mouse pointer
		* @param y y position of mouse pointer
		* @param parentNodeName the name of the node parent from the released node; if the parentNodeName and nodeName are not from the same format scene graph, then the sub-scene graph of parentNodeName 
		* has been included through an inline mechanism
		* @brief notify the FormatWrapper concerned that the node nodeName has been released
		*/
		SCENEGRAPHADAPTER_API void onRelease(const std::string &nodeName, const std::string &engineId, const float clickTime, const float x, const float y, const std::string & parentNodeName = std::string() );

		/**
		* @fn void onEnter ( const string &nodeName, const string &engineId, const float touchTime, const float x, const float y, const std::string & parentNodeName = std::string()  )
		* @param nodeName the name of the node in the renderer scene graph
		* @param engineId the identifier of the renderer scene graph
		* @param touchTime the time
		* @param x x position of mouse pointer
		* @param y y position of mouse pointer
		* @param parentNodeName the name of the node parent from the released node; if the parentNodeName and nodeName are not from the same format scene graph, then the sub-scene graph of parentNodeName 
		* has been included through an inline mechanism
		* @brief notify the FormatWrapper concerned that the node nodeName has been touched, i.e. the pointing device pointer is over the node
		*/
		SCENEGRAPHADAPTER_API void onEnter ( const string &nodeName, const string &engineId, const float touchTime, const float x, const float y, const std::string & parentNodeName = std::string()  );

		/**
		* @fn void onExit ( const string &nodeName, const string &engineId, const float touchTime, const float x, const float y, const std::string & parentNodeName = std::string()  )
		* @param nodeName the name of the node in the renderer scene graph
		* @param engineId the identifier of the renderer scene graph
		* @param touchTime the time
		* @param x x position of mouse pointer
		* @param y y position of mouse pointer
		* @param parentNodeName the name of the node parent from the released node; if the parentNodeName and nodeName are not from the same format scene graph, then the sub-scene graph of parentNodeName 
		* has been included through an inline mechanism
		* @brief notify the FormatWrapper concerned that the pointing device pointer is no more over the node
		*/
		SCENEGRAPHADAPTER_API void onExit ( const string &nodeName, const string &engineId, const float touchTime, const float x, const float y, const std::string & parentNodeName = std::string()  );

		/**
		* @fn string createTransformNode( const string &sNodeId, const string &sFileName, const std::vector<float> &translation, const std::vector<float> &scale, const std::vector<float> &rotation, const string &sEngineId = string())
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param translation translation parameters for the transform node to create
		* @param scale scale parameters for the transform node to create
		* @param rotation rotation parameters as quaternion for the transform node to create 
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief create a new transform node in all engines of the SGA if sEngineId is empty, in the engine identified by sEngineId otherwise
		*/
		SCENEGRAPHADAPTER_API string createTransformNode( const string &sNodeId, const string &sFileName, const std::vector<float> &translation, const std::vector<float> &scale, const std::vector<float> &rotation, const string &sEngineId = string());

		/**
		* @fn string createGroupNode( const string &sNodeId, const string &sFileName, const string &sEngineId = string())
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief create a new group node in all engines of the SGA if sEngineId is empty, in the engine identified by sEngineId otherwise
		*/
		SCENEGRAPHADAPTER_API string createGroupNode( const string &sNodeId, const string &sFileName, const string &sEngineId = string());

		/**
		* @fn void setTransformNode( const string &sNodeId, const string &sFileName, const std::vector<float> &translation, const std::vector<float> &rotation, const std::vector<float> &scale, const TransformSpace relativeTo = TransformSpace::PARENT, const string &sEngineId = string())
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param translation translation parameters for the transform node to create
		* @param scale scale parameters for the transform node to create
		* @param rotation rotation parameters as quaternion for the transform node to create 
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		 @param relativeTo the transform space used by the transform, relative to parent node by default
		* @brief set transform node in all engines of the SGA if sEngineId is empty, in the engine identified by sEngineId otherwise
		*/
		SCENEGRAPHADAPTER_API void setTransformNode( const string &sNodeId, const string &sFileName, const std::vector<float> &translation, const std::vector<float> &rotation, const std::vector<float> &scale, const TransformSpace relativeTo = TransformSpace::PARENT, const string &sEngineId = string());

	
		/**
		* @fn void closeGroupNode ( const string &sNodeId, const string &sFileName, const string &sEngineId = string())
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief close the current group node ( aka group, transform, orderedgroup nodes )
		*/
		SCENEGRAPHADAPTER_API string closeGroupNode ( const string &sNodeId, const string &sFileName, const string &sEngineId = string()); 

		/**
		* @fn string createViewpointNode ( const string &sNodeId, const string &sFileName, const std::vector<float> &position, const std::vector<float> &orientation, const string &sEngineId = string() )
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param position position of the viewpoint
		* @param orientation orientation of the viewpoint
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief create a new viewpoint node
		*/
		SCENEGRAPHADAPTER_API string createViewpointNode ( const string &sNodeId, const string &sFileName, const std::vector<float> &position, const std::vector<float> &orientation, const string &sEngineId = string() ); 

		/**
		* @fn string createMesh ( 
			const string & sNodeId, 
			const string & sFileName,
			const std::vector<float> & verticesArray,
			const std::vector<float> & normalsArray,
			const std::vector<float> & textureCoordArray,
			const std::vector<float> & coloursArray,
			const std::vector<unsigned short> & facesArray,
			const string & sEngineId = string()
			)
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param verticesArray vertices
		* @param normalsArray normals
		* @param textureCoordArray texture coordinates, texture's point of origin is low left
		* @param coloursArray colours
		* @param facesArray faces
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief create a new IFS shape node
		*/
		SCENEGRAPHADAPTER_API string createMesh ( 
			const string & sNodeId, 
			const string & sFileName,
			const std::vector<float> & verticesArray,
			const std::vector<float> & normalsArray,
			const std::vector<float> & textureCoordArray,
			const std::vector<float> & coloursArray,
			const std::vector<unsigned short> & facesArray,
			const string & sEngineId = string()
			); 

		/**
		* @fn string createPlane( const string &sNodeId, const string &sFileName, const float width, const float height, const std::vector<float> &orientation, const string &sEngineId = string() )
		* @brief Create a plane of the given width, height and orientation identified by shapeId
		* @param width the width of the plane to create
		* @param height the height of the plane to create
		* @param orientation the orientation of the plane to create
		* @param sNodeId the identifier of the plane to create ( ex: "PLANE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		 @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @return the new node identifier in the engine scene graph or an empty string if the creation failed
		*/
		virtual string createPlane( const string &sNodeId, const string &sFileName, const float width, const float height, const std::vector<float> &orientation, const string &sEngineId = string() );

		/**
		* @fn string createGround( const string &sNodeId, const string &sFileName, const std::vector<float> &normal, const float constant, const std::vector<float> &color = std::vector<float>(3,0.5), const std::string urlTexture = std::string(), const string &sEngineId = string() )
		* @brief Create a infinite ground identified by sNodeId
		* @param sNodeId the identifier of the ground to create ( ex: "GROUND" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param normal the normal of the ground
		* @param constant the height of the plane to create
		* @param color the color of the plane to create
		* @param urlTexture the texture of the plane to create
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @return the new node identifier in the engine scene graph or an empty string if the creation failed
		*/
		virtual string createGround( const string &sNodeId, const string &sFileName, const std::vector<float> &normal, const float constant, const std::vector<float> &color = std::vector<float>(3,0.5), const std::string urlTexture = std::string(), const string &sEngineId = string() );

		/**
		* @fn string createBoxShape ( const string &sNodeId, const string &sFileName, const std::vector<float> &size, const string &sEngineId = string() )
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param size the size of the box
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief create a new box shape node
		*/
		SCENEGRAPHADAPTER_API string createBoxShape ( const string &sNodeId, const string &sFileName, const std::vector<float> &size, const string &sEngineId = string() ); 

		/**
		* @fn string createSphereShape ( const string &sNodeId, const string &sFileName, float radius, const string &sEngineId = string() )
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param radius the radius of the sphere
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief create a new sphere shape node
		*/
		SCENEGRAPHADAPTER_API string createSphereShape ( const string &sNodeId, const string &sFileName, float radius, const string &sEngineId = string() ); 


		/**
		* @fn string createCylinderShape ( const string &sNodeId, const string &sFileName, float radius, float height,  const string &sEngineId = string() )
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param radius the radius of the cylinder
		* @param height the height of the cylinder
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief create a new cylinder shape node
		*/
		SCENEGRAPHADAPTER_API string createCylinderShape ( const string &sNodeId, const string &sFileName, float radius, float height,  const string &sEngineId = string() ); 

		/**
		* @fn string createMaterialProperty ( 
			const string sNodeId, 
			const string sFileName, 
			const float ambientIntensity, 
			const float shininess, 
			const float alpha, 
			const std::vector<float> &diffuseColor, 
			const std::vector<float> &emissiveColor, 
			const std::vector<float> &specularColor,
			const string sEngineId = string()
			)
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param ambientIntensity the radius of the sphere
		* @param shininess the radius of the sphere
		* @param alpha the radius of the sphere
		* @param diffuseColor the radius of the sphere
		* @param emissiveColor the radius of the sphere
		* @param specularColor the radius of the sphere
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief create a new material property node
		*/
		SCENEGRAPHADAPTER_API string createMaterialProperty ( 
			const string sNodeId, 
			const string sFileName, 
			const float ambientIntensity, 
			const float shininess, 
			const float alpha, 
			const std::vector<float> &diffuseColor, 
			const std::vector<float> &emissiveColor, 
			const std::vector<float> &specularColor,
			const string sEngineId = string()
			); 

		/**
		* @fn void setMaterialProperty ( 
			const string sNodeId, 
			const string sFileName, 
			const float ambientIntensity, 
			const float shininess, 
			const float alpha, 
			const std::vector<float> &diffuseColor, 
			const std::vector<float> &emissiveColor, 
			const std::vector<float> &specularColor,
			const string sEngineId = string()
			)
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param ambientIntensity the radius of the sphere
		* @param shininess the radius of the sphere
		* @param alpha the radius of the sphere
		* @param diffuseColor the radius of the sphere
		* @param emissiveColor the radius of the sphere
		* @param specularColor the radius of the sphere
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief set material property node
		*/
		SCENEGRAPHADAPTER_API void setMaterialProperty ( 
			const string sNodeId, 
			const string sFileName, 
			const float ambientIntensity, 
			const float shininess, 
			const float alpha, 
			const std::vector<float> &diffuseColor, 
			const std::vector<float> &emissiveColor, 
			const std::vector<float> &specularColor,
			const string sEngineId = string()
			); 

		/**
		* @fn string createTextureProperty ( const string &sNodeId, const string &sFileName, const string &sTexture, const string &sEngineId = string() )
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param sTexture the texture file
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief create a new texture property node
		*/
		SCENEGRAPHADAPTER_API string createTextureProperty ( const string &sNodeId, const string &sFileName, const string &sTexture, const string &sEngineId = string() ); 

		/**
		* @fn void attachShapeToCurrentGroupNode ( const string &sNodeId, const string &sFileName, const string &sEngineId = string())
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief attach the current shape and its properties ( material, texture) to the current group node ( transform, group node )
		*/
		SCENEGRAPHADAPTER_API void attachShapeToCurrentGroupNode ( const string &sNodeId, const string &sFileName, const string &sEngineId = string()); 

		/**
		* @fn void deleteNode ( const string &sNodeId, const string &sFileName, const string &sEngineId = string() ) 
		* @brief delete the node identified by sNodeId in sFileName format scene graph ( also delete all its attached children) in the engine scene graph identify by sEngineId if its not empty in every engine otherwise
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		* @brief delete the node identify by sNodeId in the format scene graph in the engine identify by sEngineId if its not empty in every engine otherwise
		*/
		SCENEGRAPHADAPTER_API void deleteNode ( const string &sNodeId, const string &sFileName, const string &sEngineId = string() );

		/**
		* @fn void deleteNodeChildren ( const string &sNodeId, const string &sFileName, const string &sEngineId = string() )
		* @brief delete children of the node identified by sNodeId in sFileName format scene graph in the engine scene graph identify by sEngineId if its not empty in every engine otherwise
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param sEngineId the identifier of the engine, can be empty ( ex: "ogre" )
		*/
		SCENEGRAPHADAPTER_API void deleteNodeChildren ( const string &sNodeId, const string &sFileName, const string &sEngineId = string() );

		/**
		* @fn void updateNodePosition( const string &sNodeId, const string &sEngineId, const std::vector<float> position, const std::vector<float> rotation)
		* @brief update the position of a node in a format scene graph given the matching node in an engine scene graph
		* @param sNodeId the node identifier in the engine scene graph ( ex: "SCENE_SPHERE_1_OGRE" )
		* @param sEngineId the identifier of the engine ( ex: "ogre" )
		* @param position the new position of the node
		* @param rotation the new rotation of the node as quaternion
		*/
		SCENEGRAPHADAPTER_API void updateNodePosition( const string &sNodeId, const string &sEngineId, const std::vector<float> position, const std::vector<float> rotation);


		/**
		* @fn void setNodePhysicsProperties ( const string &sNodeId, const string &sFileName, const float fMass, const std::vector<float> angularVelocity = std::vector(3,0), const std::vector<float> velocity = std::vector(3,0) )
		* @brief set physical properties of a node identified by nodeId
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param sFileName the input file name ( ex: "..\samples\scene.x3d" )
		* @param fMass the mass of the node
		* @param fRestitution restitution to set
		* @param fFriction friction to set
		* @param isDynamic  if the node is dynamic
		*/
		//SCENEGRAPHADAPTER_API void setNodePhysicsProperties ( const string &sNodeId, const string &sFileName, const float fMass, const std::vector<float> angularVelocity = std::vector<float>(3,0), const std::vector<float> velocity = std::vector<float>(3,0) );
		SCENEGRAPHADAPTER_API void setNodePhysicsProperties ( 
			const string &sNodeId, 
			const string &sFileName, 
			const float fMass = SGA_UNCHANGED_FLOAT, 
			const float fRestitution = SGA_UNCHANGED_FLOAT, 
			const float fFriction = SGA_UNCHANGED_FLOAT, 
			const bool isDynamic = true  );

		/**
		* @fn void setWorldPhysicsProperties ( const std::vector<float> &vGravity, const float &fTimeStep )
		* @brief set properties of the physical world
		* @param vGravity the gravity ( the world is Y_up_axis ) 
		* @param fTimeStep the time step for simulation
		*/
		SCENEGRAPHADAPTER_API void setNodeMass ( const string &sNodeId, const string &sFileName, const float fMass );

		/**
		* @fn std::string getNodeFieldValue ( const std::string scenegraphId, const std::string &nodeId,  const string &fieldId )
		* @brief given a scene graph, a node identifier and a field name retrieve the value of the field 
		* @param scenegraphId the input file name ( ex: "..\samples\scene.x3d" ) or the engine identifier ( ex: "ogre" )
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param fieldId the name of the field
		*/
		SCENEGRAPHADAPTER_API std::string getNodeFieldValue ( const std::string scenegraphId, const std::string &nodeId,  const string &fieldId );

		/**
		* @fn void setNodeFieldValue( const std::string scenegraphId, const std::string &nodeId, const string &fieldId, const string &sValue )
		* @param scenegraphId the input file name ( ex: "..\samples\scene.x3d" ) or the engine identifier ( ex: "ogre" )
		* @brief given a scene graph, a node identifier, a field name and a value, updates the value of the field
		* @param sNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param fieldId the name of the field
		* @param sValue the new value of the field
		*/
		SCENEGRAPHADAPTER_API void setNodeFieldValue( const std::string scenegraphId, const std::string &nodeId, const string &fieldId, const string &sValue );

		/**
		* @fn void setWorldPhysicsProperties ( const std::vector<float> &vGravity, const float &fTimeStep )
		* @brief set properties of the physical world
		* @param vGravity the gravity ( the world is Y_up_axis ) 
		* @param fTimeStep the time step for simulation
		*/
		SCENEGRAPHADAPTER_API void setWorldPhysicsProperties ( const std::vector<float> &vGravity, const float &fTimeStep );

		/**
		* @fn void matchNodeFieldValue( const std::string &fromNodeId, const std::string &toNodeId, const std::string &toNodeFileName, const std::vector<std::string> fields )
		* @brief given a node identifier and a list of fields set its values to the node "toNode" of the file "toNodeFile"
		* @param fromNodeId the node identifier in the format scene graph ( ex: "SPHERE_1" )
		* @param fromScenegraphId the input file name ( ex: "..\samples\scene.x3d" )
		* @param toNodeId the node identifier to set value of ( ex: "SPHERE_2" )
		* @param toNodeFileName the name of the file ( ex: "spheres.x3d" )
		* @param fields the name of the field ( ex: "dynamic mass" )
		*/
		SCENEGRAPHADAPTER_API void matchNodeFieldValue( const std::string &fromNodeId, const std::string fromScenegraphId, const std::string &toNodeId, const std::string &toNodeFileName, const std::vector<std::string> fields );

	private:


		/** \typedef typedef  std::map< std::string,WrapperPlugin > WrapperMap 
		*	WrapperMap.first =  dll name (ex: "X3dWrapper.dll" )
		*	WrapperMap.second = the loaded wrapper
		*/
		typedef std::map< std::string, WrapperPlugin > WrapperMap; 

		/** @typedef typedef std::map<std::string, std::string> FormatWrapperMap
		*	@brief A map of available format
		*	FormatWrapperMap.first =  file format extension ( ex: "x3d" )
		*	FormatWrapperMap.second = the name of the associated dll ( ex: "X3dWrapper.dll" )
		*/
		typedef std::map<std::string, std::string> FormatWrapperMap;

		WrapperMap      m_LoadedWrapper;  ///< All plugins currently loaded
		FormatWrapperMap m_AvailableFormatWrapperMap; ///<All the format wrappers availables

		EngineWrapperFactory m_EngineWrapper; 
		FormatWrapperFactory  m_FormatWrapper;

		string m_sCurrentFileName; ///< the input file name currently load

		/**
		* @fn  void printError(int i, std::string message = std::string(),std::string message2 = std::string())
		* @param i error code
		* @param message message that explain the error
		* @param message2 message that explain the error
		* @details A general method for errors:
		* - 0: no appropriate plugin has been found for a given file
		* - 1: no rendering engine set
		* - 2: wrapper configuration not correctly set
		* - 3: a wrapper with the same identifier has already been loaded
		* - 4: File not found in NodeIndexer
		* - 5: Unable to load file
		* - 6: Node not found in a scenegraph
		*/
		void printError(int i, std::string message = std::string(),std::string message2 = std::string()){

			if(i==0) printf("SGAKernel => No appropriate plugin found for format \"%s\" \n",message.c_str());
			else if(i==1) printf("SGAKernel => No rendering engine set for file \"%s\" \n",message.c_str());
			else if(i==2) printf("SGAKernel => The plugin \"%s\" is not correctly set, can not find the input format extension process by this Format Wrapper \n",message.c_str());
			else if(i==3) printf("SGAKernel => A Format Wrapper for this file format \"%s\" has already  been loaded. \n",message.c_str());
			else if(i==4) printf("SGAKernel => File \"%s\" not found in NodeIndexer\n",message.c_str());
			else if(i==5) printf("SGAKernel => Unable to load file \"%s\" \n",message.c_str());
			else if(i==6) printf("SGAKernel => Node \"%s\" not found in  \"%s\" scenegraph.\n",message.c_str(),message2.c_str());
		}

	};

} // namespace SceneGraphAdapter

#endif // _SGA_KERNEL_H