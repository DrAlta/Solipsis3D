#include "WrapperPlugin.h"
#include <stdexcept>

using namespace std;
using namespace SceneGraphAdapter;

/*
* @fn WrapperPlugin(const std::string &sFilename)
* @brief Constructor; loads the specified plugin as a dynamic library and locates
the plugin's exported functions
* @param  sFilename  Filename of the plugin to load
*/

WrapperPlugin::WrapperPlugin( const std::string & sFilename ) :
m_hDLL(0),
m_pfnRegisterPlugin(0),
m_pfnQueryPlugin(0),
m_pDLLRefCount(0),
m_type(WrapperType::UnknownType),
m_name(sFilename){

	std::wstring swFilename(sFilename.length(), L' ');
	std::copy(sFilename.begin(), sFilename.end(), swFilename.begin());
	m_name = sFilename;

	// Try to load the plugin as a dynamic library
	m_hDLL = ::LoadLibrary(swFilename.c_str());
	if(!m_hDLL)
		throw runtime_error(string("Could not load '") + sFilename + "'");

	// Locate the plugin's exported functions  
	try {
		m_pfnQueryPlugin = reinterpret_cast<fnQueryPlugin *> ( 
			::GetProcAddress( m_hDLL, "queryPlugin" ) 
			);
		m_pfnRegisterPlugin = reinterpret_cast<fnRegisterPlugin *>(
			::GetProcAddress(m_hDLL, "registerPlugin")
			);
		if(!m_pfnQueryPlugin || !m_pfnRegisterPlugin){
			m_pDLLRefCount = new size_t(0);
			throw runtime_error(string("'") + sFilename + "' is not a valid plugin");
		}

		m_pDLLRefCount = new size_t(1);
		WrapperConfiguration * wConfig = m_pfnQueryPlugin();
		setWrapperConfiguration(wConfig);
	}
	catch(...) {
		::FreeLibrary(m_hDLL);
		throw;
	}
}

/*
* @fn WrapperPlugin * checkWrapperPlugin(const std::string &sFilename)
* @brief loads the specified plugin as a dynamic library and locates
the plugin's exported functions, if they are not found, return NULL
* @param  &sFilename  Filename of the plugin to load
*/
WrapperPlugin * WrapperPlugin::checkWrapperPlugin(const std::string &sFilename){

	std::wstring swFilename(sFilename.length(), L' ');
	std::copy(sFilename.begin(), sFilename.end(), swFilename.begin());

	HMODULE m_hDLL = ::LoadLibrary(swFilename.c_str());
	if(!m_hDLL)
		throw runtime_error(string("Could not load '") + sFilename + "'");
	else{
		void (*pFn1)(char*) = 0;
		void (*pFn2)(char*) = 0;
		pFn1 = (void (__cdecl *)(char *))::GetProcAddress( m_hDLL, "queryPlugin" ) ;
		pFn2 = (void (__cdecl *)(char *))::GetProcAddress( m_hDLL, "registerPlugin" ) ;
		::FreeLibrary(m_hDLL);

		if(!pFn1 ) return NULL;
		else return new WrapperPlugin(sFilename);
	}

}

/*
* @fn WrapperPlugin(const WrapperPlugin &Other)
* @brief Creates a copy of a plugin that has already been loaded.
Required to provide correct semantics for storing plugins in
an STL map container.
@param  Other  Other plugin instance to copy
*/
WrapperPlugin::WrapperPlugin(const WrapperPlugin &Other) :
m_hDLL(Other.m_hDLL),
m_pfnRegisterPlugin(Other.m_pfnRegisterPlugin),
m_pfnQueryPlugin(Other.m_pfnQueryPlugin),
m_pDLLRefCount(Other.m_pDLLRefCount) {
	/// Increase DLL reference counter
	++*m_pDLLRefCount;
}

/*
* @fn ~WrapperPlugin() 
* @brief Destroys the plugin, unloading its library when no more references
to it exist.
*/
WrapperPlugin::~WrapperPlugin() {
	// Only unload the DLL if there are no more references to it
	if(!--*m_pDLLRefCount) {
		delete m_pDLLRefCount;
		::FreeLibrary(m_hDLL);
	}
}
