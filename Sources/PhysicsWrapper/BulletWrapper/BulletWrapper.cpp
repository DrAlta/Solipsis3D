#include "BulletWrapper.h"

void BulletWrapper::configureEngine(SGAKernel * kernel){

	_kernel = kernel;
	initPhysics();

}

void BulletWrapper::initPhysics(){

	/////collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	_collisionConfiguration = new btDefaultCollisionConfiguration();

	/////use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	_dispatcher = new	btCollisionDispatcher(_collisionConfiguration);

	/////btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	_overlappingPairCache = new btDbvtBroadphase();

	/////the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	_solver = new btSequentialImpulseConstraintSolver;

	_dynamicsWorld = new btDiscreteDynamicsWorld(_dispatcher,_overlappingPairCache,_solver,_collisionConfiguration);

	_dynamicsWorld->setGravity(btVector3(0,-10,0));

	_defaultContactProcessingThreshold = btScalar(1e18f);

}

void BulletWrapper::exitPhysics(){

	//remove the rigidbodies from the dynamics world and delete them
	for (int i=_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--){
		btCollisionObject* obj = _dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()){
			delete body->getMotionState();
		}
		_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}	

	//delete collision shapes
	for (int j=0;j<_collisionShapes.size();j++){
		btCollisionShape* shape = _collisionShapes[j];
		_collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete _dynamicsWorld;

	//delete solver
	delete _solver;

	//delete broadphase
	delete _overlappingPairCache;

	//delete dispatcher
	delete _dispatcher;

	delete _collisionConfiguration;

	_collisionShapes.clear();
}

string BulletWrapper::setGround( const string &sNodeId, const std::vector<float> &normal, const float constant ){

	string res = string();

	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(normal.at(0),normal.at(1),normal.at(2)),constant);

	_collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,0,0));

	btScalar mass(0.);
	btVector3 localInertia(0,0,0);

	//_groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0))); ///non !!!
	_groundMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(mass,_groundMotionState,groundShape,localInertia);
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

	_dynamicsWorld->addRigidBody(groundRigidBody);

	return res;
}

string BulletWrapper::createBox( const vector<float> &size, const string &shapeId ){

	string res = string();

	if(!size.empty()){

		btBoxShape * bBox = new btBoxShape(btVector3(btScalar(size.at(0)),btScalar(size.at(1)),btScalar(size.at(2))));
		//int shapeIdx = _collisionShapes.size();
		_collisionShapes.push_back(bBox);
		//localCreateRigidBody(0.f,_curTransform,bBox);
		m_ShapeMap.insert( ShapeMap::value_type(bBox,shapeId) );
		res = shapeId;
	}
	return res;
}

string BulletWrapper::createMesh( const vector<float> &vertex, const string &shapeId ){

	string res = string();

	if(!vertex.empty()){

		int numVertices = vertex.size()/3;
		int vSize = vertex.size();
		int i;

		btConvexHullShape* convexShape = new btConvexHullShape();
		for (i=0;i<vSize-2;i=i+3){
			convexShape->addPoint( btVector3( btScalar(vertex.at(i)),btScalar(vertex.at(i+1)),btScalar(vertex.at(i+2)) ) );	
		}

		//if(numVertices>100){
		//	//reduce the number of vertices below 100
		//	btShapeHull* hull = new btShapeHull(convexShape);
		//	btScalar margin = convexShape->getMargin();
		//	hull->buildHull(margin);
		//	//btConvexHullShape* simplifiedConvexShape = new btConvexHullShape(hull->getVertexPointer(),hull->numVertices());
		//	convexShape->setUserPointer(hull);
		//}

		//int shapeIdx = _collisionShapes.size();
		_collisionShapes.push_back(convexShape);
		m_ShapeMap.insert( ShapeMap::value_type(convexShape,shapeId) );
		res = shapeId;
	}
	return res;

}

void BulletWrapper::setNodeMass( const string nodeId, const float fMass, const std::vector<float> &translation,  const std::vector<float> &rotation){

	if (_dynamicsWorld){

		string iNodeId = std::string();
		btCollisionShape * cShape = NULL;
		ShapeMap::iterator it;

		for (it=m_ShapeMap.begin(); it!=m_ShapeMap.end(); ++it){
			iNodeId = it->second;
			if( iNodeId.compare(nodeId)==0 ){
				cShape = it->first;
				break;
			}
		}
		if(cShape){
			btScalar mass(fMass/1.f);
			btVector3 localInertia(0,0,0);

			bool isDynamic = (mass != 0.f);
			if (isDynamic) cShape->calculateLocalInertia(mass,localInertia);
			setTransform(translation,rotation);
			BulletWrapperMotionState * mMotionState = new BulletWrapperMotionState(_curTransform,nodeId,_kernel);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,mMotionState,cShape,localInertia);
			btRigidBody* body = new btRigidBody(rbInfo);
			_dynamicsWorld->addRigidBody(body);
		}
	}
}

void BulletWrapper::setNodePhysicsProperties( const string nodeId , const std::vector<float> &translation,  const std::vector<float> &rotation, const float fMass, const float fRestitution, const float fFriction, const bool isDynamic){

	if (_dynamicsWorld){

		string iNodeId = std::string();
		btCollisionShape * cShape = NULL;
		ShapeMap::iterator it;

		for (it=m_ShapeMap.begin(); it!=m_ShapeMap.end(); ++it){
			iNodeId = it->second;
			if( iNodeId.compare(nodeId)==0 ){
				cShape = it->first;
				break;
			}
		}
		if(cShape){


			BulletWrapperMotionState * mMotionState = NULL;
			btRigidBody * body  = NULL;

			float tMass;
			if( isDynamic && fMass!=-1 ) tMass = fMass;
			else if( isDynamic && fMass==-1 ) tMass = 1.0; /// !!!!!!!
			else tMass = 0.f;
			btScalar mass(tMass/1.f);
			btVector3 localInertia(0,0,0);

			//bool isDynamic = (mass != 0.f);
			if (isDynamic) cShape->calculateLocalInertia(mass,localInertia);

			setTransform(translation,rotation);

			//BulletWrapperMotionState * mMotionState = new BulletWrapperMotionState(_curTransform,nodeId,_kernel);
			getCollisionShapeMotionState( nodeId, mMotionState, body );
			if( mMotionState ){
				if( fRestitution !=-1 ) body->setRestitution(fRestitution);
				if( fFriction !=-1 ) body->setFriction(fFriction);
				if( fMass != -1 || !isDynamic ) body->setMassProps( mass,localInertia );
			}
			else{
				mMotionState = new BulletWrapperMotionState(_curTransform,nodeId,_kernel);
				btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,mMotionState,cShape,localInertia);
				body = new btRigidBody(rbInfo);
				body->setRestitution(fRestitution);
				body->setFriction(fFriction);
				//body->setContactProcessingThreshold(_defaultContactProcessingThreshold);
				_dynamicsWorld->addRigidBody(body);
			}
		}
	}
}

void BulletWrapper::getCollisionShapeMotionState( const string nodeId, BulletWrapperMotionState* &mMotionState, btRigidBody* &body ){

	std::string curName;
	btRigidBody*  curBody;
	BulletWrapperMotionState * bwMState;
	int nbCollObjet = _dynamicsWorld->getNumCollisionObjects();
	for (int i=_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--){
		btCollisionObject * obj = _dynamicsWorld->getCollisionObjectArray()[i];
		curBody = btRigidBody::upcast(obj);
		//char * s = typeid(curBody->getMotionState()).name();
		if( typeid(curBody->getMotionState()).name() == typeid(bwMState).name() ){
			curName = dynamic_cast<BulletWrapperMotionState *>(curBody->getMotionState())->getNodeName();
			if( curName.compare(nodeId) == 0 ){
				mMotionState = (dynamic_cast<BulletWrapperMotionState *>(curBody->getMotionState()));
				body = curBody;
				break;
			}
		}
	}
}


string BulletWrapper::createSphere( const float radius, const string &shapeId ){

	string res = string();

	btSphereShape * bSphere = new btSphereShape( btScalar(radius) );
	_collisionShapes.push_back(bSphere);
	m_ShapeMap.insert( ShapeMap::value_type(bSphere,shapeId) );
	res = shapeId;

	return res;
}

string BulletWrapper::createCylinder(const float radius, const float height, const string &shapeId ){

	string res = string();

	btCylinderShape * bCylinder = new btCylinderShape( btVector3( btScalar(radius),btScalar(height),btScalar(radius) ) );
	_collisionShapes.push_back(bCylinder);
	m_ShapeMap.insert( ShapeMap::value_type(bCylinder,shapeId) );

	res = shapeId;
	return res;
}

void BulletWrapper::setTransform( const std::vector<float> &translation,  const std::vector<float> &rotation ){

	_curTransform.setIdentity();
	if(translation.size()>=3)_curTransform.setOrigin( btVector3(btScalar(translation.at(0)),btScalar(translation.at(1)),btScalar(translation.at(2))) );
	if(rotation.size()>=4){
		btQuaternion rot( btScalar(rotation.at(0)),btScalar(rotation.at(1)),btScalar(rotation.at(2)),btScalar(rotation.at(3)));
		_curTransform.setRotation(rot);
	}
}


void BulletWrapper::run( float time ){

	if (_dynamicsWorld)	_dynamicsWorld->stepSimulation( (btScalar)time/ 1000.f,1,_timeStep );
}


void BulletWrapper::setGravity( const std::vector<float> &vGravity ){
	
	if(!vGravity.empty() && vGravity.size()>=3 ){
		_dynamicsWorld->setGravity( btVector3(vGravity.at(0),vGravity.at(1),vGravity.at(2)) );
	}
}

void BulletWrapper::setTimeStep ( const float fTimeStep ){
	_timeStep=fTimeStep;
}


extern "C" BULLET_WRAPPER WrapperConfiguration *  queryPlugin(SGAKernel &wf){
	BulletWrapper * bWrapper = new BulletWrapper();
	return bWrapper->getWrapperConfiguration();
}

extern "C" BULLET_WRAPPER size_t registerPlugin(SGAKernel &wf) {
	wf.getEngineWrapper().addWrapper(
		auto_ptr<EngineWrapperFactory::EngineWrapper> ( new BulletWrapper() )
		);
	return wf.getEngineWrapper().getWrapperCount()-1;
}