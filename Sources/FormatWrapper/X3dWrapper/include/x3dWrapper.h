#ifndef __x3dWrapper_h__
#define __x3dWrapper_h__

#include "x3dWrapperPrerequisites.h"
#include <string>
#include <vector>
#include "CyberX3D.h"
//#include "RendererAdapter.h"
#include "SGAKernel.h"
//#include "SceneGraphAdapter.h"
#include "RouteManager.h"
#include "X3DWrapperTools.h"
#include "Shlwapi.h"


using namespace std;
using namespace CyberX3D;
using namespace SceneGraphAdapter;


class X3D_WRAPPER_API X3dWrapper : public FormatWrapperFactory::FormatWrapper {

public:

	X3dWrapper(){
		initialize();
		m_configuration->setExtension(INPUT_FILE_EXTENSION);
		m_configuration->setVersion(WRAPPER_VERSION);
		//m_configuration = new WrapperConfiguration(WRAPPER_TYPE,"",INPUT_FILE_EXTENSION,WRAPPER_VERSION);
	}

	X3dWrapper( const string &sFileName ){
		initialize();
		x3dFileUrl = sFileName;
		m_configuration->setExtension(INPUT_FILE_EXTENSION);
		m_configuration->setIdentifier(sFileName);
		m_configuration->setVersion(WRAPPER_VERSION);
	}

	~X3dWrapper(){}

	int adaptFormat( const string & fileName, const string & filePath, SGAKernel * sgaKernel);
	int adaptSceneGraph( SGAKernel * sgaKernel, const string &nodeId = string() ){ return 1;}
	void updateElapsedTime( const float elapsedTime );
	void onPress ( const string &nodeId, const float time, const float x, const float y );
	void onRelease ( const string &nodeId, const float time, const float x, const float y );
	void onEnter ( const string &nodeId, const float time, const float x, const float y );
	void onExit ( const string &nodeId, const float time, const float x, const float y);
	void onDrag ( const string &nodeId, const float time, const float x, const float y ){};
	void updateNodePosition( const string &nodeId, const vector<float> position, const std::vector<float> rotation );
	std::string getNodeFieldValue ( const std::string &nodeId,  const string &fieldId );
	void setNodeFieldValue( const std::string &nodeId, const string &fieldId, const string &sValue );
	void matchNodeFieldValue( const std::string &fromNodeId, const std::string &toNodeId, const std::string &toNodeFileName, const std::vector<std::string> fields ){};
	

	static string getInputFormatExtension(){
		return INPUT_FILE_EXTENSION;
	}


	WrapperConfiguration * getWrapperConfiguration(){
		return m_configuration;
	}
	//void updateNodeCallback( float f );

	/*void updateSFFloatNode(float fValue, string nodeName, string fieldName);
	void updateSFVec3fNode(float x,float y, float z, string nodeName, string fieldName);
	
	void print_args(float x, float y);*/


private:

	//string getUniqueIdent(CyberX3D::Node * node);
	string getNodeName(CyberX3D::Node * node);
	CyberX3D::Node * processInstanceNode( CyberX3D::Node * node );

	void initialize(){
		_nodeIdx = 0;
		clean();
	}

	/**
	* @fn clean()
	* @brief clean X3DWrapper object
	*/
	void clean(){
		initMaterialDefaultValues();
		initTransformDefaultValues();
		initIFSValues();
		_size.clear();
	}
	void initIFSValues(){
		_nbVertices = _nbFaces = 0;
		_verticesArray.clear();
		_coloursArray.clear();
		_facesArray.clear();
		_normalsArray.clear();
		_textureCoordArray.clear();
		_coloursIdxArray.clear();
	}

	void initMaterialDefaultValues(){
		
		_ambientIntensity = 0.2;
		_shininess=0.2;
		_alpha = 1;

		_diffuseColor.insert(_diffuseColor.begin(),3,0.8);
		_emissiveColor.insert(_emissiveColor.begin(),3,0.0);
		_specularColor.insert(_specularColor.begin(),3,0.0);
		/*_diffuseColor[0] = _diffuseColor[1] = _diffuseColor[2] = 0.8;
		_emissiveColor[0] = _emissiveColor[1] = _emissiveColor[2] = 0;
		_specularColor[0] = _specularColor[1] = _specularColor[2] = 0;*/
	}
	void initTransformDefaultValues(){
		_translation[0] = _translation[1] = _translation[2] = 0;
		_scale[0] = _scale[1] = _scale[2] = 1;
		_rotation[0] = _rotation[1] = _rotation[3] = 0;
		_rotation[2] = 1;
	}

	void getMaterial(const CyberX3D::SceneGraph *sg,CyberX3D::MaterialNode * matNode);
	void getMaterialData(const CyberX3D::Node * node);
	void getTexture(const CyberX3D::SceneGraph *sg,CyberX3D::TextureNode * texNode);

	void getBoxData(const CyberX3D::SceneGraph *sg,CyberX3D::BoxNode * bNode);
	void getIFSData(const CyberX3D::SceneGraph *sg,CyberX3D::IndexedFaceSetNode * IFSNode);
	void getSphereData(const CyberX3D::SceneGraph *sg,CyberX3D::SphereNode * sNode);
	void getCylinderData(const CyberX3D::SceneGraph *sg,CyberX3D::CylinderNode * sNode);
	void renderShape(CyberX3D::GeometryNode * gnode, CyberX3D::AppearanceNode * apNode);

	void processSceneGraph(const CyberX3D::SceneGraph* sceneGraph, CyberX3D::Node* firstNode, CyberX3D::Node* lastNode=NULL );
	void processRoutes(CyberX3D::SceneGraph* sceneGraph);
	void processSensors( CyberX3D::SceneGraph * sceneGraph, CyberX3D::Node* firstNode);
	void processInlineFiles();
	void computeDefaultTextureCoordinates(const CyberX3D::SceneGraph *sg,  CyberX3D::IndexedFaceSetNode * IFSNode);

	void processViewpoint(const CyberX3D::SceneGraph *sg, CyberX3D::ViewpointNode * viewpoint);
	void processBackgroundNode(const CyberX3D::SceneGraph *sg, CyberX3D::BackgroundNode * bgNode);
	void processShapeNode(const CyberX3D::SceneGraph *sg, CyberX3D::ShapeNode *shape);

	string processGroupNode (const CyberX3D::SceneGraph *sg, CyberX3D::GroupNode * group);
	string processTransformNode (const CyberX3D::SceneGraph *sg, CyberX3D::TransformNode * transform);
	void getTransformData(const CyberX3D::Node *node, vector<float> &translation, vector<float> &scale, vector<float> &rotation);

	void animRoutes(CyberX3D::Node * updatedNode);
	void updateNodeRendering(CyberX3D::Node * node);
	
	TransformNode * getDerivedTransform ( TransformNode * tNode );

protected:

	//IFS values
	int _nbVertices; //nb de sommets du shape courant
	int _nbFaces; //nb de faces du shape courant
	vector<float> _verticesArray;
	vector<float> _coloursArray;
	vector<float> _normalsArray;
	vector<float> _textureCoordArray;
	vector<unsigned short> _facesArray;
	vector<unsigned short> _coloursIdxArray;

	 std::string x3dFileName; // name of the X3d file loaded without extension
	 std::string x3dFilePath; // complete path of the file
	 std::string x3dFileUrl;  // path of the file relative to the application's path 


	//Transform values
	float _translation [3];
	float _rotation [4];
	float _scale [3];

	//Material values
	float _ambientIntensity;
	float _shininess;
	float _alpha;

	vector<float> _diffuseColor;
	vector<float> _emissiveColor;
	vector<float> _specularColor;

	//Texture value
	std::string _texture;

	//cube
	vector<float> _size;

	//sphere & cylinder
	float _radius;
	float _height;

	int _nodeIdx;

	//RendererAdapter * rendererAdapter;
	SGAKernel * _kernel;

	//vector<string> vSensorNode;
	SceneGraph _mSceneGraph;

	RouteManager mRouteManager;

	typedef map< string, vector<string> > SensorMap; 
	SensorMap m_sensorMap;  ///< an map of all the sensors
	vector<string> _inlineFiles;

};

#endif // #ifndef __x3dWrapper_h__