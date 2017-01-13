#ifndef __BulletWrapperMotionState_h__
#define __BulletWrapperMotionState_h__

#include "btBulletDynamicsCommon.h"
#include "BulletWrapperPrerequisites.h"
#include "SGAKernel.h"

using namespace SceneGraphAdapter;

class BULLET_WRAPPER BulletWrapperMotionState : public btMotionState {

public:

	BulletWrapperMotionState( const btTransform &initialpos, const std::string nodeId, SGAKernel *kernel ) {
        _nodeId = nodeId;
        _nodePosition = initialpos;
		_kernel = kernel;
    }

    virtual ~BulletWrapperMotionState() {
    }

    void setNode( std::string nodeId ) {
       _nodeId = nodeId;
    }

	std::string getNodeName ( ){
		return _nodeId;
	}

    virtual void getWorldTransform(btTransform &worldTrans) const {
        worldTrans = _nodePosition;
    }

    virtual void setWorldTransform(const btTransform &worldTrans) {
        
		if(_nodeId.empty())
            return; 
        btQuaternion rot = worldTrans.getRotation();
        btVector3 pos = worldTrans.getOrigin();
        
		vector<float> translation = vector<float>();
		translation.push_back(pos.x());
		translation.push_back(pos.y());
		translation.push_back(pos.z());

		vector<float> rotation = vector<float>();
		rotation.push_back(rot.x());
		rotation.push_back(rot.y());
		rotation.push_back(rot.z());
		rotation.push_back(rot.w());

		_kernel->updateNodePosition(_nodeId,ENGINE_NAME,translation,rotation);
    }

protected:
	std::string _nodeId;
    btTransform _nodePosition;
	SGAKernel * _kernel;
};

#endif // #ifndef __BulletWrapperMotionState_h__