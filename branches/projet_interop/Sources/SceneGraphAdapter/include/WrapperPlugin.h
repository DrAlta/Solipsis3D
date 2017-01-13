/**
* \file WrapperPlugin.h
* \brief The class that loads wrappers dll
*
* Details about WrapperPlugin.h
*/

#ifndef WRAPPER_PLUGIN_H
#define WRAPPER_PLUGIN_H

#include "SceneGraphAdapter.h"
#include <string>
#include<iostream>
#include "WrapperConfiguration.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace SceneGraphAdapter {

	class SGAKernel;

/**
* The WrapperPlugin class is the class that loads wrappers dll
* ( its type, its version, ...)
* @class WrapperPlugin
* @author Rozenn Bouville Berthelot
* @date 13/01/2011
* @version 0.0
*/
	/// Representation of a plugin
	class WrapperPlugin {

	public:

		/** 
		* @fn WrapperPlugin( const std::string &sFilename )
		* @brief Constructor; initialize and load plugin; loads the specified plugin as a dynamic library and locates
		* the plugin's exported functions
		* @param sFilename the name of the plugin file
		*/
		SCENEGRAPHADAPTER_API WrapperPlugin( const std::string & sFilename );

		/** 
		* @fn static WrapperPlugin * checkWrapperPlugin(const std::string & sFilename)
		* @brief Check if the plugin "filename" is a wrapper plugin, loads the specified plugin as a dynamic library and locates
		* the plugin's exported functions, if they are not found, return NULL
		* @param sFilename the name of the plugin file to load
		* @return a pointer to the WrapperPlugin if fileName is a wrapper plugin, NULL otherwise
		*/
		SCENEGRAPHADAPTER_API static WrapperPlugin * checkWrapperPlugin(const std::string & sFilename);

		/** 
		* @fn WrapperPlugin(const WrapperPlugin &Other)
		* @brief Copy existing plugin instance;  Creates a copy of a plugin that has already been loaded.
		* Required to provide correct semantics for storing plugins in
		* an STL map container.
		* @param Other the WrapperPlugin instance to copy
		*/
		SCENEGRAPHADAPTER_API WrapperPlugin(const WrapperPlugin & Other);

		/** 
		* @fn ~WrapperPlugin()
		* @brief Destructor; unload a plugin: destroys the plugin, unloading its library when no more references
		* to it exist.
		*/
		SCENEGRAPHADAPTER_API ~WrapperPlugin();

		/**
		* \enum WrapperType
		* \brief Type of wrapper
		*/
		enum WrapperType {
			FormatWrapper,		/*!< a format wrapper */
			RendererWrapper,	/*!< a rendering engine wrapper */
			PhysicsWrapper,		/*!< a physics engine wrapper */
			UnknownType			/*!< default type */
		};

	public:

		/** 
		* @fn size_t registerPlugin(SGAKernel &WF)
		* @brief Register the plugin to a kernel
		* @param WF the kernel to register the plugin into
		* @return an index in the list where the plugin has been loaded
		*/
		SCENEGRAPHADAPTER_API size_t registerPlugin(SGAKernel &WF) {
			return m_pfnRegisterPlugin(WF);
		}

		/** 
		* @fn WrapperConfiguration * queryPlugin()
		* @brief Check if the plugin is valid
		* @return a pointer to the loaded Wrapperplugin; NULL if it is not valid
		*/
		SCENEGRAPHADAPTER_API WrapperConfiguration * queryPlugin(){
			return m_pfnQueryPlugin();
		}

		/** 
		* @fn WrapperType getType () const
		* @brief Give the type of the current WrapperPlugin
		* @return a WrapperPlugin::WrapperType
		*/
		inline WrapperType getType () const{
			return m_type;
		}

		/** 
		* @fn HMODULE getHDLL()
		* @brief Give the Win32 DLL handle
		* @return a HMODULE
		*/
		HMODULE getHDLL(){
			return m_hDLL;
		}

		/** 
		* @fn void setType(std::string sType)
		* @brief Given a string, set the WapperPlugin type
		* @param sType a string 
		*/
		void setType(std::string sType){
			//FormatWrapper, RendererWrapper, PhysicsWrapper, UnknownType
			if(sType=="FormatWrapper") m_type = WrapperType::FormatWrapper;
			else if(sType=="RendererWrapper") m_type = WrapperType::RendererWrapper;
			else if(sType=="PhysicsWrapper") m_type = WrapperType::PhysicsWrapper;
			else m_type = WrapperType::UnknownType;
		}

		/** 
		* @fn void setWrapperConfiguration(WrapperConfiguration * wConfig)
		* @brief Given a WrapperConfiguration; set WrapperPlugin variables 
		* @param wConfig the WrapperConfiguration of the loaded plugin
		*/
		void setWrapperConfiguration(WrapperConfiguration * wConfig){
			std::string sType = wConfig->getType();
			setType(sType);
		}

	private:

		//WrapperPlugin &operator =(const WrapperPlugin &Other);

		/// Signature for the plugin's registration function
		typedef size_t fnRegisterPlugin(SGAKernel &);

		// Signature for the plugin query function
		typedef WrapperConfiguration * fnQueryPlugin();

		HMODULE             m_hDLL;                ///< Win32 DLL handle
		size_t             *m_pDLLRefCount;        ///< Number of references to the DLL

		fnRegisterPlugin   *m_pfnRegisterPlugin;   ///< Plugin registration function
		fnQueryPlugin	   *m_pfnQueryPlugin;	   ///< The format manages by the plugin

		WrapperType			m_type;		///< wrapper type
		std::string			m_name;		///< dll file name
		std::string			m_ext;		///< input file extension 

	};

} // namespace SceneGraphAdapter

#endif // WRAPPER_PLUGIN_H