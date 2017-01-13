#include "SceneGraphAdapter.h"
#include "SGAKernel.h"

//using namespace SceneGraphAdapter;

//string FormatSceneGraphAdapter::createGroupNode( const string &sNodeId, const string &sFileName, const string &sEngineId ){
//
//	EngineWrapperFactory::EngineWrapper * eWrapper;
//	WrapperConfiguration * wConfig;
//	string sEngineNodeId, res;
//	int i;
//
//	/*if( !sEngineId.empty() ){
//		eWrapper = getEngineWrapper().getWrapper(sEngineId);
//		if(eWrapper){
//			wConfig = eWrapper->getWrapperConfiguration();
//			sEngineNodeId = SGATools::getUniqueEngineNodeId(sFileName,sNodeId, sEngineId);
//			if(!sEngineNodeId.empty()){
//				if( wConfig->getType() == RENDERER_WRAPPER_TYPE ) res = static_cast<EngineWrapperFactory::RendererWrapper*>(eWrapper)->createGroupNode(sEngineNodeId);
//				if(!res.empty()) SGATools::addInNodeIndexer( sNodeId, sFileName, res, sEngineId); 
//			}
//		}
//	}
//	else{
//		size_t nbEWrapper = getEngineWrapper().getWrapperCount();
//		for( i=0; i<nbEWrapper; i++){
//			eWrapper = getEngineWrapper().getWrapper(i);
//			if(eWrapper) res = createGroupNode(sNodeId,sFileName,eWrapper->getWrapperConfiguration()->getIdentifier());
//		}
//	}*/
//	return res;
//}