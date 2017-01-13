///////////////////////////////////////////////////////////////////
// Automatically generated by xml2ulxr v1.7.3.
//
// xml2ulxr is part of Ultra Lightweight XML RPC for C++.
// See also http://ulxmlrpcpp.sourceforge.net
//
// Don't edit manually unless you know what you are doing
///////////////////////////////////////////////////////////////////

//#define ULXR_UNICODE_ONLY_HELPERS
#include <ulxmlrpcpp/ulxmlrpcpp.h> // always first

#include "UlxrIdlTest_ulxr_server.h"

#include <ulxmlrpcpp/ulxr_response.h>
#include <ulxmlrpcpp/ulxr_call.h>
#include <ulxmlrpcpp/ulxr_dispatcher.h>
#include <ulxmlrpcpp/ulxr_value.h>

#include "/home/ea/Dokumente/src/ulxmlrpcpp/trunk/ulxmlrpcpp/stubber/ulxr2xml/ulxridl_test.h"


#include "UlxrIdlTest_ulxr_names.h"

UlxrIdlTestServer::UlxrIdlTestServer(ulxr::MethodAdder &in_method_adder, myname::UlxrIdlTest &in_server)
   : server(in_server)
   , method_adder(in_method_adder)
{
  setupServerMethods();
}


UlxrIdlTestServer::~UlxrIdlTestServer()
{
  removeServerMethods();
}


// mapped to: void first_url4();   (there are overloaded methods)
ulxr::MethodResponse
  UlxrIdlTestServer::first_url4_ovr4 (const ulxr::MethodCall &calldata)
{
  try
  {
    server.first_url4();
    return ulxr::MethodResponse (ulxr::Void());
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'void first_url4();'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'void first_url4();'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: std::string * first_url4(int i);   (there are overloaded methods)
ulxr::MethodResponse
  UlxrIdlTestServer::first_url4_ovr3 (const ulxr::MethodCall &calldata)
{
  try
  {
    int p0 = (int) ulxr::Integer(calldata.getParam(0)).getInteger();
    std::string retval = *server.first_url4(p0);
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'std::string * first_url4(int i);'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'std::string * first_url4(int i);'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: std::string * first_url4(long int li);   (there are overloaded methods)
ulxr::MethodResponse
  UlxrIdlTestServer::first_url4_ovr2 (const ulxr::MethodCall &calldata)
{
  try
  {
    long int p0 = (long int) ulxr::Integer(calldata.getParam(0)).getInteger();
    std::string retval = *server.first_url4(p0);
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'std::string * first_url4(long int li);'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'std::string * first_url4(long int li);'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: const char * first_url4(float li);   (there are overloaded methods)
ulxr::MethodResponse
  UlxrIdlTestServer::first_url4_ovr1 (const ulxr::MethodCall &calldata)
{
  try
  {
    float p0 = (float) ulxr::Double(calldata.getParam(0)).getDouble();
    std::string retval = server.first_url4(p0);
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'const char * first_url4(float li);'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'const char * first_url4(float li);'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: wchar_t * const first_url4(double li);   (there are overloaded methods)
ulxr::MethodResponse
  UlxrIdlTestServer::first_url4_ovr0 (const ulxr::MethodCall &calldata)
{
  try
  {
    double p0 = (double) ulxr::Double(calldata.getParam(0)).getDouble();
    std::wstring retval = server.first_url4(p0);
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'wchar_t * const first_url4(double li);'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'wchar_t * const first_url4(double li);'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: std::string constTest() const;
ulxr::MethodResponse
  UlxrIdlTestServer::constTest (const ulxr::MethodCall &calldata)
{
  try
  {
    std::string retval = server.constTest();
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'std::string constTest() const;'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'std::string constTest() const;'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: std::string & constRefTest() const;
ulxr::MethodResponse
  UlxrIdlTestServer::constRefTest (const ulxr::MethodCall &calldata)
{
  try
  {
    std::string retval = server.constRefTest();
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'std::string & constRefTest() const;'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'std::string & constRefTest() const;'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: long int * firstViewConst() const;
ulxr::MethodResponse
  UlxrIdlTestServer::firstViewConst (const ulxr::MethodCall &calldata)
{
  try
  {
    long int retval = *server.firstViewConst();
    return ulxr::MethodResponse (ulxr::Integer (retval));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'long int * firstViewConst() const;'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'long int * firstViewConst() const;'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: std::string * firstView();
ulxr::MethodResponse
  UlxrIdlTestServer::firstView (const ulxr::MethodCall &calldata)
{
  try
  {
    std::string retval = *server.firstView();
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'std::string * firstView();'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'std::string * firstView();'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: std::wstring * nextView1(int i, long int * l, std::string & s, const bool * b, char c) const;
ulxr::MethodResponse
  UlxrIdlTestServer::nextView1 (const ulxr::MethodCall &calldata)
{
  try
  {
    int p0 = (int) ulxr::Integer(calldata.getParam(0)).getInteger();
    long int p1 = (long int) ulxr::Integer(calldata.getParam(1)).getInteger();
    std::string p2 = (std::string) ulxr::getLatin1(ulxr::RpcString(calldata.getParam(2)).getString());
    bool p3 = (bool) ulxr::Boolean(calldata.getParam(3)).getBoolean();
    char p4 = (char) ulxr::Integer(calldata.getParam(4)).getInteger();
    std::wstring retval = *server.nextView1(p0, &p1, p2, &p3, p4);
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'std::wstring * nextView1(int i, long int * l, std::string & s, const bool * b, char c) const;'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'std::wstring * nextView1(int i, long int * l, std::string & s, const bool * b, char c) const;'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: std::basic_string<char> * nextView2(int i, long int l, std::string & s, bool * const b, char c) const;
ulxr::MethodResponse
  UlxrIdlTestServer::nextView2 (const ulxr::MethodCall &calldata)
{
  try
  {
    int p0 = (int) ulxr::Integer(calldata.getParam(0)).getInteger();
    long int p1 = (long int) ulxr::Integer(calldata.getParam(1)).getInteger();
    std::string p2 = (std::string) ulxr::getLatin1(ulxr::RpcString(calldata.getParam(2)).getString());
    bool p3 = (bool) ulxr::Boolean(calldata.getParam(3)).getBoolean();
    char p4 = (char) ulxr::Integer(calldata.getParam(4)).getInteger();
    std::string retval = *server.nextView2(p0, p1, p2, &p3, p4);
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'std::basic_string<char> * nextView2(int i, long int l, std::string & s, bool * const b, char c) const;'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'std::basic_string<char> * nextView2(int i, long int l, std::string & s, bool * const b, char c) const;'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: void getNumObjects(std::string s);
ulxr::MethodResponse
  UlxrIdlTestServer::getNumObjects (const ulxr::MethodCall &calldata)
{
  try
  {
    std::string p0 = (std::string) ulxr::getLatin1(ulxr::RpcString(calldata.getParam(0)).getString());
    server.getNumObjects(p0);
    return ulxr::MethodResponse (ulxr::Void());
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'void getNumObjects(std::string s);'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'void getNumObjects(std::string s);'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: const std::basic_string<wchar_t> * getObject(const std::string & s);
ulxr::MethodResponse
  UlxrIdlTestServer::getObject (const ulxr::MethodCall &calldata)
{
  try
  {
    std::string p0 = (std::string) ulxr::getLatin1(ulxr::RpcString(calldata.getParam(0)).getString());
    std::wstring retval = *server.getObject(p0);
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'const std::basic_string<wchar_t> * getObject(const std::string & s);'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'const std::basic_string<wchar_t> * getObject(const std::string & s);'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: int getNumPages(const std::wstring & s);
ulxr::MethodResponse
  UlxrIdlTestServer::getNumPages (const ulxr::MethodCall &calldata)
{
  try
  {
    std::wstring p0 = (std::wstring) ulxr::getUnicode(ulxr::RpcString(calldata.getParam(0)).getString());
    long int retval = server.getNumPages(p0);
    return ulxr::MethodResponse (ulxr::Integer (retval));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'int getNumPages(const std::wstring & s);'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'int getNumPages(const std::wstring & s);'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: const std::string * last_url4();   (there are overloaded methods)
ulxr::MethodResponse
  UlxrIdlTestServer::last_url4_ovr0 (const ulxr::MethodCall &calldata)
{
  try
  {
    std::string retval = *server.last_url4();
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'const std::string * last_url4();'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'const std::string * last_url4();'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: std::string * last_url4(int i);   (there are overloaded methods)
ulxr::MethodResponse
  UlxrIdlTestServer::last_url4_ovr1 (const ulxr::MethodCall &calldata)
{
  try
  {
    int p0 = (int) ulxr::Integer(calldata.getParam(0)).getInteger();
    std::string retval = *server.last_url4(p0);
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'std::string * last_url4(int i);'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'std::string * last_url4(int i);'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}


// mapped to: std::string * last_url4(long int li);   (there are overloaded methods)
ulxr::MethodResponse
  UlxrIdlTestServer::last_url4_ovr2 (const ulxr::MethodCall &calldata)
{
  try
  {
    long int p0 = (long int) ulxr::Integer(calldata.getParam(0)).getInteger();
    std::string retval = *server.last_url4(p0);
    return ulxr::MethodResponse (ulxr::RpcString (ULXR_GET_STRING(retval)));
  }
  catch(std::exception &ex)
  {
    ulxr::CppString s = ULXR_PCHAR("C++ exception caught when invoking 'std::string * last_url4(long int li);'\n  ");
    s += ULXR_GET_STRING(ex.what());
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
  catch(...)
  {
    ulxr::CppString s = ULXR_PCHAR("Unknown exception caught when invoking 'std::string * last_url4(long int li);'");
    return ulxr::MethodResponse(ulxr::ApplicationError, s);
  }
}



void UlxrIdlTestServer::removeServerMethods()
{
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_first_url4_ovr4);  // mapped to: void first_url4();   (there are overloaded methods)
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_first_url4_ovr3);  // mapped to: std::string * first_url4(int i);   (there are overloaded methods)
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_first_url4_ovr2);  // mapped to: std::string * first_url4(long int li);   (there are overloaded methods)
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_first_url4_ovr1);  // mapped to: const char * first_url4(float li);   (there are overloaded methods)
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_first_url4_ovr0);  // mapped to: wchar_t * const first_url4(double li);   (there are overloaded methods)
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_constTest);  // mapped to: std::string constTest() const;
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_constRefTest);  // mapped to: std::string & constRefTest() const;
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_firstViewConst);  // mapped to: long int * firstViewConst() const;
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_firstView);  // mapped to: std::string * firstView();
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_nextView1);  // mapped to: std::wstring * nextView1(int i, long int * l, std::string & s, const bool * b, char c) const;
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_nextView2);  // mapped to: std::basic_string<char> * nextView2(int i, long int l, std::string & s, bool * const b, char c) const;
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_getNumObjects);  // mapped to: void getNumObjects(std::string s);
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_getObject);  // mapped to: const std::basic_string<wchar_t> * getObject(const std::string & s);
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_getNumPages);  // mapped to: int getNumPages(const std::wstring & s);
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_last_url4_ovr0);  // mapped to: const std::string * last_url4();   (there are overloaded methods)
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_last_url4_ovr1);  // mapped to: std::string * last_url4(int i);   (there are overloaded methods)
  method_adder.removeMethod(ULXR_CALLTO_UlxrIdlTest_last_url4_ovr2);  // mapped to: std::string * last_url4(long int li);   (there are overloaded methods)

}


