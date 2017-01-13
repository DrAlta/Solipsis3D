// X3dWrapper.cpp : Defines the exported functions for the DLL application.


#include "x3dWrapper.h"
#include <algorithm>
//#include <boost/signals2/signal.hpp>
//#include <boost/bind.hpp>


using namespace std;
using namespace CyberX3D;


float bBoxSize[3];

int iChoice = 1;
string groupName;
int cptName = 0;


//static int transformIdx;
//static int shapeIdx;


/*const string sPluginName = "Plugin_x3d";
//------------------------------------------------------


//------------------------------------------------------
const Ogre::String& X3dWrapper::getName() const
{
return sPluginName;
}
//------------------------------------------------------

void X3dWrapper::install()
{
}
//------------------------------------------------------
void X3dWrapper::initialise()
{
}
//------------------------------------------------------
void X3dWrapper::shutdown()
{
}
//------------------------------------------------------
void X3dWrapper::uninstall()
{
}
//------------------------------------------------------ */

CyberX3D::Node * X3dWrapper::processInstanceNode( CyberX3D::Node * node){
	if(node->isInstanceNode()){
		CyberX3D::Node * refNode = node->getReferenceNode();
		return refNode;
	}
	else return node;
}

void X3dWrapper::getMaterial(const CyberX3D::SceneGraph *sg,CyberX3D::MaterialNode * matNode){
	getMaterialData(matNode);
}

void X3dWrapper::getMaterialData(const CyberX3D::Node * node){

	CyberX3D::MaterialNode * matNode = (MaterialNode*) node;
	initMaterialDefaultValues();
	if(matNode){

		_ambientIntensity = matNode->getAmbientIntensity();
		_shininess = matNode->getShininess();
		_alpha = 1 - matNode->getTransparency();

		float diffColor[3], emisColor[3], specColor[3];
		matNode->getDiffuseColor(diffColor);
		_diffuseColor.assign( diffColor, diffColor+ sizeof(diffColor) / sizeof(float) );
		//scale.assign(_scale, _scale + sizeof(_scale) / sizeof(float) );
		matNode->getEmissiveColor(emisColor);
		_emissiveColor.assign( emisColor, emisColor+ sizeof(emisColor) / sizeof(float) );
		matNode->getSpecularColor(specColor);
		_specularColor.assign( specColor, specColor+ sizeof(specColor) / sizeof(float) );
	}
}

void X3dWrapper::getTexture(const CyberX3D::SceneGraph *sg,CyberX3D::TextureNode * texNode){

	if(texNode){
		char * url = "url";
		Field * urlField = texNode->findField(url);	
		String urlValue = "";
		if(urlField) urlField->getValue(urlValue);
		_texture =  x3dFilePath + urlValue.getValue();
	}
}


void X3dWrapper::getSphereData(const CyberX3D::SceneGraph *sg,CyberX3D::SphereNode * sNode){
	_radius = 0;
	if(sNode) _radius = sNode->getRadius();
}

void X3dWrapper::getCylinderData(const CyberX3D::SceneGraph *sg,CyberX3D::CylinderNode * sNode){
	_radius = 0;
	_height = 0;
	if(sNode){
		_radius = sNode->getRadius();
		_height = sNode->getHeight();
	}
}


void X3dWrapper::getBoxData(const CyberX3D::SceneGraph *sg,CyberX3D::BoxNode * bNode){

	if(bNode){
		float sizeA[3];

		bNode->getSize(sizeA);

		float coordX = sizeA[0]/2;
		float coordY = sizeA[1]/2;
		float coordZ = sizeA[2]/2;
		/*float coordX = sizeA[0];
		float coordY = sizeA[1];
		float coordZ = sizeA[2];*/

		_size.push_back(coordX);
		_size.push_back(coordY);
		_size.push_back(coordZ);
	}
}

void X3dWrapper::getIFSData(const CyberX3D::SceneGraph *sg,CyberX3D::IndexedFaceSetNode * IFSNode){

	float pointsArray [3]; //temporary array for vertices coordinates

	if(IFSNode){

		MFInt32 * coordIdx = IFSNode->getCoordIndexField();
		int nbCoordIdx = coordIdx->getSize();
		//int nbPolygons = IFSNode->getNPolygons();

		CoordinateNode* coordinateNode = IFSNode->getCoordinateNodes();

		if(coordinateNode){

			int nbVertices = coordinateNode->getNPoints(); //nb of vertices
			_nbVertices = nbVertices;

			NormalNode * normalCoordNode = IFSNode->getNormalNodes();
			ColorNode * colorNode = IFSNode->getColorNodes();
			TextureCoordinateNode * textCoordNode = IFSNode->getTextureCoordinateNodes();

			MFVec3f * coordPointField = coordinateNode->getPointField();
			MFVec3f * normalVectorField = NULL;
			MFColor * colorValueField = NULL;
			MFVec2f * texValueField = NULL;
			if(normalCoordNode) normalVectorField = normalCoordNode->getVectorField();
			if(colorNode) colorValueField = colorNode->getColorField();
			if(textCoordNode) texValueField = textCoordNode->getPointField();

			int nbNIdx = IFSNode->getNNormalIndexes();
			int nbColors = IFSNode->getNColorIndexes(); 
			int nbTIdx = IFSNode->getNTexCoordIndexes();

			int i, cIdx, j, colIdx, tIdx, nIdx;
			int cptFace = 0;
			int cptIdx = 0;
			for(i=0; i<nbCoordIdx; i++){

				cIdx = coordIdx->get1Value(i);
				if( cIdx == -1 ){
					cptFace++;
					continue;
				}

				coordPointField->get1Value(cIdx,pointsArray);
				_verticesArray.push_back(pointsArray[0]);
				_verticesArray.push_back(pointsArray[1]);
				_verticesArray.push_back(pointsArray[2]);

				if(normalCoordNode){
					(nbNIdx!=0) ? nIdx = IFSNode->getNormalIndex(i) : nIdx=cIdx;
					normalVectorField->get1Value(nIdx,pointsArray);
					_normalsArray.push_back(pointsArray[0]);
					_normalsArray.push_back(pointsArray[1]);
					_normalsArray.push_back(pointsArray[2]);
				}
				if(colorNode){
					(nbColors!=0) ? colIdx = IFSNode->getNormalIndex(i) : colIdx=cIdx;
					colorValueField->get1Value(colIdx,pointsArray);
					_coloursArray.push_back(pointsArray[0]);
					_coloursArray.push_back(pointsArray[1]);
					_coloursArray.push_back(pointsArray[2]);
				}
				if(textCoordNode){
					(nbTIdx!=0) ? tIdx = IFSNode->getTexCoordIndex(i) : tIdx=cIdx;
					texValueField->get1Value(tIdx,pointsArray);
					_textureCoordArray.push_back(pointsArray[0]);
					_textureCoordArray.push_back(pointsArray[1]);
				}
				/* no textureCoordinate but a texture is attached to the mesh => compute default textureCoordinate as specified in X3D specification*/
				else if(!_texture.empty()) {
					if(i%4==0 ){ _textureCoordArray.push_back(0);_textureCoordArray.push_back(0); }
					else if(i%4==1){  _textureCoordArray.push_back(0);_textureCoordArray.push_back(1); }
					else if(i%4==2){  _textureCoordArray.push_back(1);_textureCoordArray.push_back(1); }
					else {  _textureCoordArray.push_back(1);_textureCoordArray.push_back(0); }
				}
				_facesArray.push_back(cptIdx);
				cptIdx++;
			}
			_nbFaces = cptFace;
		}
	}
}

void X3dWrapper::renderShape(CyberX3D::GeometryNode * gnode, CyberX3D::AppearanceNode * apNode){

	string shapeIdx = getNodeName(gnode);
	//_nodeIdx++;
	if(gnode->isIndexedFaceSetNode()){
		_kernel->createMesh(shapeIdx,x3dFileUrl,_verticesArray,_normalsArray,_textureCoordArray,_coloursArray,_facesArray);
	}
	else if(gnode->isBoxNode()){
		_kernel->createBoxShape(shapeIdx,x3dFileUrl,_size);
	}
	else if(gnode->isSphereNode()){
		_kernel->createSphereShape(shapeIdx,x3dFileUrl,_radius);
	}
	else if(gnode->isCylinderNode()){
		_kernel->createCylinderShape(shapeIdx,x3dFileUrl,_radius,_height);
	}
	//_kernel->setNodePhysicsProperties(shapeIdx,x3dFileName,0.0);
	if(apNode){
		CyberX3D::MaterialNode * matNode = apNode->getMaterialNodes();
		if(matNode){
			string matIdx = getNodeName(matNode);
			_kernel->createMaterialProperty(matIdx,x3dFileUrl,_ambientIntensity,_shininess,_alpha,_diffuseColor,_emissiveColor,_specularColor);
		}
		CyberX3D::TextureNode * texNode = apNode->getTextureNode();
		if(texNode){
			string textureIdx = getNodeName(texNode);
			_kernel->createTextureProperty(textureIdx,x3dFileUrl,_texture);
		}
	}
	_kernel->attachShapeToCurrentGroupNode(shapeIdx,x3dFileUrl);
	this->clean();	
}

string X3dWrapper::getNodeName(CyberX3D::Node * node){

	char * nName = NULL;
	string res = "";

	if( !node->isInstanceNode() ){
		nName = node->getName();
	}
	if( nName && string(nName)!="") {
		res = string(nName);
	}
	else{
		//Geometry
		if( node->isGeometry3DNode() ){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::GeometryNode);
		}
		//Material
		else if( node->isMaterialNode() ){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::MaterialNode);
		}
		//Texture
		else if( node->isTextureNode() ){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::TextureNode);
		}
		//Viewpoint
		else if(node->isViewpointNode()){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::ViewpointNode);
		}
		//Transform
		else if(node->isTransformNode()){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::TransformNode);
		}
		//Timesensor
		else if(node->isTimeSensorNode()){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::SensorNode);
		}
		//Group
		else if(node->isGroupNode()){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::GroupNode);
		}
		//Inline
		else if(node->isInlineNode()){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::InlineNode);
		}
		//Background
		else if(node->isBackgroundNode()){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::GenericNode);
		}
		node->setName(res.c_str());	
		ostringstream oss;
		oss << _nodeIdx;
		//groupName = "switcher_" + oss.str();
		string s = "log_" + x3dFileName + "_SETNAME.xml";
		_mSceneGraph.saveXML(s.c_str());
	}
	_nodeIdx++;
	return res;
}


void X3dWrapper::processShapeNode(const CyberX3D::SceneGraph *sg, CyberX3D::ShapeNode *shape){

	shape = (ShapeNode*)  processInstanceNode(shape);

	CyberX3D::AppearanceNode * apNode = shape->getAppearanceNodes();
	if(apNode){
		apNode = (AppearanceNode*)  processInstanceNode(apNode);
		CyberX3D::MaterialNode * matNode = apNode->getMaterialNodes();
		getMaterial(sg,matNode);
		CyberX3D::TextureNode * texNode = apNode->getTextureNode();
		getTexture(sg,texNode);
	}

	CyberX3D::GeometryNode *gnode = shape->getGeometry3D();
	if (gnode) {
		gnode = (GeometryNode*)  processInstanceNode(gnode);
		if(gnode->isIndexedFaceSetNode()){
			CyberX3D::IndexedFaceSetNode * IFSNode = shape->getIndexedFaceSetNodes();
			getIFSData(sg,IFSNode);
		}

		else if(gnode->isBoxNode()){
			BoxNode * bNode = shape->getBoxeNodes();
			getBoxData(sg,bNode);
		}
		else if(gnode->isSphereNode()){
			SphereNode * sNode = shape->getSphereNodes();
			getSphereData(sg,sNode);
		}
		else if(gnode->isCylinderNode()){
			CylinderNode * cNode = shape->getCylinderNodes();
			getCylinderData(sg,cNode);
		}
		renderShape(gnode,apNode);
	}

}

void X3dWrapper::processViewpoint(const CyberX3D::SceneGraph *sg, CyberX3D::ViewpointNode * viewpoint){

	if(viewpoint){
		string vName = getNodeName(viewpoint);
		//_nodeIdx++;

		//X3D default values
		float curOrientation[4] = {0,0,1,0};
		float curPosition[3] = {0,0,10};

		//set node values if node is defined using USE
		viewpoint = (ViewpointNode*)  processInstanceNode(viewpoint);

		viewpoint->getOrientation(curOrientation);
		viewpoint->getPosition(curPosition);

		vector<float>  orientation (curOrientation, curOrientation + sizeof(curOrientation) / sizeof(float));
		vector<float>  position (curPosition, curPosition + sizeof(curPosition) / sizeof(float));

		_kernel->createViewpointNode(vName,x3dFileUrl,position,orientation);
	}
}

void X3dWrapper::processBackgroundNode(const CyberX3D::SceneGraph *sg, CyberX3D::BackgroundNode * bgNode){

	if(bgNode){
		string vName = getNodeName(bgNode);

		//set node values if node is defined using USE
		bgNode = (BackgroundNode*)  processInstanceNode(bgNode);

		MFFloat * cAngle= bgNode->getGroundAngleField();

		float curColor[3] ;
		bgNode->getGroundColor(0,curColor);
		vector<float> color (curColor, curColor + sizeof(curColor) / sizeof(float));

		float curNormal[3] = {0,1,0};
		vector<float> normal (curNormal, curNormal + sizeof(curNormal) / sizeof(float));

		std::string urlValue = std::string();
		char * temp = bgNode->getBottomUrl(0);
		if(temp) urlValue =  x3dFilePath + std::string(temp);

		float constante = 0.0;

		_kernel->createGround(vName,x3dFileUrl,normal,constante,color, urlValue );
	}
}

void X3dWrapper::getTransformData(const CyberX3D::Node *node, vector<float> &translation, vector<float> &scale, vector<float> &rotation){

	TransformNode * tNode = (TransformNode*) node;
	tNode->getScale(_scale);
	tNode->getTranslation(_translation);
	tNode->getRotation(_rotation);

	scale.assign(_scale, _scale + sizeof(_scale) / sizeof(float) );
	translation.assign( _translation, _translation + sizeof(_translation) / sizeof(float) );

	//axis+angle representation to quaternion representation conversion
	rotation.push_back(_rotation[0]*sin(_rotation[3]/2));
	rotation.push_back(_rotation[1]*sin(_rotation[3]/2));
	rotation.push_back(_rotation[2]*sin(_rotation[3]/2));
	rotation.push_back(cos(_rotation[3]/2));

}

string X3dWrapper::processGroupNode (const CyberX3D::SceneGraph *sg, CyberX3D::GroupNode * group){

	string nodeId = string();

	if(group){
		nodeId = getNodeName(group);
		_kernel->createGroupNode(nodeId,x3dFileUrl);
	}
	return nodeId;
}


string X3dWrapper::processTransformNode (const CyberX3D::SceneGraph *sg, CyberX3D::TransformNode *transform){

	string nodeId = string();

	if(transform){

		nodeId = getNodeName(transform);
		//_nodeIdx++;
		TransformNode * curTransform = transform;

		initTransformDefaultValues();
		transform = (TransformNode*)  processInstanceNode(transform);
		if (transform->hasChildNodes() && curTransform->isInstanceNode() ){
			curTransform->deleteChildNodes();
			CyberX3D::Node * newChildNode = transform->getChildNodes();
			CyberX3D::Node * node;
			for (node = newChildNode; node; node=node->next()) {
				curTransform->addChildNode(node);
			}
		}
		vector<float> translation;
		vector<float> scale;
		vector<float> rotation;
		getTransformData(transform,translation,scale,rotation);
		_kernel->createTransformNode(nodeId,x3dFileUrl,translation,scale,rotation);
		//_kernel->createTransformNode("NODE",x3dFileUrl,translation,scale,rotation,"FAKE");
	}
	return nodeId;
}


void X3dWrapper::processSceneGraph(const CyberX3D::SceneGraph* sceneGraph, CyberX3D::Node* firstNode, CyberX3D::Node* lastNode){

	if(!firstNode) return;
	if(firstNode==lastNode) return;
	CyberX3D::Node	*node;
	for (node = firstNode; node; node=node->next()) {
		if(node==lastNode) break;
		char * nName = node->getName();
		const char * nType = node->getTypeString();
		if(node->isShapeNode()){
			processShapeNode(sceneGraph, (ShapeNode *) node);
		}
		else if (node->isTransformNode()) {
			string nodeIdent = processTransformNode(sceneGraph, (TransformNode *)node);
			processSceneGraph(sceneGraph,node->getChildNodes());
			_kernel->closeGroupNode( nodeIdent, x3dFileUrl );
		}
		else if (node->isGroupNode()) {
			string nodeIdent = processGroupNode(sceneGraph, (GroupNode *)node);
			processSceneGraph(sceneGraph,node->getChildNodes());
			_kernel->closeGroupNode( nodeIdent, x3dFileUrl );
		}
		else if ( node->isViewpointNode()){
			processViewpoint(sceneGraph , (ViewpointNode *) node);
		}
		else if ( node->isTimeSensorNode()){
			TimeSensorNode * tNode = (TimeSensorNode *) node;
			string tNodeName = string(tNode->getName());
			//vSensorNode.push_back(tNodeName);
		}
		else if ( node->isSwitchNode() ){
			SwitchNode * swiNode = (SwitchNode*) node;
			int sChoice = swiNode->getWhichChoice();
			GroupNode * gNode = new GroupNode();
			gNode->setName(swiNode->getName());
			string nodeIdent = processGroupNode(sceneGraph, gNode);
			if(sChoice!=-1){
				Node * childNode = swiNode->getChildNode(sChoice);
				Node * nextNode = swiNode->getChildNode(sChoice+1);
				if(!nextNode) nextNode = swiNode->next();
				processSceneGraph(sceneGraph,childNode,nextNode);
			}
			_kernel->closeGroupNode( nodeIdent, x3dFileUrl );
		}
		else if ( node->isInlineNode() ){
			InlineNode * inlineNode = (InlineNode*) node;
			string gNodeName = getNodeName(inlineNode);
			//GroupNode * gNode = new GroupNode();
			//gNode->setName(gNodeName.c_str());
			//string nodeIdent = processGroupNode(sceneGraph, gNode);
			MFString * sUrl = inlineNode->getUrlField();
			if( inlineNode->getLoad() ) _kernel->loadFile(x3dFilePath + (sUrl->get1Value(0)),x3dFilePath);
			//_kernel->closeGroupNode( nodeIdent, x3dFileUrl );
		}
		else if ( node->isBackgroundNode() ){
			//BackgroundNode * bgNode = (BackgroundNode*) node;
			processBackgroundNode(sceneGraph , (BackgroundNode *) node);
		}
		else{
			processSceneGraph(sceneGraph,node->getChildNodes());
		}
	}
}

void X3dWrapper::processRoutes(CyberX3D::SceneGraph* sceneGraph){

	CyberX3D::Route * firstRoute = sceneGraph->getRoutes();

	CyberX3D::Route * route;

	for (route = firstRoute; route; route=route->next()) {

		char * toFieldName = route->getEventInField()->getName();
		char * toNodeName = route->getEventInNode()->getName();

		char * fromFieldName = route->getEventOutField()->getName();
		char * fromNodeName = route->getEventOutNode()->getName();

		mRouteManager.addRoute(string(fromNodeName),string(fromFieldName),string(toNodeName),string(toFieldName));
	}
}
void X3dWrapper::processSensors( CyberX3D::SceneGraph * sceneGraph, CyberX3D::Node* firstNode){

	//CyberX3D::Node* firstNode = sceneGraph->getNodes();
	Node* node;
	Node * parentNode;
	Node * cNode;

	for (node = firstNode; node; node=node->nextTraversal()) {
		if(node->isSensorNode()){
			string sName = string(node->getName());

			parentNode = node->getParentNode();
			if(!parentNode) cNode = sceneGraph->getNodes();
			else cNode = parentNode->getChildNodes();
			string nName;
			while(cNode){
				nName = string(cNode->getName());
				if( !cNode->isGeometry3DNode() && !cNode->isShapeNode() ){
					if (cNode->isInterpolatorNode() || cNode->isSensorNode() || cNode->isCommonNode() ||  cNode->isAppearanceNode() || cNode->isRouteNode() || cNode->isBackgroundNode() || cNode->isViewpointNode() ) {
						cNode = cNode->nextTraversal();
						continue;
					}
				}
				if( !nName.empty() || nName!="") {
					if(m_sensorMap.find(nName) != m_sensorMap.end()){
						vector<string>::iterator it;
						it = find(m_sensorMap.find(nName)->second.begin(), m_sensorMap.find(nName)->second.end(), nName);
						if(it != m_sensorMap.find(nName)->second.end() ) m_sensorMap.find(nName)->second.push_back(sName);

					}
					else { 
						m_sensorMap.insert(SensorMap::value_type( nName, vector<string>() ));
						m_sensorMap.find(nName)->second.push_back(sName);
					}
				}
				cNode = cNode->nextTraversal();
			}
		}
	}
}

void X3dWrapper::processInlineFiles(){

	for(vector<string>::iterator it = _inlineFiles.begin(); it != _inlineFiles.end();++it){
		_kernel->loadFile(*it,x3dFilePath);
	}
}

//int X3dWrapper::adaptFormat( const string & fileName, const string & filePath, EngineWrapperFactory::EngineWrapper * ogLoader)
int X3dWrapper::adaptFormat( const string & fileName, const string & filePath, SGAKernel * sgaKernel){

	//size_t lastSlashFile = fileName.find_last_of("\\") + 1;
	size_t lastSlashFile = fileName.find_last_of("/\\") + 1;
	size_t lastSlashPath = filePath.find_last_of("/\\") + 1;
	if( PathIsRelative(fileName.c_str())) x3dFilePath = filePath.substr(0,lastSlashPath) + fileName.substr(0,lastSlashFile);
	else x3dFilePath = filePath.substr(0,lastSlashPath);

	string szMeshPrefix = fileName.substr(0,fileName.length()-4);
	size_t lastSlash = szMeshPrefix.find_last_of("\\");
	x3dFileName = szMeshPrefix.substr(lastSlash+1);

	bool loadingResult = _mSceneGraph.load(fileName.c_str(), true);
	if (loadingResult == false) {
		cout << "CyberX3D Error(" << _mSceneGraph.getParserErrorLineNumber() << ") : " << _mSceneGraph.getParserErrorLineString() << endl;
		cout << "\t" << _mSceneGraph.getParserErrorMessage() << endl;
		cout << "\t" << _mSceneGraph.getParserErrorToken() << endl;
		return 0;
	}
	else{
		try{
			CyberX3D::Node*  firstNode = _mSceneGraph.getNodes();
			_kernel = sgaKernel;
			if(_kernel){
				processSceneGraph(&_mSceneGraph,firstNode);
				string s = "log_" + x3dFileName + ".xml";
				_mSceneGraph.saveXML(s.c_str());
				processRoutes(&_mSceneGraph);
				processSensors(&_mSceneGraph,firstNode);
				//processInlineFiles();
			}
		}
		catch(exception e){
			fprintf(stderr, "An exception has occurred in X3DWrapper: %s\n",e.what());
			return 0;
		}
	}
	return 1;
}

void X3dWrapper::updateElapsedTime( const float elapsedTime ){

	TimeSensorNode * timeSNode;
	for (timeSNode = _mSceneGraph.findTimeSensorNode(); timeSNode; timeSNode=timeSNode->nextTraversal()) {
		timeSNode->update();
		if( timeSNode->getEnabled() == true /*&& timeSNode->isActive()*/){				
			animRoutes(timeSNode);
		}
	}
}

void X3dWrapper::onPress ( const string &nodeId, const float time, const float x, const float y ){

	Node * node = _mSceneGraph.findNode(nodeId.c_str());
	string nName;

	if(!node) return;
	nName = string(node->getName());
	if( m_sensorMap.find(nName) != m_sensorMap.end() ){
		vector<string> sNameArray = m_sensorMap.find(nName)->second;
		for(vector<string>::iterator it = sNameArray.begin(); it != sNameArray.end();++it){
			string sName = *it;
			Node * sNode = _mSceneGraph.findNode(sName.c_str());
			if(sNode->isTouchSensorNode()){
				TouchSensorNode * tsNode = (TouchSensorNode*) sNode;
				if(!tsNode->getIsActive()){
					tsNode->setIsActive(true);
					tsNode->setTouchTime(CyberX3D::GetCurrentSystemTime());
				}
				animRoutes(tsNode);
			}
		}
	}
}

void X3dWrapper::onRelease ( const string &nodeId, const float time, const float x, const float y ){

	Node * node = _mSceneGraph.findNode(nodeId.c_str());
	string nName;

	if(!node) return;
	nName = string(node->getName());
	if( m_sensorMap.find(nName) != m_sensorMap.end() ){
		vector<string> sNameArray = m_sensorMap.find(nName)->second;
		for(vector<string>::iterator it = sNameArray.begin(); it != sNameArray.end();++it){
			string sName = *it;
			Node * sNode = _mSceneGraph.findNode(sName.c_str());
			if(sNode->isTouchSensorNode()){
				TouchSensorNode * tsNode = (TouchSensorNode*) sNode;
				if(tsNode->getIsActive()){
					tsNode->setIsActive(false);
					tsNode->setTouchTime(CyberX3D::GetCurrentSystemTime());
				}
				animRoutes(tsNode);
			}
		}
	}
}

void X3dWrapper::onEnter( const string &nodeId, const float time, const float x, const float y ){

	Node * node = _mSceneGraph.findNode(nodeId.c_str());
	string nName;

	if(!node) return;
	nName = string(node->getName());

	if( m_sensorMap.find(nName) != m_sensorMap.end() ){
		vector<string> sNameArray = m_sensorMap.find(nName)->second;
		for(vector<string>::iterator it = sNameArray.begin(); it != sNameArray.end();++it){
			string sName = *it;
			Node * sNode = _mSceneGraph.findNode(sName.c_str());
			if(sNode->isTouchSensorNode()){
				TouchSensorNode * tsNode = (TouchSensorNode*) sNode;
				if(!tsNode->getIsOver()){
					tsNode->setIsOver(true);
					//tsNode->setTouchTime(CyberX3D::GetCurrentSystemTime());
				}
				/*tsNode->setHitPointChanged(x,y,0);
				tsNode->setHitNormalChanged(x,y,0);*/
				tsNode->setHitTexCoord(x,y);
				animRoutes(tsNode);
			}
		}
	}
}

void X3dWrapper::onExit( const string &nodeId, const float time, const float x, const float y ){

	Node * node = _mSceneGraph.findNode(nodeId.c_str());
	string nName;

	if(!node) return;
	nName = string(node->getName());

	if( m_sensorMap.find(nName) != m_sensorMap.end() ){
		vector<string> sNameArray = m_sensorMap.find(nName)->second;
		for(vector<string>::iterator it = sNameArray.begin(); it != sNameArray.end();++it){
			string sName = *it;
			Node * sNode = _mSceneGraph.findNode(sName.c_str());
			if(sNode->isTouchSensorNode()){
				TouchSensorNode * tsNode = (TouchSensorNode*) sNode;
				if(tsNode->getIsOver()) tsNode->setIsOver(false);
				animRoutes(tsNode);
			}
		}
	}
}

void X3dWrapper::animRoutes(CyberX3D::Node*  updatedNode){

	char * nodeName = updatedNode->getName();
	vector<WRoute> vRoutes = mRouteManager.getRoutes(string(nodeName));
	bool needUpdate;
	if(!vRoutes.empty()){
		vector<WRoute>::iterator it;
		for(it=vRoutes.begin(); it!=vRoutes.end(); it++){
			WRoute curRoute = (*it); 
			Node * toNode = _mSceneGraph.findNode(curRoute.toNodeName.c_str());
			Field * fromField = updatedNode->findField(curRoute.fromFieldName.c_str());
			Field * toField = toNode->findField(curRoute.toFieldName.c_str());
			needUpdate = false;

			if( toField->getType()==fromField->getType() ){
				if( toField->isSField() ){
					if( toField->isSFBool() ) {
						SFBool * toBoolField = (SFBool*) toField;
						SFBool * fromBoolField = (SFBool*) fromField;
						if(!toBoolField->equals(fromBoolField)){
							toBoolField->setValue(fromBoolField);
							needUpdate = true;
						}
					}
					else if( toField->isSFTime() ){
						SFTime * toTimeField = (SFTime*) toField;
						SFTime * fromTimeField = (SFTime*) fromField;
						if(!toTimeField->equals(fromTimeField)){
							toTimeField->setValue(fromTimeField);
							needUpdate = true;
						}

					}
					else if( toField->isSFFloat() ){
						SFFloat * toFloatField = (SFFloat*) toField;
						SFFloat * fromFloatField = (SFFloat*) fromField;
						if(!toFloatField->equals(fromFloatField)){
							toFloatField->setValue(fromFloatField);
							needUpdate = true;
						}
					}
					else if( toField->isSFColor()){
						SFColor * toColorField = (SFColor*) toField;
						SFColor * fromColorField = (SFColor*) fromField;
						if(!toColorField->equals(fromColorField)){
							toColorField->setValue(fromColorField);
							needUpdate = true;
						}
					}
					else if( toField->isSFImage()){
						SFImage * toImageField = (SFImage*) toField;
						SFImage * fromImageField = (SFImage*) fromField;
						if(!toImageField->equals(fromImageField)){
							toImageField->setValue(fromImageField);
							needUpdate = true;
						}
					}
					else if( toField->isSFInt32() ){
						SFInt32 * toIntField = (SFInt32*) toField;
						SFInt32 * fromIntField = (SFInt32*) fromField;
						if(!toIntField->equals(fromIntField)){
							toIntField->setValue(fromIntField);
							needUpdate = true;
						}
					}
					else if ( toField->isSFRotation() ){
						SFRotation * toRotationField = (SFRotation*) toField;
						SFRotation * fromRotationField = (SFRotation*) fromField;
						if(!toRotationField->equals(fromRotationField)){
							toRotationField->setValue(fromRotationField);
							needUpdate = true;						
						}
					}
					else if( toField->isSFString()){
						SFString * toStringField = (SFString*) toField; 
						SFString * fromStringField = (SFString*) fromField; 
						if(!toStringField->equals(fromStringField)){
							toStringField->setValue(fromStringField);
							needUpdate = true;
						}
					}
					else if ( toField->isSFVec2f() ) {
						SFVec2f * toVec2Field = (SFVec2f*) toField; 
						SFVec2f * fromVec2Field = (SFVec2f*) fromField; 
						if(!toVec2Field->equals(fromVec2Field)){
							toVec2Field->setValue(fromVec2Field);
							needUpdate = true;
						}
					}
					else if ( toField->isSFVec3f() ){
						SFVec3f * toVec3Field = (SFVec3f*) toField; 
						SFVec3f * fromVec3Field = (SFVec3f*) fromField; 
						if(!toVec3Field->equals(fromVec3Field)){
							toVec3Field->setValue(fromVec3Field);
							needUpdate = true;
						}
					}
				}
				else /*toField->isMField() */{
					MField * toMField = (MField*) toField;
					MField * fromMField = (MField*) fromField;
					if(!toMField->equals(fromMField)){
						toMField->setValue(fromMField);
						needUpdate = true;
					}
				}
				if(needUpdate) updateNodeRendering(toNode);
				animRoutes(toNode);
			}
		}
	}
}


//void X3dWrapper::animRoutes(CyberX3D::Node*  updatedNode){
//
//	char * nodeName = updatedNode->getName();
//	vector<WRoute> vRoutes = mRouteManager.getRoutes(string(nodeName));
//	vector<WRoute>::iterator it;
//	Node * toNode;
//	Field * fromField;
//	Field * toField;
//
//	if(!vRoutes.empty()){
//
//		for(it=vRoutes.begin(); it!=vRoutes.end(); it++){
//
//			WRoute curRoute = (*it); 
//			toNode = _mSceneGraph.findNode(curRoute.toNodeName.c_str());
//			fromField = updatedNode->findField(curRoute.fromFieldName.c_str());
//			toField = toNode->findField(curRoute.toFieldName.c_str());
//
//			if( toField->getType()==fromField->getType() ){
//
//				if( toField->isSField() ){
//					String fromFieldValue = "";
//					fromField->getValue(fromFieldValue);
//					toField->setValue(fromFieldValue.getValue());
//				}
//				else /*toField->isMField() */{
//					MField * toMField = (MField*) toField;
//					MField * fromMField = (MField*) fromField;
//					toMField->setValue(fromMField);
//				}
//				updateNodeRendering(toNode);
//				animRoutes(toNode);
//			}
//		}
//	}
//}


void X3dWrapper::updateNodeRendering(CyberX3D::Node * node){

	string nodeId = string(node->getName());

	if(node->isTransformNode()){
		vector<float> translation;
		vector<float> scale;
		vector<float> rotation;
		getTransformData(node,translation,scale,rotation);
		_kernel->setTransformNode(nodeId,x3dFileUrl,translation,rotation,scale);
	}
	else if(node->isMaterialNode()){
		getMaterialData(node);
		_kernel->setMaterialProperty(nodeId,x3dFileUrl,_ambientIntensity,_shininess,_alpha,_diffuseColor,_emissiveColor,_specularColor);
	}
	else if (node->isSwitchNode()){
		SwitchNode * switchNode = (SwitchNode*) node;
		int sChoice = switchNode->getWhichChoice();
		_kernel->deleteNodeChildren(nodeId,x3dFileUrl);
		if(sChoice!=-1){
			Node * childNode = switchNode->getChildNode(sChoice);
			if(childNode){
				Node * nextNode = switchNode->getChildNode(sChoice+1);
				if(!nextNode) nextNode = switchNode->next();
				processSceneGraph(&_mSceneGraph, childNode, nextNode);
				processSensors(&_mSceneGraph, childNode);
			}		
		}
	}
	else{ 
		node->update(); 
	}
}

TransformNode * X3dWrapper::getDerivedTransform ( TransformNode * tNode ){

	Node * curNode;
	TransformNode * curTransNode;
	TransformNode * res = new TransformNode();
	float rTrans [4], cTrans[4], nTrans[4];

	tNode->getTranslation(rTrans);
	res->setTranslation(rTrans);
	tNode->getScale(rTrans);
	res->setScale(rTrans);
	tNode->getRotation(rTrans);
	res->setRotation(rTrans);

	curNode = (Node*) tNode;

	while( curNode->getParentNode() != NULL ){

		curNode = curNode->getParentNode();
		if(curNode->isTransformNode()){

			curTransNode = (TransformNode*) curNode;			

			//translation
			curTransNode->getTranslation(cTrans);
			if(! (cTrans[0]==cTrans[1]==cTrans[2]==0) ){
				res->getTranslation(rTrans);
				X3DWrapperTools::addTranslations(rTrans,cTrans,nTrans);
				res->setTranslation(nTrans);
			}
			//scale
			curTransNode->getScale(cTrans);
			if(! (cTrans[0]==cTrans[1]==cTrans[2]==1) ){
				res->getScale(rTrans);
				X3DWrapperTools::addScales(rTrans,cTrans,nTrans);
				res->setScale(nTrans);
			}

			//rotation
			curTransNode->getScale(cTrans);
			if( !(cTrans[0]==cTrans[1]==cTrans[3]==0) && !cTrans[2]==1 ){
				res->getRotation(rTrans);
				X3DWrapperTools::addRotations(rTrans,cTrans,nTrans);
				res->setRotation(nTrans[0],nTrans[1],nTrans[2],nTrans[3]);
			}
		}
	}

	return res;
}

void X3dWrapper::updateNodePosition( const string &nodeId, const vector<float> position, const std::vector<float> rotation ){

	float dTrans [3], diffTrans[3]/*, curTrans[3]*/;
	
	Node * node = _mSceneGraph.findNode(nodeId.c_str()); //geometry node
	Node * parentNode = node->getParentNode()->getParentNode(); //its transform parent node

	if(position.empty()) return;

	if(!parentNode){
		//Node * shapeNode = node->getParentNode();
		////its parent node is the root node
		//if(shapeNode){
		//	TransformNode * trans = new TransformNode();
		//	trans->addChildNodeAtFirst(shapeNode);
		//	_mSceneGraph.addNode(trans);
		//	string s = "log_update" + x3dFileName + ".xml";
		//	_mSceneGraph.saveXML(s.c_str());
		//}
		return;
	}
	if( parentNode->isTransformNode()){
		/*  la nouvelle translation est la plupart du temps, une position absolue ( le moteur physique n'a pas de graphe de scène )*/
		TransformNode * tNode = (TransformNode*) parentNode;
		if(!tNode->getParentNode()){
			tNode->setTranslation(position.at(0),position.at(1),position.at(2));
		}
		else{
			TransformNode * derivedTrans = getDerivedTransform(tNode);
			derivedTrans->getTranslation(dTrans);

			float curTrans [3] = { position.at(0),position.at(1),position.at(2) };
			X3DWrapperTools::subTranslations(curTrans,dTrans,diffTrans);

			tNode->getTranslation(curTrans);
			X3DWrapperTools::addTranslations(curTrans,diffTrans,dTrans);

			tNode->setTranslation(dTrans);
		}

		float uRot[4];
		float uQRot[4] = { rotation.at(0),rotation.at(1),rotation.at(2),rotation.at(3) };
		X3DWrapperTools::quaternionToAxis_angle(uQRot,uRot);
		tNode->setRotation(uRot);


		updateNodeRendering(tNode);
	}
}

std::string X3dWrapper::getNodeFieldValue ( const std::string &nodeId,  const string &fieldId ){
	
	std::string res = std::string();
	Node * node = _mSceneGraph.findNode(nodeId.c_str());
	if(node){
		Field * field = node->findField(fieldId.c_str());
		if(field){
			String sValue;
			field->getValue(sValue);
			res=  sValue.getValue();
		}
	}
	return res;
}

void X3dWrapper::setNodeFieldValue( const std::string &nodeId, const string &fieldId, const string &sValue ){

	Node * node = _mSceneGraph.findNode(nodeId.c_str());
	Field * field = node->findField(fieldId.c_str());
	field->setValue(sValue.c_str());
	int cpt = 0;
	updateNodeRendering(node);
}


/// Tells us to register our functionality to an engine kernel
extern "C" X3D_WRAPPER_API size_t registerPlugin(SGAKernel &wf) {
	wf.getFormatWrappers().addWrapper(
		auto_ptr<FormatWrapperFactory::FormatWrapper> ( new X3dWrapper(wf.getCurrentFileName()) )
		);
	return wf.getFormatWrappers().getWrapperCount()-1;
}

extern "C" X3D_WRAPPER_API WrapperConfiguration *  queryPlugin(SGAKernel &wf){
	X3dWrapper * xWrapper = new X3dWrapper();
	return xWrapper->getWrapperConfiguration();
}
