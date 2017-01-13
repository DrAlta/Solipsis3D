#include "3dcWrapper.h"

const unsigned int NUM_INDENTS_PER_SPACE=2;

const char * _3dcWrapper::getIndent( unsigned int numIndents ){

	static const char * pINDENT="                                      + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

std::string _3dcWrapper::searchReferencedFilesMap( std::string sDef ){

	std::string res;
	std::map<std::string,std::string>::iterator it;
	it = _referencedFilesMap.find(sDef);
	if( it == _referencedFilesMap.end() ) res=std::string();
	else res = it->second;
	return res;
}

std::string _3dcWrapper::getNodeName( TiXmlElement* pElement ){

	std::string res = "";
	const char * nName = NULL;
	nName = pElement->Attribute("DEF");
	if( nName && std::string(nName)!="") {
		res = string(nName);
	}
	else{
		const char * cc = pElement->Value();
		if( std::string(cc)==SGA_3DC_GROUP_NODE ){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::GroupNode);
		}
		else if( std::string(cc)==SGA_3DC_TRANSFORM_NODE ){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::TransformNode);
		}
	}
	_nodeIdx++;
	return res;
}

void _3dcWrapper::processTransform( TiXmlNode* pParent ){

	std::vector<float> translation (3,0.0);
	std::vector<float> scale (3,1.0);
	std::vector<float> rotation (4,0.0);
	std::string nodeId;

	std::string sTrans = string();
	std::string sRot = string();
	std::string sScale = string();

	const char * cTrans = pParent->ToElement()->Attribute(SGA_3DC_TRANSLATION_FIELD);
	const char * cRot = pParent->ToElement()->Attribute(SGA_3DC_ROTATION_FIELD);
	const char * cScale = pParent->ToElement()->Attribute(SGA_3DC_SCALE_FIELD);

	if(cTrans!=NULL ) sTrans = string(cTrans);
	if(cRot!=NULL )  sRot = string(cRot);
	if(cScale!=NULL )  sScale = string(cScale);

	std::string sValue;
	float fValue;
	std::istringstream iss;

	if(!sTrans.empty()){
		translation.clear();
		iss.str(sTrans);
		while( iss >> fValue ) translation.push_back(fValue);
	}
	if(!sRot.empty()){
		rotation.clear();
		iss.clear();
		iss.str(sRot);
		while( iss >> fValue ) rotation.push_back(fValue);
	}
	if(!sScale.empty()){
		scale.clear();
		iss.clear();
		iss.str(sScale);
		while( iss >> fValue ) scale.push_back(fValue);
	}
	nodeId = getNodeName( pParent->ToElement() );
	_kernel->createTransformNode(nodeId,c_fileName,translation,scale,rotation);

}
//std::vector< std::string > tokenizeNodeDescriptor ( std::string sNodeDescriptor,  char cDelimiter = ' ')

std::string _3dcWrapper::toString( std::vector<float> fValue ){
	
	std::stringstream ss;
	int vSize = fValue.size();
	for( int i = 0; i<vSize; i++){
		ss<<fValue.at(i)<<' ';
	}
	return ss.str();

}

std::vector< float > _3dcWrapper::tokenizeToFloat ( std::string sValue,  char cDelimiter){

	std::vector<float> res;
	float buf;
	std::stringstream ss(sValue);

	if( cDelimiter==' ' ) while (ss >> buf) res.push_back(buf);
	//else while(std::getline(ss, buf, cDelimiter)) res.push_back(buf);
	return res;
}

std::vector< std::string > _3dcWrapper::tokenizeNodeDescriptor ( std::string sNodeDescriptor,  char cDelimiter ){

	std::vector<string> res;
	std::string buf;
	std::stringstream ss(sNodeDescriptor);

	if( cDelimiter==' ' ) while (ss >> buf) res.push_back(buf);
	else while(std::getline(ss, buf, cDelimiter)) res.push_back(buf);
	return res;
}

void _3dcWrapper::processMatch( TiXmlNode* pParent ){

	std::string sFromNode;
	std::string sToNode;
	std::string sFields;

	const char * cFromNode = pParent->ToElement()->Attribute(SGA_3DC_FROMNODE_FIELD);
	const char * cToNode = pParent->ToElement()->Attribute(SGA_3DC_TONODE_FIELD);
	const char * cFields = pParent->ToElement()->Attribute(SGA_3DC_FIELDS_FIELD);

	if( cFromNode && cToNode && cFields){
		
		sFromNode = string(cFromNode);
		sToNode = string(cToNode);
		sFields = string(cFields);

		std::vector<std::string> tokenizedFromNode = tokenizeNodeDescriptor(sFromNode,'.');
		std::vector<std::string> tokenizedToNode = tokenizeNodeDescriptor(sToNode,'.');
		std::vector<std::string> tokenizedFields = tokenizeNodeDescriptor(sFields,' ');

		std::string sFromFileUrl = searchReferencedFilesMap( tokenizedFromNode.at(0) );
		std::string sToFileUrl = searchReferencedFilesMap( tokenizedToNode.at(0) );

		_kernel->matchNodeFieldValue(tokenizedFromNode.at(1),sFromFileUrl,tokenizedToNode.at(1),sToFileUrl,tokenizedFields);
	}

}

void _3dcWrapper::processRoute( TiXmlNode* pParent ){
	
	const char * cFromNode = pParent->ToElement()->Attribute(SGA_3DC_FROMFIELD_FIELD);
	const char * cToNode = pParent->ToElement()->Attribute(SGA_3DC_TOFIELD_FIELD);
	const char * cConvertor = pParent->ToElement()->Attribute(SGA_3DC_CONVERSION_FIELD);

	if( cFromNode && cToNode){
		std::string sFromNode = string();
		std::string sToNode = string();

		if(cFromNode!=NULL ) sFromNode = string(cFromNode);
		if(cToNode!=NULL )  sToNode = string(cToNode);

		std::vector<std::string> tokenizedFromNode = tokenizeNodeDescriptor(sFromNode,'.');
		std::vector<std::string> tokenizedToNode = tokenizeNodeDescriptor(sToNode,'.');


		std::string sFromFileUrl = searchReferencedFilesMap( tokenizedFromNode.at(0) );
		std::string sToFileUrl = searchReferencedFilesMap( tokenizedToNode.at(0) );

		std::string sFieldValue = _kernel->getNodeFieldValue( sFromFileUrl, tokenizedFromNode.at(1), tokenizedFromNode.at(2) );
		if(cConvertor){

			sFieldValue = convert(sFieldValue,string(cConvertor));
		}
		_kernel->setNodeFieldValue( sToFileUrl, tokenizedToNode.at(1), tokenizedToNode.at(2), sFieldValue );

		_routeManager.addRoute(sFromFileUrl,tokenizedFromNode.at(1),tokenizedFromNode.at(2),sToFileUrl,tokenizedToNode.at(1),tokenizedToNode.at(2),string(cConvertor));
	}

	
}

std::string _3dcWrapper::convert( std::string value, std::string convertorName){

	std::string res;
	if(convertorName.compare(SGA_3DC_CONVERTOR_VRTQ)==0){
		vector<float> vRot = tokenizeToFloat(value);
		vector<float> vRotQ = _3dcWrapperConversion::axis_angleToQuaternion(vRot);
		res = toString(vRotQ);
	}

	return res;
}



void _3dcWrapper::processContent( TiXmlNode* pParent ){

	const char * cDef = pParent->ToElement()->Attribute(SGA_3DC_DEF_FIELD);
	const char * fileUrl = pParent->ToElement()->Attribute(SGA_3DC_URL_FIELD);

	/*const char buffer_1[ ] = pParent->ToElement()->Attribute(SGA_3DC_URL_FIELD);
	const char *lpStr1;
	lpStr1 = buffer_1;*/
	
	std::string sFullFilePath;
	if( PathIsRelative(fileUrl) ){
		sFullFilePath = c_filePath + string(fileUrl);
	}
	else sFullFilePath = fileUrl;

	if( cDef && std::string(cDef)!="") {
		_referencedFilesMap.insert( std::pair<std::string,std::string>( std::string(cDef), sFullFilePath ) );
	}

	_kernel->loadFile(sFullFilePath,c_filePath);
	//_kernel->loadFile(fileUrl,c_filePath);
}

void _3dcWrapper::processElement ( TiXmlNode* pParent ){

	const char * s =	pParent->Value();
	std::string nodeId;  
	if(string(s)==SGA_3DC_GROUP_NODE){
		nodeId = getNodeName( pParent->ToElement() );
		_kernel->createGroupNode(nodeId,c_fileName);
	}
	else if ( string(s)==SGA_3DC_TRANSFORM_NODE){
		nodeId = getNodeName( pParent->ToElement() );
		processTransform(pParent);
	}
	else if ( string(s)==SGA_3DC_CONTENT_NODE ){
		processContent( pParent );
	}
	else if ( string(s)==SGA_3DC_ROUTE_NODE ){
		processRoute( pParent );
	}
	else if ( string(s)==SGA_3DC_MATCH_NODE ){
		processMatch( pParent );
	}
}

void  _3dcWrapper::processFile ( TiXmlNode* pParent, unsigned int indent ){
	
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;

	switch ( t ){

		case TiXmlNode::TINYXML_DOCUMENT:
		printf( "Document" );
		break;

		case TiXmlNode::TINYXML_ELEMENT:
		printf( "Element [%s]", pParent->Value() );
		processElement(pParent);
		break;

	case TiXmlNode::TINYXML_COMMENT:
		printf( "Comment: [%s]", pParent->Value());
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		printf( "Unknown" );
		break;

	case TiXmlNode::TINYXML_TEXT:
		pText = pParent->ToText();
		printf( "Text: [%s]", pText->Value() );
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		printf( "Declaration" );
		break;
	default:
		break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		processFile( pChild, indent+1 );
	}
}

int _3dcWrapper::adaptFormat( const string & fileName, const string & filePath, SGAKernel * sgaKernel){
	
	c_fileName = fileName;
	TiXmlDocument doc( c_fileName.c_str() );
	bool loadOkay = doc.LoadFile();
	if (loadOkay){
		_kernel = sgaKernel;

		size_t lastSlashFile = fileName.find_last_of("\\") + 1;
		size_t lastSlashPath = filePath.find_last_of("\\") + 1;
		c_filePath = filePath.substr(0,lastSlashPath) + fileName.substr(0,lastSlashFile);
		//c_rendererId = sRendererIdentifier;
		printf("\n%s:\n", c_fileName.c_str());
		processFile( &doc );
		return 1;
	}
	else{
		printf("SceneGraphAdapter::SGA3DContainerDecoder::loadFile Failed to load file \"%s\"\n", c_fileName.c_str());
		return 0;
	}
	
}

void _3dcWrapper::updateElapsedTime( const float elapsedTime ){

	int nbRoute = _routeManager.getNbRoutes();
	int i = 0;
	for(i=0; i<nbRoute; i++){
		WRoute curRoute = _routeManager.getRoutes(i);
		std::string sFieldValue = _kernel->getNodeFieldValue( curRoute.fromFileName, curRoute.fromNodeName, curRoute.fromFieldName);
		if(_routeManager.needConversion(curRoute)){
			sFieldValue = convert(sFieldValue,curRoute.convertor);
		}
		if(!sFieldValue.empty()) _kernel->setNodeFieldValue( curRoute.toFileName, curRoute.toNodeName , curRoute.toFieldName , sFieldValue );
	}
}


extern "C" _3DC_WRAPPER_API size_t registerPlugin(SGAKernel &wf) {
	wf.getFormatWrappers().addWrapper(
		auto_ptr<FormatWrapperFactory::FormatWrapper> ( new _3dcWrapper(wf.getCurrentFileName()) )
		);
	return wf.getFormatWrappers().getWrapperCount()-1;
}

extern "C" _3DC_WRAPPER_API WrapperConfiguration *  queryPlugin(SGAKernel &wf){
	_3dcWrapper * xWrapper = new _3dcWrapper();
	return xWrapper->getWrapperConfiguration();
}