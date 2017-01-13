/**
* \file WrapperConfiguration.h
* \brief The WrapperConfiguration class is an object contain in every Wrapper that keep basic information about a wrapper
* ( its type, its version, ...)
* Details about WrapperConfiguration.h
*/
#ifndef _WRAPPERCONFIGURATION_H
#define _WRAPPERCONFIGURATION_H

#include <string>
#include "SceneGraphAdapter.h"
//#include "SGAKernel.h"

#define		FORMAT_WRAPPER_TYPE		"FormatWrapper"
#define		RENDERER_WRAPPER_TYPE	"RendererWrapper"
#define		PHYSICS_WRAPPER_TYPE	"PhysicsWrapper"
#define		UNKNOWN_WRAPPER_TYPE	"UnknownType"

/**
* The WrapperConfiguration class is an object contain in every Wrapper that keep basic information about a wrapper
* ( its type, its version, ...)
* @class WrapperConfiguration
* @author Rozenn Bouville Berthelot
* @date 12/01/2011
* @version 0.0
*/

using namespace std;

namespace SceneGraphAdapter {

	class WrapperConfiguration {

	public:
		
		/**
		* @fn WrapperConfiguration() 
		* Default constructor
		*/
		WrapperConfiguration(){
			initialize();
		}

		/**
		* @fn WrapperConfiguration( const std::string sIdentifier = string(), const std::string sType = "UnknownType", const string sExt = string(), const float fVersion = 0.0)
		* @param fVersion an int that give the version number of the wrapper
		* @param sType = { "FormatWrapper", "RendererWrapper", "PhysicsWrapper", "UnknownType" }
		* @param sIdentifier a string that identify the wrapper; the input file name for a Format Wrapper ( ex: "..\media\duck.dae" )
		* or the name of the engine adapted by the wrapper ( "ogre", "unity", "physix", ...)
		* @param sExt a string that is the input file extension for format wrappers ( "x3d", "dae", ... )
		*/
		WrapperConfiguration( const std::string sIdentifier = string(), const std::string sType = "UnknownType", const string sExt = string(), const float fVersion = 0.0){
			initialize(sIdentifier,sType,sExt,fVersion);
		}

		/**
		* @fn WrapperConfiguration( const WrapperConfiguration &wConf ) 
		* @brief Copy constructor
		*/
		WrapperConfiguration( const WrapperConfiguration &wConf ){
			copy(wConf);
		}

		/**
		* @fn ~WrapperConfiguration() Destructor
		*/
		~WrapperConfiguration(){
			clean();
		}


		void	setType(string sType)		{ this->m_type = sType; }			///< Setter for m_type 
		void	setIdentifier(string sIdent){ this->m_identifier = sIdent; }	///< Setter for m_identifier 	
		void	setExtension(string sExt)	{ this->m_extension = sExt; }		///< Setter for m_extension 
		void	setVersion(float fVersion)	{ this->m_version = fVersion; }		///< Setter for m_version 
	

		float	getVersion() const { return this->m_version; }			///< Getter for m_version 
		string	getType() const { return this->m_type; }			///< Getter for m_type 	
		string	getIdentifier() const { return this->m_identifier; }		///< Getter for m_identifier 
		string	getExtension() const { return this->m_extension; }		///< Getter for m_extension 

	private:

		/**
		* @fn void initialize( std::string sType = "UnknownType", std::string sIdentifier = string(), string sExt = string(), float fVersion = 0.0 )
		* @brief initialize WrapperConfiguration variables
		* @param sType = { "FormatWrapper", "RendererWrapper", "PhysicsWrapper", "UnknownType" }
		* @param sIdentifier a string that identify the wrapper; the input file name for a Format Wrapper ( ex: duck.dae )
		* or the name of the engine adapted by the wrapper ( ogre, unity, physics, ...)
		* @param sExt a string that is the input file extension for format wrappers ( x3d, dae, ... )
		* @param fVersion an int that give the version number of the wrapper
		*/
		void initialize(  const std::string sIdentifier = string(), const std::string sType = "UnknownType", const string sExt = string(), const float fVersion = 0.0 ){
			this->m_identifier = sIdentifier;
			this->m_type = sType;
			this->m_extension = sExt;
			this->m_version = fVersion;	

			/*if(sType=="FormatWrapper") m_type = WrapperPlugin::WrapperType::FormatWrapper
			else if(sType=="RendererWrapper") m_type = WrapperPlugin::WrapperType::RendererWrapper;
			else if(sType=="PhysicsWrapper") m_type = WrapperPlugin::WrapperType::PhysicsWrapper;
			else m_type = WrapperPlugin::WrapperType::UnknownType;*/
		}
		/**
		* @fn void copy( const WrapperConfiguration &wConf )
		* @brief copy WrapperConfiguration variables of another WrapperConfiguration object
		* @param &wConf a WrapperConfiguration to copy
		*/
		void copy( const WrapperConfiguration &wConf ){
			initialize( wConf.getIdentifier(), wConf.getType(), wConf.getExtension(), wConf.getVersion() );
		}

		/**
		* @fn clean()
		* @brief clean WrapperConfiguration object
		*/
		void clean(){
			m_type.clear();
			m_identifier.clear();
			m_extension.clear();
		}


	private:
		float	m_version; ///<  m_version a float that give the version number of the wrapper 
		string	m_type; ///<  m_type type = { "FormatWrapper", "RendererWrapper", "PhysicsWrapper", "UnknownType" }
		//WrapperPlugin::WrapperType m_type; ///<  m_type type = { FormatWrapper, RendererWrapper, PhysicsWrapper, UnknownType }
		string	m_identifier; ///<  a string that identify the wrapper ( input file name or engine name )
		string	m_extension; ///< the input file extension for format wrappers ( "x3d", "dae", ... )
	};
} // namespace SceneGraphAdapter

#endif // _WRAPPERCONFIGURATION_H