#ifndef __3dcWrapper_h__
#define __3dcWrapper_h__

#include "3dcWrapperPrerequisites.h"
#include "3dcWrapperConversion.h"
#include <string>
#include <vector>
#include "SGAKernel.h"
#include "tinyxml.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "RouteManager.h"
#include <windows.h>
#include "Shlwapi.h"

using namespace SceneGraphAdapter;

#define SGA_3DC_GROUP_NODE "Group"
#define SGA_3DC_TRANSFORM_NODE "Transform"
#define SGA_3DC_CONTENT_NODE "Content"
#define SGA_3DC_ROUTE_NODE "ROUTE"
#define SGA_3DC_PATCH_NODE "Patch"
#define SGA_3DC_MATCH_NODE "Match"

#define SGA_3DC_TRANSLATION_FIELD "translation"
#define SGA_3DC_ROTATION_FIELD "rotation"
#define SGA_3DC_SCALE_FIELD "scale"
#define SGA_3DC_FROMFIELD_FIELD "fromField"
#define SGA_3DC_TOFIELD_FIELD "toField"
#define SGA_3DC_URL_FIELD "url"
#define SGA_3DC_FROMNODE_FIELD "fromNode"
#define SGA_3DC_TONODE_FIELD "toNode"
#define SGA_3DC_FIELDS_FIELD "fields"
#define SGA_3DC_DEF_FIELD "DEF"
#define SGA_3DC_CONVERSION_FIELD "conversion"

#define SGA_3DC_CONVERTOR_VRTQ "VectRotToQuaternion"


class _3DC_WRAPPER_API _3dcWrapper : public FormatWrapperFactory::FormatWrapper {

public:

	_3dcWrapper(){
		initialize();
		c_fileName = std::string();
		m_configuration->setExtension(INPUT_FILE_EXTENSION);
		m_configuration->setVersion(WRAPPER_VERSION);
	}
	_3dcWrapper( const string &sFileName ){
		initialize();
		c_fileName = sFileName;
		m_configuration->setExtension(INPUT_FILE_EXTENSION);
		m_configuration->setIdentifier(sFileName);
		m_configuration->setVersion(WRAPPER_VERSION);
	}

	~_3dcWrapper(){}

	WrapperConfiguration * getWrapperConfiguration(){
		return m_configuration;
	}

	int adaptFormat( const string & fileName, const string & filePath, SGAKernel * sgaKernel);
	int adaptSceneGraph( SGAKernel * sgaKernel, const string &nodeId = string() ){ return 1;}
	void updateElapsedTime( const float elapsedTime );
	void onPress ( const string &nodeId, const float time, const float x, const float y ){};
	void onRelease ( const string &nodeId, const float time, const float x, const float y ){};
	void onEnter ( const string &nodeId, const float time, const float x, const float y ){};
	void onExit ( const string &nodeId, const float time, const float x, const float y){};
	void onDrag ( const string &nodeId, const float time, const float x, const float y ){};
	void updateNodePosition( const string &nodeId, const vector<float> position, const std::vector<float> rotation ){};
	std::string getNodeFieldValue ( const std::string &nodeId,  const string &fieldId ){ return ""; };
	void setNodeFieldValue( const std::string &nodeId, const string &fieldId, const string &sValue ){};
	void matchNodeFieldValue( const std::string &fromNodeId, const std::string &toNodeId, const std::string &toNodeFileName, const std::vector<std::string> fields ){};

private:

	/**
	* @fn initialize()
	* @brief initialize a 3dcWrapper object
	*/
	void initialize(){
		_nodeIdx = 0;
		_referencedFilesMap  = FilesDefMap();
		//c_containedFilesArray = std::vector< std::string >();
	}

	/**
	* @fn clean()
	* @brief clean 3dcWrapper object
	*/
	void clean(){
		//c_containedFilesArray.clear();
		_referencedFilesMap.clear();
	}

	std::string getNodeName( TiXmlElement* pElement );
	const char * getIndent( unsigned int numIndents );
	void processTransform( TiXmlNode* pParent );
	void processContent( TiXmlNode* pParent );
	void processElement ( TiXmlNode* pParent );
	void processFile ( TiXmlNode* pParent, unsigned int indent = 0 );
	void processRoute( TiXmlNode* pParent );
	void processMatch( TiXmlNode* pParent );
	std::string convert( std::string value, std::string convertorName);

	std::string toString( std::vector<float> fValue );
	std::vector< float > tokenizeToFloat ( std::string sValue,  char cDelimiter = ' ' );
	std::vector< std::string > tokenizeNodeDescriptor ( std::string sNodeDescriptor,  char cDelimiter = ' ');
	std::string searchReferencedFilesMap( std::string sDef );

private:

	SGAKernel * _kernel;
	int _nodeIdx;
	std::string	c_fileName; ///<  the input file name to decode
	std::string c_filePath;
	//std::string c_rendererId;
	//std::vector< std::string > c_containedFilesArray; ///<  an array containing all files url linked in the 3dc file
	/** \typedef std::map< std::string, std::string > FilesDefMap 
	*	WrapperMap.first =  file DEF identifier ( ex: "File_1")
	*	WrapperMap.second = file url ( ex: "..\test\dice.dae" )
	*/
	typedef std::map< std::string, std::string > FilesDefMap; 
	FilesDefMap _referencedFilesMap; ///< a map that gives a file url linked in a "Content" tag given its DEF field 
	RouteManager _routeManager;

};

#endif // #ifndef __3dcWrapper_h__