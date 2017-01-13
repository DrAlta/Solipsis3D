/**
* \file RendererAdapter.h
* \brief A virtual class that gives methods to adapt every rendering engine scene graph
* Details about RendererAdapter.h
*/

#ifndef __RendererAdapter_h__
#define __RendererAdapter_h__

#include <vector>
#include <string>

using namespace std;

/**
* The RendererAdapter class is a pure virtual class that gives methods to adapt every rendering engine scene graph.
* @author Rozenn Bouville Berthelot
* @date 10/01/2011
* @version 0.0
*/

class RendererAdapter {

public:

	enum TransformSpace {
			WORLD,		/*!< transform relative to the world */
			PARENT,		/*!< transform relative to parent node */
			LOCAL		/*!< local transform */
		};

	/**
	* @fn RendererAdapter()
	* @brief Constructor
	*/
	RendererAdapter(){};
	/**
	* @fn ~RendererAdapter()
	* @brief Destructor
	*/
	SCENEGRAPHADAPTER_API virtual ~RendererAdapter(){};

	/* SCENE */

	/**
	* @fn string createViewpoint( const std::vector<float> &position, const std::vector<float> &orientation, const string &viewpointId = string() )
	* @brief Set the viewpoint: create a new viewpoint node identified by viewpointId in SGA
	* @param position a vector of 3 float for the new position
	* @param orientation a vector of 3 float for the new orientation
	* @param viewpointId a string that identify the viewpoint
	* @return the new node identifier in the renderer scene graph
	*/
	SCENEGRAPHADAPTER_API virtual string createViewpoint( const std::vector<float> &position, const std::vector<float> &orientation, const string &viewpointId ) = 0;
	/**
	* @fn string createCamera ( const std::vector<float> &position, const std::vector<float> &orientation,  const string &viewpointId = string() ) 
	* @brief set the camera; not implemented in OgreWrapper
	* @return the new node identifier in the renderer scene graph
	*/
	SCENEGRAPHADAPTER_API virtual string createCamera ( const std::vector<float> &position, const std::vector<float> &orientation,  const string &viewpointId ) = 0;
	/**
	* @fn string createViewport ( const float left, const float top, const float width, const float height )
	* @brief set the viewport; not implemented in OgreWrapper
	* @return the new node identifier in the renderer scene graph
	*/
	SCENEGRAPHADAPTER_API virtual string createViewport ( const float left, const float top, const float width, const float height ) = 0;


	/* SCENEGRAPH */
	/* 
	* Create, close and delete node in the renderer scene graph
	*/
	//virtual void createNode (string nodeId) = 0;
	/**
	* @fn void closeNode (const string &nodeId ) 
	* @brief Enable to close the current grouping node of the renderer scene graph; the current grouping node become the parent of the node nodeId
	* @param nodeId the identifier in SGA of the node to close
	*/
	SCENEGRAPHADAPTER_API virtual void closeNode (const string &nodeId ) = 0;

	/**
	* @fn void showNode (const string &nodeId )
	* @brief Enable to reveal a node
	* @param nodeId the node to show
	*/
	SCENEGRAPHADAPTER_API virtual void showNode (const string &nodeId ) = 0;

	/**
	* @fn void hideNode (const string &nodeId )
	* @brief Enable to hide a node
	* @param nodeId the node to hide
	*/
	SCENEGRAPHADAPTER_API virtual void hideNode (const string &nodeId ) = 0;

	/**
	* @fn std::vector<string> deleteNode (const string &nodeId )
	* @brief Delete the node identified by nodeId
	* @param nodeId the identifier of the node to delete
	* @return an array of the node identifiers of all the node that have been deleted ( the node nodeId and all the attached children ) or an empty array if the deletion failed
	*/
	SCENEGRAPHADAPTER_API virtual std::vector<string> deleteNode (const string &nodeId ) = 0;

	/**
	* @fn std::vector<string> deleteNodeChildren (const string &nodeId )
	* @brief Delete all children of the node identified by nodeId
	* @param nodeId the identifier of the parent node
	* @return an array of the node identifiers of all the node that have been deleted or an empty array if the deletion failed
	*/
	SCENEGRAPHADAPTER_API virtual std::vector<string> deleteNodeChildren (const string &nodeId ) = 0;

	/**
	* @fn void attachShapeToSceneGraph( const string &shapeId, const bool show = true, const bool cascade = false )
	* @brief Attach the shape identified by shapeId to the current grouping node of the renderer scene graph;
	* it also applied the current material and the current texture to the shape 
	* @param shapeId the identifier of the shape to attach
	* @param show set if the shape is visible or not
	* @param cascade show or hide also all children node
	*/
	SCENEGRAPHADAPTER_API virtual void attachShapeToSceneGraph( const string &shapeId, const bool show = true, const bool cascade = false )= 0;
	
	/* 
	* Geometry
	*/

	/**
	* @fn string createCylinder(const float radius, const float height, const string &shapeId ) 
	* @brief Create a cylinder shape of the given radius and height, identified by shapeId
	* @param radius the radius of the cylinder to create
	* @param height the height of the cylinder to create
	* @param shapeId the identifier of the cylinder to create
	* @return the new node identifier in the renderer scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createCylinder(const float radius, const float height, const string &shapeId ) = 0;

	/**
	* @fn string createSphere(const float radius, const string &shapeId )
	* @brief Create a sphere shape of the given radius, identified by shapeId
	* @param radius the radius of the sphere to create
	* @param shapeId the identifier of the sphere to create
	* @return the new node identifier in the renderer scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createSphere(const float radius, const string &shapeId )= 0;
	/**
	* @fn string createBox( const std::vector<float> &size, const string &shapeId )
	* @brief Create a box shape of the given size, identified by shapeId
	* @param size the size of the box to create
	* @param shapeId the identifier of the box to create
	* @return the new node identifier in the renderer scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createBox( const std::vector<float> &size, const string &shapeId )= 0;
	//virtual void createIFSGeometry( const std::vector<float> &vertices, const std::vector<unsigned short> &faces, const std::vector<float> &colours, const std::vector<unsigned short> &colorIdx, int nbVertices, int nbFaces, string shapeId = string() )= 0;
	

	/**
	* @fn sstring createPlane( const float width, const float height, const std::vector<float> &orientation, const string &shapeId )
	* @brief Create a plane of the given width and height, identified by shapeId
	* @param width the width of the plane to create
	* @param height the height of the plane to create
	* @param orientation the orientation of the plane to create
	* @param shapeId the identifier of the plane to create
	* @return the new node identifier in the renderer scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createPlane( const float width, const float height, const std::vector<float> &orientation, const string &shapeId )= 0;

	/**
	* @fn string createMesh( 
		const std::vector<float> &vertices,
		const std::vector<float> &normals, 
		const std::vector<float> &textureCoord, 
		const std::vector<float> &colours, 
		const std::vector<unsigned short> &indices, 
		const string &shapeId )
	* @brief Create an idexed face set shape
	* @param vertices the vertices of the IFS
	* @param normals the normals of the IFS, can be empty
	* @param textureCoord the texture coordinates of the IFS, can be empty. The origin point of a texture is low left.
	* @param colours the colours per vertex of the IFS, can be empty
	* @param indices the faces indices of the IFS, can be empty
	* @param shapeId the identifier of the IFS
	* @return the new node identifier in the renderer scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createMesh( 
		const std::vector<float> &vertices,
		const std::vector<float> &normals, 
		const std::vector<float> &textureCoord, 
		const std::vector<float> &colours, 
		const std::vector<unsigned short> &indices, //index
		const string &shapeId )= 0;

	/**
	* @fn string createGround( const string &sNodeId,const std::vector<float> &normal, const float constant, const std::vector<float> &color = std::vector<float>(3,0.5), const std::string urlTexture = std::string() )
	* @brief Create a infinite ground identified by sNodeId
	* @param sNodeId the identifier of the ground to create ( ex: "GROUND" )
	* @param normal the normal of the ground
	* @param constant the height of the plane to create
	* @param color the color of the plane to create
	* @param urlTexture the texture of the plane to create
	* @return the new node identifier in the renderer scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createGround( const string &sNodeId,const std::vector<float> &normal, const float constant, const std::vector<float> &color = std::vector<float>(3,0.5), const std::string urlTexture = std::string() )= 0;


	/*
	* Group
	*/
	/**
	* @fn string createGroupNode( const string &nodeId )
	* @brief Create a group node 
	* @param nodeId the identifier of the group node
	* @return the new node identifier in the renderer scene graph
	*/
	SCENEGRAPHADAPTER_API virtual string createGroupNode( const string &nodeId )= 0;

	/*
	* Transform
	*/
	/**
	* @fn string createTransformNode( 
		const std::vector<float> &translation, 
		const std::vector<float> &scale, 
		const std::vector<float> &rotation, 
		const string &transformId )
	* @brief Create a transformNode identified by transformId, add it to SGA's node index
	* @param translation the translation applied to the transform node
	* @param scale the scale applied to the transform node
	* @param rotation the rotation applied to the transform node as a quaternion
	* @param transformId the identifier of the transform node
	* @return the new node identifier in the renderer scene graph
	*/
	SCENEGRAPHADAPTER_API virtual string createTransformNode( 
		const std::vector<float> &translation, 
		const std::vector<float> &scale, 
		const std::vector<float> &rotation, 
		const string &transformId )= 0;
	//virtual void createTransformNode( std::vector<double> translation, std::vector<double> scale, std::vector<double> rotation )= 0;
	
	/**
	* @fn void setTransformNode( 
		const string &nodeId, 
		const std::vector<float> &translation = std::vector<float>(), 
		const std::vector<float> &scale= std::vector<float>(), 
		const std::vector<float> &rotation= std::vector<float>(),
		const TransformSpace relativeTo = TransformSpace::PARENT ) 
	* @brief Set the transformNode identified by nodeId in SGA's node index
	* @param nodeId the identifier of the transform node to set
	* @param translation the translation applied to the transform node, can be empty
	* @param scale the scale applied to the transform node, can be empty
	* @param rotation the rotation applied to the transform node as a quaternion, can be empty
	* @param relativeTo the transform space to use, relative to parent by default
	*/
	SCENEGRAPHADAPTER_API virtual void setTransformNode( 
		const string &nodeId, 
		const std::vector<float> &translation = std::vector<float>(), 
		const std::vector<float> &scale= std::vector<float>(), 
		const std::vector<float> &rotation= std::vector<float>(),
		const TransformSpace relativeTo = TransformSpace::PARENT
		) = 0;

	/*
	* APPEARANCE
	*/

	/**
	* @fn string createMaterial(
		const float ambientIntensity, 
		const float shininess, 
		const float alpha, 
		const std::vector<float> &diffuseColor, 
		const std::vector<float> &emissiveColor, 
		const std::vector<float> &specularColor, 
		const string &materialId )
	* @brief Create a new material identified by materialId
	* @param ambientIntensity the ambient intensity of the material
	* @param shininess the shininess of the material
	* @param alpha the alpha of the material; [0:1] with O means transparent and 1 means opaque
	* @param diffuseColor the diffuse color of the material: ( R,G,B, alpha); alpha can be empty
	* @param emissiveColor the emissive color of the material: ( R,G,B, alpha); alpha can be empty
	* @param specularColor the specular color of the material: ( R,G,B, alpha); alpha can be empty
	* @param materialId the identifier of the material
	* @return the new node identifier in the renderer scene graph
	*/
	SCENEGRAPHADAPTER_API virtual string createMaterial(
		const float ambientIntensity, 
		const float shininess, 
		const float alpha, 
		const std::vector<float> &diffuseColor, 
		const std::vector<float> &emissiveColor, 
		const std::vector<float> &specularColor, 
		const string &materialId )= 0;
	/*virtual string createMaterial(
		float ambientIntensity, 
		float shininess, 
		float alpha, 
		float * diffuseColor, 
		float * emissiveColor, 
		float * specularColor, 
		string materialId )= 0;*/
	
	/**
	* @fn void setMaterial(
		const string &nodeId, 
		const float ambientIntensity, 
		const float shininess, 
		const float alpha, 
		const std::vector<float> diffuseColor = std::vector<float>(), 
		const std::vector<float> emissiveColor = std::vector<float>(), 
		const std::vector<float> specularColor = std::vector<float>() )
	* @brief Set the material identified by materialId in SGA's node index
	* @param nodeId the identifier of the material
	* @param ambientIntensity the ambient intensity of the material, if ambientIntensity=-1 its value is not updated 
	* @param shininess the shininess of the material, if shininess=-1 its value is not updated 
	* @param alpha the alpha of the material, if alpha=-1 its value is not updated  
	* @param diffuseColor the diffuse color of the material, if diffuseColor is empty its value is not updated  
	* @param emissiveColor the emissive color of the material, if emissiveColor is empty its value is not updated  
	* @param specularColor the specular color of the material, if specularColor is empty its value is not updated  
	*/
	SCENEGRAPHADAPTER_API virtual void setMaterial(
		const string &nodeId, 
		const float ambientIntensity, 
		const float shininess, 
		const float alpha, 
		const std::vector<float> diffuseColor = std::vector<float>(), 
		const std::vector<float> emissiveColor = std::vector<float>(), 
		const std::vector<float> specularColor = std::vector<float>() ) = 0;
	
	/**
	* @fn string createTexture(const string &url, const string &textureId )
	* @brief Create a new texture identified by textureId
	* @param url the url of the texture
	* @param textureId the identifier of the new texture
	* @return the new node identifier in the renderer scene graph
	*/
	SCENEGRAPHADAPTER_API virtual string createTexture(const string &url, const string &textureId )= 0;

	/**
	* @fn void setTexture( const string &textureId, const string &url)
	* @brief Set the texture identified by textureId in SGA's node index
	* @param textureId the identifier of the texture to set
	* @param url the new url of the texture
	*/
	SCENEGRAPHADAPTER_API virtual void setTexture( const string &textureId, const string &url)= 0;
	//virtual void setAppearance()= 0;

	/**
	* @fn void getNodeTransform( const string &nodeId, std::vector<float> &translation, std::vector<float> &scale, std::vector<float> &rotation )
	* @brief Get the transform information of the node identify by nodeId
	* @param nodeId the identifier of the node
	* @param translation translation of the node
	* @param scale scale of the node
	* @param rotation rotation of the node
	* @return the node transform info (x,y,z)
	*/
	SCENEGRAPHADAPTER_API virtual void getNodeTransform( const string &nodeId, std::vector<float> &translation, std::vector<float> &scale, std::vector<float> &rotation ) = 0;

	/**
	* @fn void getCurrentTransform( std::vector<float> &translation, std::vector<float> &scale, std::vector<float> &rotation ) = 0
	* @brief Get the transform information of the current node (i.e. the scene graph node that is pointed in the renderer wrapper )
	* @param translation translation of the node
	* @param scale scale of the node
	* @param rotation rotation of the node
	* @return the node transform infos: translation (x,y,z), scale (x,y,z) and rotation (x,y,z,w)
	*/
	SCENEGRAPHADAPTER_API virtual void getCurrentTransform( std::vector<float> &translation, std::vector<float> &scale, std::vector<float> &rotation ) = 0;

};

#endif // #ifndef __RendererAdapter_h__