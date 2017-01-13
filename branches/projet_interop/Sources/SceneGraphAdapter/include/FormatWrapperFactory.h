/**
* \file FormatWrapperFactory.h
* \brief A factory for FormatWrappers
* Details about FormatWrapperFactory.h
*/
#ifndef _FORMATWRAPPERFACTORY_H
#define _FORMATWRAPPERFACTORY_H

#include "SceneGraphAdapter.h"
#include "WrapperConfiguration.h"
#include "FormatAdapter.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

/**
* The FormatWrapperFactory class is a factory for FormatWrappers
* @class FormatWrapperFactory
* @author Rozenn Bouville Berthelot
* @date 13/01/2011
* @version 0.0
*/

namespace SceneGraphAdapter {

	class FormatWrapperFactory {

	public:

		/**
		* @fn FormatWrapperFactory()
		* @brief Constructor
		*/
		SCENEGRAPHADAPTER_API FormatWrapperFactory(){
			initialize();
		}

		/**
		* @fn ~FormatWrapperFactory()
		* @brief Destructor
		*/
		SCENEGRAPHADAPTER_API  ~FormatWrapperFactory(){
			clean();
		};

		/**
		* The FormatWrapper class is the parent class of all FormatWrappers
		* @class FormatWrapper
		* @author Rozenn Bouville Berthelot
		* @date 10/01/2011
		* @version 0.0
		*/
		class FormatWrapper : public FormatAdapter {

		public:
			/**
			* @fn FormatWrapper()
			* @brief Constructor
			*/
			FormatWrapper(){ 
				initialize(); 
			}
			/**
			* @fn FormatWrapper(const std::string sUrl,const string sExt = string(),const float fVersion = 0.0 )
			* @brief Constructor
			* @param sUrl the input file name of the FormatWrapper ( ex: "..\..\test.x3d" )
			* @param sExt the input file extension of the FormatWrapper ( ex: "x3d" )
			* @param fVersion the version number of the wrapper ( ex: 1.0 )
			*/
			FormatWrapper(const std::string sUrl,const string sExt = string(),const float fVersion = 0.0 ){ 
				initialize(sUrl,sExt,fVersion); 
			}
			/**
			* @fn ~FormatWrapper() 
			* @brief Destructor
			*/
			virtual ~FormatWrapper() {
				delete m_configuration;
			};

			/**
			* @fn WrapperConfiguration * getWrapperConfiguration()
			* @brief Virtual method to get WrapperConfiguration of the FormatWrapper
			*/
			virtual WrapperConfiguration * getWrapperConfiguration() = 0;

		private:

			void initialize(const std::string sUrl = string(), const string sExt = string(), const float fVersion = 0.0 ){
				m_configuration = new WrapperConfiguration(sUrl,"FormatWrapper",sExt,fVersion);
			}

		protected:

			WrapperConfiguration * m_configuration; /// > the WrapperConfiguration of the FormatWrapper
		};

		/**
		* @fn void addWrapper(std::auto_ptr<FormatWrapper> FW)
		* @brief add a new FormatWrapper to the FormatWrapperFactory
		* @param FW a pointer to the new FormatWrapper to add
		*/
		SCENEGRAPHADAPTER_API void addWrapper(std::auto_ptr<FormatWrapper> FW) {
			m_FormatWrapperMap.insert( FormatWrapperMap::value_type( FW.get()->getWrapperConfiguration()->getIdentifier(), getWrapperCount() ) );
			m_FormatWrappers.push_back(FW.release());
		}
		/**
		* @fn size_t getWrapperCount() const
		* @brief give the number of FormatWrapper in the FormatWrapperFactory
		*/
		SCENEGRAPHADAPTER_API size_t getWrapperCount() const {
			return m_FormatWrappers.size();
		}
		/**
		* @fn FormatWrapper * getWrapper(const size_t idx) 
		* @brief give the FormatWrapper at the given index in FormatWrapperFactory::m_FormatWrappers
		* @param idx index in FormatWrapperFactory::m_FormatWrappers
		* @return a pointer to the searched FormatWrapper
		*/
		SCENEGRAPHADAPTER_API FormatWrapper * getWrapper(const size_t idx) {
			return m_FormatWrappers.at(idx);
		}
		/**
		* @fn FormatWrapper * getWrapper( const string &sUrl )
		* @brief given an input file, give the associated FormatWrapper
		* @param &sUrl the input file 
		* @return a pointer to the searched FormatWrapper
		*/
		SCENEGRAPHADAPTER_API FormatWrapper * getWrapper( const string &sUrl ){
			if(m_FormatWrapperMap.find(sUrl) == m_FormatWrapperMap.end()) return NULL;
			size_t idx = m_FormatWrapperMap.find(sUrl)->second;
			return getWrapper(idx);
		}

	private:

		/**
		* @fn void initialize( const std::vector<FormatWrapper *> &fWrappers = std::vector<FormatWrapper *>(), const std::map< string, size_t > &fwMap = std::map< string, size_t >() )
		* @brief initialize FormatWrapperFactory variables
		* @param fWrappers a FormatWrapperFactory::FormatWrapperVector
		* @param fwMap a FormatWrapperFactory::FormatWrapperMap
		*/
		void initialize( const std::vector<FormatWrapper *> &fWrappers = std::vector<FormatWrapper *>(), const std::map< string, size_t > &fwMap = std::map< string, size_t >() ){
			m_FormatWrappers = fWrappers;
			m_FormatWrapperMap = fwMap;
		}
		/**
		* @fn clean()
		* @brief clean FormatWrapperFactory
		*/
		void clean(){
			m_FormatWrappers.clear();
			m_FormatWrapperMap.clear();
		}	

	private:

		/** \typedef typedef std::vector<FormatWrapper *> FormatWrapperVector
		*	A vector of format wrappers
		*/
		typedef std::vector<FormatWrapper *> FormatWrapperVector;

		/** \typedef typedef std::map< string, size_t > FormatWrapperMap 
		*	WrapperMap.first =  input file name ( ex: "duck.dae" )
		*	WrapperMap.second = index of the associated FormatWrapper in m_FormatWrappers
		*/
		typedef std::map< string, size_t > FormatWrapperMap; 

		FormatWrapperVector m_FormatWrappers; ///< All available format wrappers
		FormatWrapperMap m_FormatWrapperMap;  ///< All loaded files and their associated FormatWrapper

	};

} //namespace SceneGraphAdapter

#endif // _FORMATWRAPPERFACTORY_H