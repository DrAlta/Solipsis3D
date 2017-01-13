#ifndef __OgreWrapper_h__
#define __OgreWrapper_h__

#include "OgreWrapperPrerequisites.h"
#include <Ogre.h>
#include <vector>
#include <string>
//#include "RendererAdapter.h"
//#include "NodeIndexer.h"
#include "SGAKernel.h"

#define NB_VERTICES_LIMIT 65526
#define GROUND_WIDTH 1500
#define GROUND_HEIGHT 1500

using namespace Ogre;
using namespace std;
using namespace SceneGraphAdapter;

class OGRE_WRAPPER OgreWrapper : public EngineWrapperFactory::RendererWrapper {

public:

	OgreWrapper(){
		init();
	}
	OgreWrapper(Ogre::Root * mRoot, Ogre::SceneManager* scnMgr);
	
	~OgreWrapper();

	void setOgreApplication(Root * oRoot=NULL, SceneManager* scnMgr=NULL){
		this->cRoot = oRoot;
		this->cSceneMgr = scnMgr;
		if(cSceneMgr) this->curSceneNode = cSceneMgr->getRootSceneNode();
		initPrimitive();
	}

	/* RENDERER_WRAPPER */

	//Scene
	string createViewpoint( const std::vector<float> &position, const std::vector<float> &orientation, const string &viewpointId );
	string createCamera ( const std::vector<float> &position, const std::vector<float> &orientation,  const string &viewpointId ){ return ""; };
	string createViewport ( const float left, const float top, const float width, const float height )  { return ""; };


	//Scenegraph
	void closeNode (const string &nodeId );
	std::vector<string> deleteNode (const string &nodeId );
	std::vector<string> deleteNodeChildren (const string &nodeId );
	//void setNodeVisibility ( const string &nodeId, const bool isVisible );
	void showNode (const string &nodeId );
	void hideNode (const string &nodeId );
	void attachShapeToSceneGraph( const string &shapeId, const bool show = true, const bool cascade = false );


	//Geometry
	string createPlane( const float width, const float height, const std::vector<float> &orientation, const string &shapeId );
	string createGround( const string &sNodeId,const std::vector<float> &normal, const float constant, const std::vector<float> &color = std::vector<float>(3,0.5), const std::string urlTexture = std::string() );
	string createCylinder(const float radius, const float height, const string &shapeId );
	string createSphere(const float radius, const string &shapeId );
	string createBox( const std::vector<float> &size, const string &shapeId );
	string createMesh( const std::vector<float> &vertices,const std::vector<float> &normals, const std::vector<float> &textureCoord, const std::vector<float> &colours, const std::vector<unsigned short> &indices, const string &shapeId );
	string createGroupNode( const string &nodeId );
	string createTransformNode( const std::vector<float> &translation, const std::vector<float> &scale, const std::vector<float> &rotation, const string &transformId );
	void setTransformNode( const string &nodeId, const std::vector<float> &translation = std::vector<float>(), const std::vector<float> &scale= std::vector<float>(), const std::vector<float> &rotation= std::vector<float>(),const TransformSpace relativeTo = TransformSpace::PARENT );

	//Material
	//string createMaterial(float ambientIntensity, float shininess, float alpha, float * diffuseColor, float * emissiveColor, float * specularColor, string materialId);
	string createMaterial(const float ambientIntensity, const float shininess, const float alpha, const std::vector<float> &diffuseColor, const std::vector<float> &emissiveColor, const std::vector<float> &specularColor, const string &materialId );
	//void setMaterial(string nodeId, float ambientIntensity, float shininess, float alpha, float * diffuseColor, float * emissiveColor, float * specularColor );
	void setMaterial(const string &nodeId, const float ambientIntensity, const float shininess, const float alpha, const std::vector<float> diffuseColor = std::vector<float>(), const std::vector<float> emissiveColor = std::vector<float>(), const std::vector<float> specularColor = std::vector<float>() );
	

	string createTexture(const string &url, const string &textureId );
	void setTexture( const string &textureId, const string &url){};
	//void setAppearance();


	WrapperConfiguration * getWrapperConfiguration(){
		return m_configuration;
	}

	void getNodeTransform( const string &nodeId, std::vector<float> &translation, std::vector<float> &scale, std::vector<float> &rotation );
	void getCurrentTransform( std::vector<float> &translation, std::vector<float> &scale, std::vector<float> &rotation );

	/* ENGINE_WRAPPER */

	void configureEngine(SGAKernel * kernel){
		_kernel = kernel;
		setOgreApplication();
	}
	void run( float time ){};

	

private:
	//std::vector<float> formatVerticesArrayForOgre(const std::vector<float> & verticesCoord, const std::vector<float> & normalsCoord, const std::vector<float> &textureCoord);
	std::vector<float>  getBoundingBox( const std::vector <float> &vertices);
	void destroyAllAttachedMovableObjects( SceneNode* i_pSceneNode );
	string createSceneNode(const string nodeId);

	void clearPrivateDatas();
	void init();
	void initPrimitive(){
		// Init generic primitives to clone when the creation will be called
	/*mGenericPlane = this->cSceneMgr->createEntity( "GenericPlane", "Plane.mesh" );
	mGenericBox = this->cSceneMgr->createEntity( "GenericBox", "Box.mesh" );
	mGenericPrism = this->cSceneMgr->createEntity( "GenericPrism", "Prism.mesh" );*/
	mGenericCylinder = this->cSceneMgr->createEntity( "GenericCylinder", "Cylinder.mesh" );
	mGenericSphere = this->cSceneMgr->createEntity( "GenericSphere", "Sphere.mesh" );
	/*mGenericTorus = this->cSceneMgr->createEntity( "GenericTorus", "Torus.mesh" );
	mGenericTube = this->cSceneMgr->createEntity( "GenericTube", "Tube.mesh" );
	mGenericRing = this->cSceneMgr->createEntity( "GenericRing", "Ring.mesh" );*/
	}

	Ogre::Root *cRoot;
	Ogre::SceneManager* cSceneMgr;
	Ogre::SceneNode* curSceneNode;
	Ogre::Entity* curEntity;
	Ogre::String curMaterialName;
	Ogre::ManualObject* curManualObject;
	bool meshIsGeneric;
	SGAKernel * _kernel;

	/// Primitive entities
	/*Entity				*mGenericPlane;
	Entity				*mGenericBox;
	Entity				*mGenericPrism;*/
	Entity				*mGenericCylinder;
	Entity				*mGenericSphere;
	/*Entity				*mGenericTorus;
	Entity				*mGenericTube;
	Entity				*mGenericRing;*/

	std::vector<string> _deletedNodesArray;

};

#endif // #ifndef __OgreWrapper_h__