#include "NodeIndexer.h"

#include "SGATools.h"

using namespace SceneGraphAdapter;

bool NodeIndexer::instanceFlag = false;

NodeIndexer * NodeIndexer::nodeIndexInstance = NULL;

NodeIndexer * NodeIndexer::getInstance(){
	if(!instanceFlag){
		nodeIndexInstance = new NodeIndexer();
		instanceFlag = true;
		return nodeIndexInstance;
	}
	else{
		return nodeIndexInstance;
	}
}

void NodeIndexer::addNode( const string &sFormatId, const string &sFileName, const string &sEngineId,  const string &sEngine){

	m_EngineToFormatMap.insert(
		NodeIndexer::EngineToFormatMap::value_type( NodeIdentifier(sEngineId,sEngine) , NodeIdentifier(sFormatId,sFileName) ));

	m_FormatToEngineMap.insert(
		NodeIndexer::FormatToEngineMap::value_type( NodeIdentifier(sFormatId,sFileName) , NodeIdentifier(sEngineId,sEngine) ));
}

int NodeIndexer::deleteNode( const string &sId , const string &sSceneGraphId, bool isFormatNode ){

	if(isFormatNode){
		if(m_FormatToEngineMap.find(NodeIdentifier(sId,sSceneGraphId)) == m_FormatToEngineMap.end()){
			printf("NodeIndexer::deleteNode => Format Scene Graph Node: (\"%s\",\"%s\") not found in NodeIndexer\n",sId.c_str(),sSceneGraphId.c_str());
			return 0;
		}
		FormatToEngineMap::iterator it;
		it = m_FormatToEngineMap.find(NodeIdentifier(sId,sSceneGraphId));
		NodeIdentifier res = it->second;
		m_FormatToEngineMap.erase( NodeIdentifier(sId,sSceneGraphId) );
		m_EngineToFormatMap.erase( res );
		return 1;
	}
	else{
		
		if(m_EngineToFormatMap.find(NodeIdentifier(sId,sSceneGraphId)) == m_EngineToFormatMap.end()){
			printf("NodeIndexer::deleteNode => Node: (\"%s\",\"%s\") not found in NodeIndexer\n",sId.c_str(),sSceneGraphId.c_str());
			return 0;
		}
		EngineToFormatMap::iterator it;
		it = m_EngineToFormatMap.find(NodeIdentifier(sId,sSceneGraphId));
		NodeIdentifier res = it->second;
		m_EngineToFormatMap.erase( NodeIdentifier(sId,sSceneGraphId) );
		m_FormatToEngineMap.erase( res );
		return 1;
	}
}

void NodeIndexer::setNode ( const string &sNodeId, const string &sSceneGraphId ){
	/*if(m_nodeIndexMap.find(sNodeId) == m_nodeIndexMap.end()){
		printf("NodeIndexer => Node: \"%s\" not found in NodeIndexer\n",sNodeId.c_str());
		return;
	}
	m_nodeIndexMap.find(sNodeId)->second.setFileName(sFileName);
	if( !sIdentifier.empty() ) m_nodeIndexMap.find(sNodeId)->second.setIdentifier(sIdentifier);*/
}

string NodeIndexer::getEngineNodeInputFile( const string &sEngineNodeId , const string &sEngineId ) {

	EngineToFormatMap::iterator it;
	it = m_EngineToFormatMap.find(NodeIdentifier(sEngineNodeId,sEngineId));
	if( it == m_EngineToFormatMap.end() ) return "";
	else return it->second.second;
}


string NodeIndexer::getEngineNodeInputIdentifier( const string &sEngineNodeId , const string &sEngineId ) {
	EngineToFormatMap::iterator it;
	it = m_EngineToFormatMap.find(NodeIdentifier(sEngineNodeId,sEngineId));
	if( it == m_EngineToFormatMap.end() ) return "";
	else return it->second.first;
}

string NodeIndexer::getFormatNodeOutputIdentifier( const string &sFormatNodeId , const string &sInputFile, const string &sEngineId ){

	FormatToEngineMap::iterator it;
	pair< FormatToEngineMap::iterator , FormatToEngineMap::iterator > ret;
	ret = m_FormatToEngineMap.equal_range( NodeIdentifier(sFormatNodeId,sInputFile) );

	NodeIdentifier n;
	for (it=ret.first; it!=ret.second; ++it){
		n = it->second;
		if( n.second.compare(sEngineId) == 0 ) return n.first;
	}
	return "";
}




