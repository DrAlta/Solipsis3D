/**
* \file EngineWrapperFactory.h
* \brief A factory for EngineWrappers
* Details about EngineWrapperFactory.h
*/

#ifndef _ENGINEWRAPPERFACTORY_H
#define _ENGINEWRAPPERFACTORY_H

#include "SceneGraphAdapter.h"
#include "WrapperConfiguration.h"
#include "RendererAdapter.h"
#include "PhysicsAdapter.h"
#include <string>
#include <map>

using namespace std;

/**
* The EngineWrapperFactory class is a factory for EngineWrappers
* @class EngineWrapperFactory
* @author Rozenn Bouville Berthelot
* @date 13/01/2011
* @version 0.0
*/

namespace SceneGraphAdapter {

	class EngineWrapperFactory {

	public:
		/**
		* @fn EngineWrapperFactory()
		* @brief Constructor
		*/
		SCENEGRAPHADAPTER_API EngineWrapperFactory(){
			initialize();
		}

		/**
		* @fn ~EngineWrapperFactory()
		* @brief Destructor
		*/
		SCENEGRAPHADAPTER_API  ~EngineWrapperFactory(){
			clean();
		};

		/**
		* The EngineWrapper class is the parent class of all EngineWrappers
		* @class EngineWrapper
		* @author Rozenn Bouville Berthelot
		* @date 10/01/2011
		* @version 0.0
		*/
		class EngineWrapper {

		public:
			/**
			* @fn EngineWrapper()
			* @brief Constructor
			*/
			EngineWrapper(){
				initialize(); 
			}
			/**
			* @fn EngineWrapper(const std::string &sIdentifier, const std::string &sType = "UnknownType", const float fVersion = 0.0 )
			* @brief Constructor
			* @param sIdentifier a string that identify the engine ( ex: "ogre", "unity", "physx", ...)
			* @param sType  the type of engine, sType = { RendererWrapper, PhysicsWrapper, UnknownType }
			* @param fVersion the version of the EngineWrapper ( ex: 1.0 )
			*/
			EngineWrapper(const std::string &sIdentifier, const std::string &sType = "UnknownType", const float fVersion = 0.0 ){ 
				initialize(sIdentifier,sType,fVersion); 
			}
			/**
			* @fn ~EngineWrapper() 
			* @brief Destructor
			*/
			virtual ~EngineWrapper() {
				delete m_configuration;
			};
			/**
			* @fn WrapperConfiguration * getWrapperConfiguration()
			* @brief Virtual method to get WrapperConfiguration of the EngineWrapper
			*/
			virtual WrapperConfiguration * getWrapperConfiguration() = 0;

			/**
			* @fn void configureEngine(SGAKernel * kernel)
			* @brief Virtual method to set engine configuration
			*/
			virtual void configureEngine(SGAKernel * kernel) = 0;

			/**
			* @fn void run( float time )
			* @brief Virtual method to run engine if required
			*/
			virtual void run( float time ) = 0;

		private:

			void initialize(const std::string sIdentifier = string(), const std::string sType = "UnknownType", const float fVersion = 0.0 ){
				m_configuration = new WrapperConfiguration(sIdentifier,sType,string(),fVersion);
			}

		protected:

			WrapperConfiguration * m_configuration;
		};

		class RendererWrapper : public EngineWrapper, public RendererAdapter {};
		class PhysicsWrapper : public EngineWrapper, public PhysicsAdapter {};

		/**
		* @fn void addWrapper(std::auto_ptr<EngineWrapper> FW)
		* @brief add a new EngineWrapper to the EngineWrapperFactory
		* @param FW a pointer to the new EngineWrapper to add
		*/
		SCENEGRAPHADAPTER_API void addWrapper(std::auto_ptr<EngineWrapper> FW) {
			int idx = getWrapperCount();
			m_EngineWrappers.push_back(FW.release());
			m_EngineWrapperMap.insert( EngineWrapperMap::value_type( getWrapper(idx)->getWrapperConfiguration()->getIdentifier(), idx ));
		}
		/**
		* @fn size_t getWrapperCount() const
		* @brief give the number of EngineWrapper in the EngineWrapperFactory
		*/
		SCENEGRAPHADAPTER_API size_t getWrapperCount() const {
			return m_EngineWrappers.size();
		}
		/**
		* @fn EngineWrapper * getWrapper(const size_t Index)
		* @brief give the EngineWrapper at the given index in EngineWrapperFactory::m_EngineWrappers
		* @param Index index in EngineWrapperFactory::m_EngineWrappers
		* @return a pointer to the searched EngineWrapper
		*/
		SCENEGRAPHADAPTER_API EngineWrapper * getWrapper(const size_t Index) {
			if( m_EngineWrappers.size()<=Index ) return NULL;
			return m_EngineWrappers.at(Index);
		}
		/**
		* @fn EngineWrapper * getWrapper( const string &sIdentifier )
		* @brief given an identifier, give the associated EngineWrapper
		* @param sIdentifier identifier of the engine, ( ex: "ogre" )
		* @return a pointer to the searched EngineWrapper
		*/
		//SCENEGRAPHADAPTER_API RendererWrapper * getWrapperByIdent( const string &sIdentifier ){
		SCENEGRAPHADAPTER_API EngineWrapper * getWrapper( const string &sIdentifier ){
			if( m_EngineWrapperMap.find(sIdentifier) == m_EngineWrapperMap.end() ) return NULL;
			size_t idx = m_EngineWrapperMap.find(sIdentifier)->second;
			return getWrapper(idx);
		}

	private:

		/**
		* @fn void initialize( const std::vector<EngineWrapper *> &eWrappers = std::vector<EngineWrapper *>(), const std::map< string, size_t > &ewMap = std::map< string, size_t >() )
		* @brief initialize EngineWrapperFactory variables
		* @param eWrappers a EngineWrapperFactory::EngineWrapperVector
		* @param ewMap a EngineWrapperFactory::EngineWrapperMap
		*/
		void initialize( const std::vector<EngineWrapper *> &eWrappers = std::vector<EngineWrapper *>(), const std::map< string, size_t > &ewMap = std::map< string, size_t >() ){
			m_EngineWrappers = eWrappers;
			m_EngineWrapperMap = ewMap;
		}
		/**
		* @fn clean()
		* @brief clean EngineWrapperFactory
		*/
		void clean(){
			m_EngineWrappers.clear();
			m_EngineWrapperMap.clear();
		}	


	private:

		typedef std::vector<EngineWrapper *> EngineWrapperVector;

		/** \typedef typedef std::map< string, size_t > EngineWrapperMap 
		*	WrapperMap.first =  engine identifier ( ex: "ogre", "physx", ... )
		*	WrapperMap.second = index of the associated EngineWrapper in EngineWrapperFactory::m_EngineWrappers
		*/
		typedef std::map< string, size_t > EngineWrapperMap; 

		EngineWrapperVector m_EngineWrappers; ///< All available EngineWrappers
		EngineWrapperMap m_EngineWrapperMap;  ///< All loaded engines and their associated index in EngineWrapperFactory::m_EngineWrappers

	};

} //namespace SceneGraphAdapter

#endif // _ENGINEWRAPPERFACTORY_H