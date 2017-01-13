/**
* \file PhysicsAdapter.h
* \brief A virtual class that gives methods to adapt every physics engine scene graph
* Details about PhysicsAdapter.h
*/
#ifndef __PhysicsAdapter_h__
#define __PhysicsAdapter_h__

//#include "SceneGraphAdapter.h"
#include <string>
//#include "EngineWrapperFactory.h"

using namespace std;

/**
* The PhysicsAdapter class is a pure virtual class that defines methods to read, access and modify a physics engine world.
* @author Rozenn Bouville Berthelot
* @date 18/05/2011
* @version 0.0
*/

//namespace SceneGraphAdapter {

class PhysicsAdapter {

public:

	
	
	/**
	* @fn PhysicsAdapter()
	* Default constructor
	*/
	SCENEGRAPHADAPTER_API PhysicsAdapter(){};
	
	/**
	* @fn ~PhysicsAdapter()
	* Destructor
	*/
	SCENEGRAPHADAPTER_API virtual ~PhysicsAdapter(){};

	/**
	* @fn string setGround( const string &sNodeId,const std::vector<float> &normal, const float constant );
	* @brief Create a infinite ground identified by sNodeId
	* @param sNodeId the identifier of the ground to create ( ex: "GROUND" )
	* @param normal the normal of the ground
	* @param constant the height of the plane to create
	* @return the new node identifier in the physics scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string setGround( const string &sNodeId,const std::vector<float> &normal, const float constant )= 0;

	/**
	* @fn string createBox( const std::vector<float> &size, const string &shapeId )
	* @brief Create a box shape of the given size, identified by shapeId
	* @param size the size of the box to create
	* @param shapeId the identifier of the box to create
	* @return the new node identifier in the physics scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createBox( const std::vector<float> &size, const string &shapeId )= 0;

	/**
	* @fn string createSphere( const float radius, const string &shapeId )
	* @brief Create a sphere shape of the given radius, identified by shapeId
	* @param radius the radius of the sphere to create
	* @param shapeId the identifier of the sphere to create
	* @return the new node identifier in the physics scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createSphere( const float radius, const string &shapeId )= 0;

	/**
	* @fn string createCylinder(const float radius, const float height, const string &shapeId )
	* @brief Create a cylinder shape of the given radius and height, identified by shapeId
	* @param radius the radius of the cylinder to create
	* @param height the height of the cylinder to create
	* @param shapeId the identifier of the cylinder to create
	* @return the new node identifier in the physics scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createCylinder(const float radius, const float height, const string &shapeId )= 0;

	/**
	* @fn string createMesh( const vector<float> &vertex, const string &shapeId )
	* @brief Create a mesh shape identified by shapeId
	* @param vertex the aray of vertices of the mesh
	* @param shapeId the identifier of the cylinder to create
	* @return the new node identifier in the physics scene graph or an empty string if the creation failed
	*/
	SCENEGRAPHADAPTER_API virtual string createMesh( const vector<float> &vertex, const string &shapeId )= 0;


	/**
	* @fn void setNodeMass( const string nodeId, const float fMass)
	* @brief Set the mass of a given nod
	* @param nodeId the identifier of the node
	* @param fMass mass to set
	* @param translation  the position of the node
	* @param rotation  the rotation of the node
	*/
	SCENEGRAPHADAPTER_API virtual void setNodeMass( const string nodeId, const float fMass, const std::vector<float> &translation,  const std::vector<float> &rotation) = 0;

	/**
	* @fn void setNodeRestitution( const string nodeId, const float fRestitution)
	* @brief Set the restitution ( = bounciness or elasticity ) of a given node
	* @param nodeId the identifier of the node
	* @param fRestitution restitution to set
	*/
	SCENEGRAPHADAPTER_API virtual void setNodeRestitution( const string nodeId, const float fRestitution) = 0;

	/**
	* @fn void setNodeFriction( const string nodeId, const float fFriction)
	* @brief Set the friction of a given node
	* @param nodeId the identifier of the node
	* @param fFriction friction to set
	*/
	SCENEGRAPHADAPTER_API virtual void setNodeFriction( const string nodeId, const float fFriction) = 0;

	/**
	* @fn void setNodePhysicsProperties( const string nodeId, const float fMass, const float fRestitution, const float fFriction)
	* @brief Set the physics properties of a given node
	* @param nodeId the identifier of the node
	* @param translation  the position of the node
	* @param rotation  the rotation of the node
	* @param fMass mass to set  ( if fMass=-1, the mass of the object won't be setted )
	* @param fRestitution restitution to set ( if fRestitution=-1, the restitution of the object won't be setted )
	* @param fFriction friction to set ( if fFriction=-1, the friction of the object won't be setted )
	* @param isDynamic if the node is dynamic ( default value us true )
	*/
	SCENEGRAPHADAPTER_API virtual void setNodePhysicsProperties( 
		const string nodeId, 
		const std::vector<float> &translation,  
		const std::vector<float> &rotation,
		const float fMass = -1, 
		const float fRestitution = -1, 
		const float fFriction = -1, 
		const bool isDynamic = true ) = 0;

	/**
	* @fn void setTransform( const std::vector<float> &translation,  const std::vector<float> &rotation )
	* @brief Set the current transform values ( called each time a new transform node is added or closed in the renderer scene graph )
	* @param translation the current position
	* @param rotation the current rotation as a quaternion
	*/
	SCENEGRAPHADAPTER_API virtual void setTransform( const std::vector<float> &translation,  const std::vector<float> &rotation ) = 0;

	/**
	* @fn void setGravity( const std::vector<float> &vGravity )
	* @brief Set the gravity in the physics world
	* @param vGravity the gravity vector ( the world is Y_up_axis ) 
	*/
	SCENEGRAPHADAPTER_API virtual void setGravity( const std::vector<float> &vGravity ) = 0;

	/**
	* @fn void setTimeStep ( const float fTimeStep )
	* @brief Set the time step for simulation in the physics world
	* @param fTimeStep the time step
	*/
	SCENEGRAPHADAPTER_API virtual void setTimeStep ( const float fTimeStep ) = 0;

	
protected:


private:

	

};

//} //namespace SceneGraphAdapter
#endif // __PhysicsAdapter_h__