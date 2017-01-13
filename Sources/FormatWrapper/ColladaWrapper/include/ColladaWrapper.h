#ifndef __ColladaWrapper_h__
#define __ColladaWrapper_h__

#include "ColladaWrapperPrerequisites.h"

#include "SGAKernel.h"
#include "ColladaWrapperTools.h"
//#include "RendererAdapter.h"
//#include "FormatAdapter.h"
#include <iostream>
#include <sstream>
#include <vector>
#include "Shlwapi.h"


#include "FCollada.h"
#include "FColladaPlugin.h"
#include "FCDocument\FCDocument.h"
#include "FCDocument\FCDocumentTools.h"
#include "FCDocument\FCDLibrary.h"
#include "FCDocument\FCDGeometryMesh.h"
#include "FCDocument\FCDGeometry.h"
#include "FCDocument\FCDGeometryPolygons.h"
#include "FCDocument\FCDGeometryInstance.h"
#include "FCDocument\FCDGeometrySource.h"
#include "FCDocument\FCDGeometryPolygonsInput.h"
#include "FCDocument\FCDGeometryPolygonsTools.h"
#include "FCDocument\FCDMaterial.h"
#include "FCDocument\FCDMaterialInstance.h"
#include "FCDocument\FCDEffect.h"
#include "FCDocument\FCDEffectProfile.h"
#include "FCDocument\FCDEffectStandard.h"
#include "FCDocument\FCDImage.h"
#include "FCDocument\FCDTexture.h"
#include "FCDocument\FCDSceneNode.h" 
#include "FCDocument\FCDTransform.h" 
#include "FCDocument\FCDLight.h"
#include "FCDocument\FCDExtra.h"
#include "FUtils\FUObject.h" 
#include "FCDocument\FCDAsset.h"
#include "FCDocument\FCDPhysicsMaterial.h" 
#include "FCDocument\FCDPhysicsModel.h" 
#include "FCDocument\FCDPhysicsModelInstance.h" 
#include "FCDocument\FCDPhysicsScene.h" 
#include "FCDocument\FCDPhysicsShape.h" 
#include "FCDocument\FCDPhysicsRigidBody.h"
#include "FCDocument\FCDPhysicsRigidBodyInstance.h"
#include "FCDocument\FCDPhysicsRigidBodyParameters.h"


#include "dae.h"
#include "dom/domCOLLADA.h"
#include "dae/domAny.h"
#include "dom/domConstants.h"

using namespace SceneGraphAdapter;
using namespace FCollada;
using namespace std;


class COLLADA_WRAPPER_API ColladaWrapper : public FormatWrapperFactory::FormatWrapper {

private:

	//RendererAdapter * rendererAdapter;
	SGAKernel * _kernel;
	string _colladaFileName;
	string _colladaFileUrl;
	string _colladaFilePath;
	vector<float> _vertices;
	vector<unsigned short> _faces;
	vector<unsigned short> _textCoordIdx;
	vector<unsigned short> _normalIdx;
	vector<float> _normals;
	vector<float> _textureCoord;  /* TODO */
	vector<float> _colours; /* TODO */
	int _nodeIdx;
	FMVector3 _upAxis;

	//Material values
	float _ambientIntensity;
	float _shininess;
	//float _transparency;
	float _alpha;

	vector<float>  _diffuseColor;
	vector<float>  _emissiveColor;
	vector<float>  _specularColor;

	FCDocument* _doc;		//FCollada root
	daeElement* _domRoot;	//ColladaDOM root
	DAE _dae; //ColladaDOM dae object

	string getNodeName(string name, int type);

	FCDGeometry * findGeometryLibrary(string name,FCDocument* doc );
	FCDMaterial * findMaterialLibrary(string name,FCDocument* doc );

	/**
	* @fn void setInstanceTransform(FCDSceneNode* sceneNode,const vector<float> translation, const std::vector<float> rotation)
	* @brief set a new translation and a new rotation to a scene node and inform the SGA kernel of the update
	* @param sceneNode the scene node to update
	* @param translation the new translation
	* @param rotation the new rotation
	*/
	void setInstanceTransform(FCDSceneNode* sceneNode, const vector<float> translation, const std::vector<float> rotation);


	/**
	* @fn FCDSceneNode * findInstanceParentSceneNode( const string &nodeId )
	* @brief return the parent scene node of an instance identified by the DAE id nodeId
	* @param nodeId the dae id of the instance
	* @return the parent scene node
	*/
	FCDSceneNode * findInstanceParentSceneNode( const string &nodeId );

	/**
	* @fn FCDTransform * getDerivedTrans ( FCDSceneNode* sceneNode, int iTrans  )
	* @brief return the derived transform of a given scene node
	* @param sceneNode the dae id of the instance
	* @param iTrans the transform index
	* @return the derived transform of the scene node
	*/

	/**
	* @fn void getTransformValues(FCDSceneNode* node, vector<float> vTranslation, vector<float> vScale, vector<float> vRotation)
	* @brief return the transform values of a given scene node
	* @param sceneNode the dae id of the scene node
	* @param vTranslation the translation vector of the transform
	* @param vScale the scale vector of the transform
	* @param vRotation the rotation vector of the transform (as quaternion)
	* @return the derived transform of the scene node
	*/
	void getTransformValues(FCDSceneNode* node, vector<float> &vTranslation, vector<float> &vScale, vector<float> &vRotation);

	FCDTransform * getDerivedTrans ( FCDSceneNode* sceneNode, int iTrans );

	void processPhysicsScene ( FCDPhysicsSceneLibrary * physicsScene );

	void processPhysicsModelInstance ( FCDPhysicsModelInstance * pMInstance );

	void processPhysicsSceneProperties( FCDPhysicsScene * pScene );

	void processMaterialInstance( FCDEntityInstance* instance,FCDocument* doc );

	void processGeometryInstance(FCDEntityInstance* instance, FCDSceneNode * parentNode );

	void processInstance(FCDSceneNode* node, FCDocument* doc );

	string processTransform(FCDSceneNode* node, FCDocument* doc);

	void processVisualSceneNode ( FCDSceneNode * sNode );

	void processExternalUris();

	void processDoc(FCDocument* doc);

	void clearDatas(){
		_vertices.clear();
		_faces.clear();
		_normals.clear();
		_textureCoord.clear();
		_colours.clear();
	}

	void initialize(){
		_nodeIdx = 0;
		clearDatas();
		initWrapperVariables();
	}

	void initWrapperVariables(){
		//rendererAdapter = NULL;
		_colladaFileName = "";
		_vertices = vector<float>();
		_faces = vector<unsigned short>();
		_normals = vector<float>();
		_textureCoord = vector<float>();
		_colours = vector<float>();
	}

	void setDefaultMaterial(){
	
	_ambientIntensity = 0.0;
	_shininess=10.0;
	_alpha = 1;
	
	_diffuseColor.clear();
	_emissiveColor.clear();
	_specularColor.clear();

	_diffuseColor.insert(_diffuseColor.begin(),3,0.8);
	_diffuseColor.push_back(0);
	_emissiveColor.insert(_emissiveColor.begin(),4,0.0);
	_specularColor.insert(_specularColor.begin(),3,0.2);
	_specularColor.push_back(0);
	//_diffuseColor[0] =  _diffuseColor[2] = 1.0;
	//_diffuseColor[1] = 0.0;
	//_emissiveColor[0] = _emissiveColor[1] = _emissiveColor[2] = 0;
	//_specularColor[0] = _specularColor[1] = _specularColor[2] = 0.2;

}

public:

	ColladaWrapper(){
		initialize();
		//setWrapperConfiguration(new WrapperConfiguration(WRAPPER_TYPE,"",INPUT_FILE_EXTENSION,WRAPPER_VERSION));
		m_configuration->setExtension(INPUT_FILE_EXTENSION);
		m_configuration->setVersion(WRAPPER_VERSION);
	}

	ColladaWrapper(string sFileName){
		_colladaFileUrl = sFileName;
		initialize();
		m_configuration->setExtension(INPUT_FILE_EXTENSION);
		m_configuration->setIdentifier(sFileName);
		m_configuration->setVersion(WRAPPER_VERSION);
		//setWrapperConfiguration(new WrapperConfiguration(WRAPPER_TYPE,sFileName,INPUT_FILE_EXTENSION,WRAPPER_VERSION));
	}

	~ColladaWrapper(){}

	int adaptFormat( const string & fileName, const string & filePath, SGAKernel * kernel);
	//int adaptSceneGraph( EngineWrapperFactory::EngineWrapper * rWrapper, int nodeIndex = -1 );
	int adaptSceneGraph( SGAKernel * sgaKernel, const string &nodeId = string() ) { return 1;}
	//SCENEGRAPHADAPTER_API virtual int adaptSceneGraph( SGAKernel * sgaKernel, string nodeId = string() ) = 0 ;
	
	void updateElapsedTime( const float elapsedTime ){};
	void onPress ( const string &nodeId, const float time, const float x, const float y ){};
	void onRelease ( const string &nodeId, const float time, const float x, const float y ){};
	void onEnter ( const string &nodeId, const float time, const float x, const float y ){};
	void onExit ( const string &nodeId, const float time, const float x, const float y){};
	void onDrag ( const string &nodeId, const float time, const float x, const float y ){};
	void updateNodePosition( const string &nodeId, const vector<float> position, const std::vector<float> rotation );
	std::string getNodeFieldValue ( const std::string &nodeId,  const string &fieldId );
	void setNodeFieldValue( const std::string &nodeId, const string &fieldId, const string &sValue );
	void matchNodeFieldValue( const std::string &fromNodeId, const std::string &toNodeId, const std::string &toNodeFileName, const std::vector<std::string> fields );
	void processMatch( FCDSceneNode * sceneNode, const std::string &toNodeId, const std::string &toNodeFileName, const std::string &field);

	void setWrapperConfiguration( WrapperConfiguration * wConfig ){
		m_configuration = wConfig;
	}

	WrapperConfiguration * getWrapperConfiguration(){
		return m_configuration;
	}

};

#endif // #ifndef __ColladaWrapper_h__