#ifndef __BulletWrapper_h__
#define __BulletWrapper_h__

#include "BulletWrapperPrerequisites.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "BulletWrapperMotionState.h"
#include <vector>
#include <string>
#include <sstream> 
#include "SGAKernel.h"


using namespace std;
using namespace SceneGraphAdapter;

class BULLET_WRAPPER BulletWrapper : public EngineWrapperFactory::PhysicsWrapper {

public:

	BulletWrapper(){
		initialize();
	};
	
	~BulletWrapper(){
		exitPhysics();
	}



	WrapperConfiguration * getWrapperConfiguration(){
		return m_configuration;
	}

	void configureEngine(SGAKernel * kernel);
	void run( float time );
	void setTransform( const std::vector<float> &translation,  const std::vector<float> &rotation );
	string setGround( const string &sNodeId,const std::vector<float> &normal, const float constant );
	string createBox( const vector<float> &size, const string &shapeId);
	string createSphere( const float radius, const string &shapeId);
	string createCylinder(const float radius, const float height, const string &shapeId);
	string createMesh( const vector<float> &vertex, const string &shapeId );
	void setNodeMass( const string nodeId, const float fMass, const std::vector<float> &translation,  const std::vector<float> &rotation);
	void setNodeRestitution( const string nodeId, const float fRestitution){};
	void setNodeFriction( const string nodeId, const float fFriction){};
	void setNodePhysicsProperties( const string nodeId, const std::vector<float> &translation,  const std::vector<float> &rotation,const float fMass = -1, const float fRestitution = -1, const float fFriction = -1, const bool isDynamic = true );
	void setGravity( const std::vector<float> &vGravity );
	void setTimeStep ( const float fTimeStep );

	void getCollisionShapeMotionState( const string nodeId, BulletWrapperMotionState* &mMotionState, btRigidBody* &body );

private:

	void initialize(){
		_kernel=NULL;
		_curTransform.setIdentity();
		m_configuration->setIdentifier(ENGINE_NAME);
		m_configuration->setVersion(WRAPPER_VERSION);
		m_configuration->setType(WRAPPER_TYPE);
		cptCollisionShape=0;
	}
	void initPhysics();
	void exitPhysics();

	btDefaultCollisionConfiguration*		_collisionConfiguration ;
	btCollisionDispatcher*					_dispatcher;
	btBroadphaseInterface*					_overlappingPairCache;
	btSequentialImpulseConstraintSolver*	_solver;
	btScalar								_defaultContactProcessingThreshold;
	btScalar                                _timeStep;

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> _collisionShapes;
	btDiscreteDynamicsWorld* _dynamicsWorld;
	btDefaultMotionState* _groundMotionState;
	SGAKernel * _kernel;
	
	btTransform _curTransform;
	int cptCollisionShape;


	//typedef std::map< string, btCollisionShape* > ShapeMap; 
	typedef std::map< btCollisionShape*, std::string > ShapeMap; 
	ShapeMap m_ShapeMap;
};

#endif // #ifndef __BulletWrapper_h__