#include "SGAKernel.h"
//#include "SGA3DContainerDecoder.h"

using namespace SceneGraphAdapter;


void SGAKernel::loadFile( const std::string &sFileName, const std::string &sFilePath, const std::string &sRendererIdentifier ){
	EngineWrapperFactory::EngineWrapper * rW;
	size_t idx = loadAppropriatePlugin(sFileName);
	if(getEngineWrapper().getWrapperCount()>0){
		if(sRendererIdentifier.empty()) rW = getEngineWrapper().getWrapper(0);
		else rW = getEngineWrapper().getWrapper(sRendererIdentifier);
		int res = getFormatWrappers().getWrapper(idx)->adaptFormat(sFileName,sFilePath, this);
		if(res==0) printError(1,sFileName);
		//getFormatWrappers().clean();
	}
	else printError(5,sFileName);
}

void SGAKernel::registerPlugin (const std::string &sFilename) {
	WrapperPlugin * wPlugin = WrapperPlugin::checkWrapperPlugin(sFilename);
	if(wPlugin){
		WrapperConfiguration * wConfig = wPlugin->queryPlugin();
		wPlugin->setWrapperConfiguration(wConfig);
		string sExt = wConfig->getExtension();

		// - FormatWrapper
		if(wPlugin->getType() == WrapperPlugin::WrapperType::FormatWrapper){	
			if(sExt.empty()) printError(2,sFilename);
			else if(m_AvailableFormatWrapperMap.find(sExt) != m_AvailableFormatWrapperMap.end()) printError(3,sExt);
			else m_AvailableFormatWrapperMap.insert(FormatWrapperMap::value_type( sExt , sFilename ));
		}
	}
}

int SGAKernel::loadPlugin(const std::string &sPluginName) {
	int idx;
	if(m_LoadedWrapper.find(sPluginName) == m_LoadedWrapper.end())
		idx = m_LoadedWrapper.insert(WrapperMap::value_type(sPluginName,WrapperPlugin(sPluginName))).first->second.registerPlugin(*this);
	else{
		idx =m_LoadedWrapper.find(sPluginName)->second.registerPlugin(*this);
	}
	return idx;
}

int SGAKernel::loadAppropriatePlugin (const std::string &sFilename) {

	int idx;
	m_sCurrentFileName = sFilename;
	size_t lastPoint = sFilename.find_last_of(".") + 1;
	string sFileExtension = sFilename.substr(lastPoint);
	if( m_AvailableFormatWrapperMap.find(sFileExtension) == m_AvailableFormatWrapperMap.end() ){
		printError(0,sFileExtension);
		return -1; 
	}
	string sPluginName = m_AvailableFormatWrapperMap.find(sFileExtension)->second;
	if(sPluginName==""){
		printError(0,sFileExtension);
		return -1;
	}
	
	if(m_LoadedWrapper.find(sPluginName) == m_LoadedWrapper.end()){
		idx = m_LoadedWrapper.insert(WrapperMap::value_type(sPluginName,WrapperPlugin(sPluginName))).first->second.registerPlugin(*this);
	}
	else{
		idx =m_LoadedWrapper.find(sPluginName)->second.registerPlugin(*this);
	}
	return idx;
}

void SGAKernel::updateSceneTime (const float elapsedTime) {
	
	int iWrapper;
	
	size_t  nbFWrapper = getFormatWrappers().getWrapperCount();
	for(iWrapper=0; iWrapper<nbFWrapper; iWrapper++) getFormatWrappers().getWrapper(iWrapper)->updateElapsedTime(elapsedTime);

	size_t nbEngine = getEngineWrapper().getWrapperCount(); 
	for( iWrapper= 0; iWrapper<nbEngine; iWrapper++) getEngineWrapper().getWrapper(iWrapper)->run(elapsedTime);
	
}

void SGAKernel::onPress(const std::string & nodeName, const std::string & sceneGraphId, const float clickTime, const float x, const float y, const std::string & parentNodeName ){

	string pFile;
	FormatWrapperFactory::FormatWrapper * fWrapper;
	
	string sIdent = NodeIndexer::getInstance()->getEngineNodeInputIdentifier(nodeName,sceneGraphId);
	string sFile = NodeIndexer::getInstance()->getEngineNodeInputFile(nodeName,sceneGraphId);
	
	if( !parentNodeName.empty() ){
		pFile = NodeIndexer::getInstance()->getEngineNodeInputFile(parentNodeName,sceneGraphId);
		if( !pFile.empty() && pFile.compare(sFile)!=0 ){
			sIdent = NodeIndexer::getInstance()->getEngineNodeInputIdentifier( parentNodeName,sceneGraphId );
			sFile = pFile;
		}
	}
	fWrapper = getFormatWrappers().getWrapper(sFile);
	if(sIdent.empty()) {
		printError(6,nodeName,sceneGraphId);
		return;
	}
	if(fWrapper) fWrapper->onPress(sIdent, clickTime, x, y);
	else printError(4,sFile);
}

void SGAKernel::onRelease(const std::string & nodeName, const std::string & sceneGraphId, const float clickTime, const float x, const float y, const std::string & parentNodeName ){

	string pFile;
	FormatWrapperFactory::FormatWrapper * fWrapper;
	
	string sIdent = NodeIndexer::getInstance()->getEngineNodeInputIdentifier(nodeName,sceneGraphId);
	string sFile = NodeIndexer::getInstance()->getEngineNodeInputFile(nodeName,sceneGraphId);
	
	if( !parentNodeName.empty() ){
		pFile = NodeIndexer::getInstance()->getEngineNodeInputFile(parentNodeName,sceneGraphId);
		if( !pFile.empty() && pFile.compare(sFile)!=0 ){
			sIdent = NodeIndexer::getInstance()->getEngineNodeInputIdentifier( parentNodeName,sceneGraphId );
			sFile = pFile;
		}
	}
	fWrapper = getFormatWrappers().getWrapper(sFile);
	if(sIdent.empty()) {
		printError(6,nodeName,sceneGraphId);
		return;
	}
	if(fWrapper) fWrapper->onRelease(sIdent, clickTime, x, y);
	else printError(4,sFile);
}

void SGAKernel::onEnter ( const string &nodeName, const string &engineId, const float touchTime, const float x, const float y, const std::string & parentNodeName ){

	string pFile;
	FormatWrapperFactory::FormatWrapper * fWrapper;

	string sIdent = NodeIndexer::getInstance()->getEngineNodeInputIdentifier(nodeName,engineId);
	string sFile = NodeIndexer::getInstance()->getEngineNodeInputFile(nodeName,engineId);
	if( !parentNodeName.empty() ){
		pFile = NodeIndexer::getInstance()->getEngineNodeInputFile(parentNodeName,engineId);
		if( !pFile.empty() && pFile.compare(sFile)!=0 ){
			sIdent = NodeIndexer::getInstance()->getEngineNodeInputIdentifier( parentNodeName,engineId );
			sFile = pFile;
		}
	}
	fWrapper = getFormatWrappers().getWrapper(sFile);
	if(sIdent.empty()) {
		printError(6,nodeName,engineId);
		return;
	}

	if(fWrapper) fWrapper->onEnter(sIdent, touchTime, x, y);
	else  printError(4,sFile);
}

void SGAKernel::onExit ( const string &nodeName, const string &engineId, const float touchTime, const float x, const float y, const std::string & parentNodeName ){

	string pFile;
	FormatWrapperFactory::FormatWrapper * fWrapper;

	string sIdent = NodeIndexer::getInstance()->getEngineNodeInputIdentifier(nodeName,engineId);
	string sFile = NodeIndexer::getInstance()->getEngineNodeInputFile(nodeName,engineId);
	if( !parentNodeName.empty() ){
		pFile = NodeIndexer::getInstance()->getEngineNodeInputFile(parentNodeName,engineId);
		if( !pFile.empty() && pFile.compare(sFile)!=0 ){
			sIdent = NodeIndexer::getInstance()->getEngineNodeInputIdentifier( parentNodeName,engineId );
			sFile = pFile;
		}
	}
	fWrapper = getFormatWrappers().getWrapper(sFile);
	if(sIdent.empty()) {
		printError(6,nodeName,engineId);
		return;
	}

	if(fWrapper) fWrapper->onExit(sIdent, touchTime, x, y);
	else  printError(4,sFile);
}

string SGAKernel::createTransformNode( const string &sNodeId, const string &sFileName, const std::vector<float> &translation, const std::vector<float> &scale, const std::vector<float> &rotation, const string &sEngineId){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;
	size_t nbEWrapper;
	vector<float> vTrans = vector<float>();
	vector<float> vScale = vector<float>();
	vector<float> vRotation = vector<float>();


	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId, sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ){
					res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createTransformNode(translation,scale,rotation,sEngineNodeId);
					//vector<float> pos = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->getNodePosition(res);
					//static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->getNodeTransform(res,vTrans,vScale,vRotation);
					static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->getCurrentTransform(vTrans,vScale,vRotation);
					nbEWrapper = getEngineWrapper().getWrapperCount();
					for( i=0; i<nbEWrapper; i++){
						eWrapper = getEngineWrapper().getWrapper(i);
						if( eWrapper->getWrapperConfiguration()->getType() == PHYSICS_WRAPPER_TYPE ) static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->setTransform(vTrans,vRotation);
					}
				}
				//if( wConfig->getType() == PHYSICS_WRAPPER_TYPE ) static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->setStartTransform(translation);
				if(!res.empty()) SGATools::addInNodeIndexer( sNodeId, sFileName, res, sEngineId); 
			}
		}
	}
	else{
		nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createTransformNode(sNodeId,sFileName,translation,scale,rotation,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::createGroupNode( const string &sNodeId, const string &sFileName, const string &sEngineId ){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId, sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createGroupNode(sEngineNodeId);
				if(!res.empty()) SGATools::addInNodeIndexer( sNodeId, sFileName, res, sEngineId); 
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createGroupNode(sNodeId,sFileName,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::createPlane( const string &sNodeId, const string &sFileName, const float width, const float height, const std::vector<float> &orientation, const string &sEngineId ){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId,sEngineId);
			wConfig = eWrapper->getWrapperConfiguration();
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createPlane(width,height,orientation,sEngineNodeId);
				if(!res.empty()) SGATools::addInNodeIndexer(sNodeId, sFileName, res, sEngineId);  
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createPlane(sNodeId,sFileName,width,height,orientation,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::createGround( const string &sNodeId, const string &sFileName, const std::vector<float> &normal, const float constant, const std::vector<float> &color, const std::string urlTexture, const string &sEngineId ){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId,sEngineId);
			wConfig = eWrapper->getWrapperConfiguration();
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createGround(sEngineNodeId,normal,constant,color,urlTexture);
				else if( wConfig->getType() == PHYSICS_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->setGround(sEngineNodeId,normal,constant);
				if(!res.empty()) SGATools::addInNodeIndexer(sNodeId, sFileName, res, sEngineId);  
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createGround(sNodeId,sFileName,normal,constant,color,urlTexture,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::createMesh ( 
			const string &sNodeId, 
			const string &sFileName,
			const std::vector<float> &verticesArray,
			const std::vector<float> &normalsArray,
			const std::vector<float> &textureCoordArray,
			const std::vector<float> &coloursArray,
			const std::vector<unsigned short> &facesArray,
			const string &sEngineId
			){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId,sEngineId);
			wConfig = eWrapper->getWrapperConfiguration();
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createMesh(verticesArray,normalsArray,textureCoordArray,coloursArray,facesArray,sEngineNodeId);
				else if( wConfig->getType() == PHYSICS_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->createMesh(verticesArray,sEngineNodeId);
				if(!res.empty()) SGATools::addInNodeIndexer(sNodeId, sFileName, res, sEngineId);  
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createMesh(sNodeId,sFileName,verticesArray,normalsArray,textureCoordArray,coloursArray,facesArray,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::closeGroupNode ( const string &sNodeId, const string &sFileName, const string &sEngineId ){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId;
	int i;
	size_t nbEWrapper;
	vector<float> vTrans = vector<float>();
	vector<float> vScale = vector<float>();
	vector<float> vRotation = vector<float>();

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ){
					static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->closeNode(sEngineNodeId);
					//vector<float> pos = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->getNodePosition(sEngineNodeId);
					//static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->getNodeTransform(sEngineNodeId,vTrans,vScale,vRotation);
					static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->getCurrentTransform(vTrans,vScale,vRotation);
					nbEWrapper = getEngineWrapper().getWrapperCount();
					for( i=0; i<nbEWrapper; i++){
						eWrapper = getEngineWrapper().getWrapper(i);
						if( eWrapper->getWrapperConfiguration()->getType() == PHYSICS_WRAPPER_TYPE ) static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->setTransform(vTrans,vRotation);
					}

				}
			}
		}
	}
	else{
		nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) sEngineNodeId = closeGroupNode(sNodeId,sFileName,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return sEngineNodeId;
}

string SGAKernel::createViewpointNode ( const string &sNodeId, const string &sFileName, const std::vector<float> &position, const std::vector<float> &orientation, const string &sEngineId ){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId, sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createViewpoint(position,orientation,sEngineNodeId);
				if(!res.empty()) SGATools::addInNodeIndexer( sNodeId, sFileName, res, sEngineId);  
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createViewpointNode(sNodeId,sFileName,position,orientation,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::createBoxShape ( const string &sNodeId, const string &sFileName, const std::vector<float> &size, const string &sEngineId){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createBox(size,sEngineNodeId);
				else if( wConfig->getType() == PHYSICS_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->createBox(size,sEngineNodeId);
				if(!res.empty()) SGATools::addInNodeIndexer( sNodeId, sFileName, res, sEngineId);  
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createBoxShape(sNodeId,sFileName,size,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::createSphereShape ( const string &sNodeId, const string &sFileName, float radius, const string &sEngineId ){
	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createSphere(radius,sEngineNodeId);
				else if( wConfig->getType() == PHYSICS_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->createSphere(radius,sEngineNodeId);
				if(!res.empty()) SGATools::addInNodeIndexer( sNodeId, sFileName, res, sEngineId);  
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createSphereShape(sNodeId,sFileName,radius,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::createCylinderShape ( const string &sNodeId, const string &sFileName, float radius, float height, const string &sEngineId ){
	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createCylinder(radius,height,sEngineNodeId);
				else if( wConfig->getType() == PHYSICS_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->createCylinder(radius,height,sEngineNodeId);
				if(!res.empty()) SGATools::addInNodeIndexer( sNodeId, sFileName, res, sEngineId);  
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createCylinderShape(sNodeId,sFileName,radius,height,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::createMaterialProperty ( const string sNodeId, const string sFileName, const float ambientIntensity, const float shininess, const float alpha, const std::vector<float> &diffuseColor, const std::vector<float> &emissiveColor, const std::vector<float> &specularColor, const string sEngineId){
	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createMaterial(ambientIntensity,shininess,alpha,diffuseColor,emissiveColor,specularColor,sEngineNodeId);
				if(!res.empty()) SGATools::addInNodeIndexer( sNodeId, sFileName, res, sEngineId);  
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createMaterialProperty(sNodeId,sFileName,ambientIntensity,shininess,alpha,diffuseColor,emissiveColor,specularColor,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

string SGAKernel::createTextureProperty ( const string &sNodeId, const string &sFileName, const string &sTexture, const string &sEngineId ){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId, res;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createTexture(sTexture,sEngineNodeId);
				if(!res.empty()) SGATools::addInNodeIndexer( sNodeId, sFileName, res, sEngineId);  
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) res = createTextureProperty(sNodeId,sFileName,sTexture,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
	return res;
}

void SGAKernel::attachShapeToCurrentGroupNode ( const string &sNodeId, const string &sFileName, const string &sEngineId){
	
	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->attachShapeToSceneGraph(sEngineNodeId);
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) attachShapeToCurrentGroupNode(sNodeId,sFileName,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
}

void SGAKernel::setTransformNode( const string &sNodeId, const string &sFileName, const std::vector<float> &translation, const std::vector<float> &rotation, const std::vector<float> &scale, const TransformSpace relativeTo,  const string &sEngineId){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->setTransformNode(sEngineNodeId,translation,scale,rotation);
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) setTransformNode(sNodeId,sFileName,translation,rotation,scale,relativeTo,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
}


void SGAKernel::setMaterialProperty (const string sNodeId, const string sFileName, const float ambientIntensity, const float shininess, const float alpha, const std::vector<float> &diffuseColor, const std::vector<float> &emissiveColor, const std::vector<float> &specularColor, const string sEngineId){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId;
	int i;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE )  static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->setMaterial(sEngineNodeId,ambientIntensity,shininess,alpha,diffuseColor,emissiveColor,specularColor);
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) setMaterialProperty(sNodeId,sFileName,ambientIntensity,shininess,alpha,diffuseColor,emissiveColor,specularColor,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
}

void SGAKernel::deleteNode ( const string &sNodeId, const string &sFileName, const string &sEngineId ){
	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId;
	int i;
	vector<string> deletedNodes = vector<string>();
	vector<string>::iterator it;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) deletedNodes = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->deleteNode(sEngineNodeId);
				//for(vector<string>::iterator it = sNameArray.begin(); it != sNameArray.end();++it){
				for(it = deletedNodes.begin(); it != deletedNodes.end(); ++it){
					SGATools::deleteInNodeIndexer( *it,sEngineId,false);
				}
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) deleteNode(sNodeId,sFileName,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
}

void SGAKernel::deleteNodeChildren ( const string &sNodeId, const string &sFileName, const string &sEngineId ){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId;
	int i;
	vector<string> deletedNodes = vector<string>();
	vector<string>::iterator it;

	if( !sEngineId.empty() ){
		eWrapper = getEngineWrapper().getWrapper(sEngineId);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,sEngineId);
			if(!sEngineNodeId.empty()){
				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) deletedNodes = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->deleteNodeChildren(sEngineNodeId);
				for(it = deletedNodes.begin(); it != deletedNodes.end(); ++it) SGATools::deleteInNodeIndexer( *it,sEngineId,false);
			}
		}
	}
	else{
		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
		for( i=0; i<nbEWrapper; i++){
			eWrapper = getEngineWrapper().getWrapper(i);
			if(eWrapper) deleteNodeChildren(sNodeId,sFileName,eWrapper->getWrapperConfiguration()->getIdentifier());
		}
	}
}

void SGAKernel::updateNodePosition( const string &sNodeId, const string &sEngineId, const std::vector<float> position, const std::vector<float> rotation){

	FormatWrapperFactory::FormatWrapper * fWrapper;
	WrapperConfiguration * wConfig;
	string sFormatNodeId = string();
	string sInputFile = string();
	int i;

	sFormatNodeId = NodeIndexer::getInstance()->getEngineNodeInputIdentifier(sNodeId,sEngineId);
	sInputFile = NodeIndexer::getInstance()->getEngineNodeInputFile(sNodeId,sEngineId);

	if(!sInputFile.empty() && !sFormatNodeId.empty()){
		fWrapper = getFormatWrappers().getWrapper(sInputFile);
		fWrapper->updateNodePosition(sFormatNodeId,position,rotation);
	}
}

void SGAKernel::setNodePhysicsProperties ( const string &sNodeId, const string &sFileName, const float fMass, const float fRestitution, const float fFriction, const bool isDynamic){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId;
	int i;

	vector<float> vTrans = vector<float>();
	vector<float> vScale = vector<float>();
	vector<float> vRotation = vector<float>();

	size_t nbEWrapper = getEngineWrapper().getWrapperCount();
	for( i=0; i<nbEWrapper; i++){
		eWrapper = getEngineWrapper().getWrapper(i);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			/*if( wConfig->getType() == PHYSICS_WRAPPER_TYPE ){
				sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,wConfig->getIdentifier());
				static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->setNodePhysicsProperties(sEngineNodeId,translation,rotation,fMass,fRestitution,fFriction,isDynamic);
			}*/
			if( wConfig->getType() == RENDERER_WRAPPER_TYPE ){
				sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,wConfig->getIdentifier());
				static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->getNodeTransform(sEngineNodeId,vTrans,vScale,vRotation);
				nbEWrapper = getEngineWrapper().getWrapperCount();
				for( i=0; i<nbEWrapper; i++){
					eWrapper = getEngineWrapper().getWrapper(i);
					if( eWrapper->getWrapperConfiguration()->getType() == PHYSICS_WRAPPER_TYPE ) {
						sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,eWrapper->getWrapperConfiguration()->getIdentifier());
						static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->setNodePhysicsProperties(sEngineNodeId,vTrans,vRotation,fMass,fRestitution,fFriction,isDynamic);
						break;
					}
				}
				break;
			}
		}
	}
}

void SGAKernel::setNodeMass ( const string &sNodeId, const string &sFileName, const float fMass ){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId;
	int i;

	vector<float> vTrans = vector<float>();
	vector<float> vScale = vector<float>();
	vector<float> vRotation = vector<float>();

	size_t nbEWrapper = getEngineWrapper().getWrapperCount();
	for( i=0; i<nbEWrapper; i++){
		eWrapper = getEngineWrapper().getWrapper(i);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			if( wConfig->getType() == RENDERER_WRAPPER_TYPE ){
				sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,wConfig->getIdentifier());
				static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->getNodeTransform(sEngineNodeId,vTrans,vScale,vRotation);
				nbEWrapper = getEngineWrapper().getWrapperCount();
				for( i=0; i<nbEWrapper; i++){
					eWrapper = getEngineWrapper().getWrapper(i);
					if( eWrapper->getWrapperConfiguration()->getType() == PHYSICS_WRAPPER_TYPE ) {
						sEngineNodeId = NodeIndexer::getInstance()->getFormatNodeOutputIdentifier(sNodeId,sFileName,eWrapper->getWrapperConfiguration()->getIdentifier());
						static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->setNodeMass(sEngineNodeId,fMass,vTrans,vRotation);
						break;
					}
				}
				break;
			}
		}
	}
}

void SGAKernel::setWorldPhysicsProperties ( const std::vector<float> &vGravity, const float &fTimeStep ){

	EngineWrapperFactory::EngineWrapper * eWrapper;
	WrapperConfiguration * wConfig;
	string sEngineNodeId;
	int i;

	size_t nbEWrapper = getEngineWrapper().getWrapperCount();
	for( i=0; i<nbEWrapper; i++){
		eWrapper = getEngineWrapper().getWrapper(i);
		if(eWrapper){
			wConfig = eWrapper->getWrapperConfiguration();
			if( wConfig->getType() == PHYSICS_WRAPPER_TYPE ){
				static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->setGravity(vGravity);
				static_cast<EngineWrapperFactory::PhysicsWrapper*>(eWrapper)->setTimeStep(fTimeStep);
			}
		}
	}
}

std::string SGAKernel::getNodeFieldValue ( const std::string scenegraphId, const std::string &nodeId,  const string &fieldId ){

	std::string res;

	FormatWrapperFactory::FormatWrapper * fWrapper;
	EngineWrapperFactory::EngineWrapper * eWrapper;

	fWrapper = getFormatWrappers().getWrapper(scenegraphId);
	if(fWrapper) res = fWrapper->getNodeFieldValue(nodeId,fieldId);
	/*else{
		eWrapper = getEngineWrapper().getWrapper(scenegraphId);
		if(eWrapper) res = eWrapper->getNodeFieldValue(nodeId,fieldId);
	}*/
	return res;
}

void SGAKernel::matchNodeFieldValue( const std::string &fromNodeId, const std::string fromScenegraphId, const std::string &toNodeId, const std::string &toNodeFileName, const std::vector<std::string> fields ){

	FormatWrapperFactory::FormatWrapper * fWrapper;
	EngineWrapperFactory::EngineWrapper * eWrapper;

	fWrapper = getFormatWrappers().getWrapper(fromScenegraphId);
	if(fWrapper) fWrapper->matchNodeFieldValue(fromNodeId,toNodeId,toNodeFileName,fields);
}

void SGAKernel::setNodeFieldValue( const std::string scenegraphId, const std::string &nodeId, const string &fieldId, const string &sValue ){
	
	FormatWrapperFactory::FormatWrapper * fWrapper;
	fWrapper = getFormatWrappers().getWrapper(scenegraphId);
	if(fWrapper) fWrapper->setNodeFieldValue(nodeId,fieldId,sValue);

}
