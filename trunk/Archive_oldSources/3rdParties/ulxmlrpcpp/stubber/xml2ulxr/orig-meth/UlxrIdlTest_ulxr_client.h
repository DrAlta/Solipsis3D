#ifndef UlxrIdlTestUlxrClient_H
#define UlxrIdlTestUlxrClient_H

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

#include <ulxmlrpcpp/ulxr_call.h>

namespace ulxr
{
  class Requester;
  class MethodResponse;
  class MethodCall;
}

class UlxrIdlTestClient
{
  public:

    UlxrIdlTestClient
      (ulxr::Requester &requester, const ulxr::CppString &realm = ULXR_PCHAR("/RPC2"),
       const ulxr::CppString &user = ULXR_PCHAR(""), const ulxr::CppString &pass = ULXR_PCHAR(""));

    // mapped to first_url4_ovr4 (there are overloaded methods)
    void first_url4();

    // mapped to first_url4_ovr3 (there are overloaded methods)
    std::string * first_url4(int i);

    // mapped to first_url4_ovr2 (there are overloaded methods)
    std::string * first_url4(long int li);

    // mapped to first_url4_ovr1 (there are overloaded methods)
    const char * first_url4(float li);

    // mapped to first_url4_ovr0 (there are overloaded methods)
    wchar_t * const first_url4(double li);

    // mapped to constTest
    std::string constTest() const;

    // mapped to constRefTest
    std::string & constRefTest() const;

    // mapped to firstViewConst
    long int * firstViewConst() const;

    // mapped to firstView
    std::string * firstView();

    // mapped to nextView1
    std::wstring * nextView1(int i, long int * l, std::string & s, const bool * b, char c) const;

    // mapped to nextView2
    std::basic_string<char> * nextView2(int i, long int l, std::string & s, bool * const b, char c) const;

    // mapped to getNumObjects
    void getNumObjects(std::string s);

    // mapped to getObject
    const std::basic_string<wchar_t> * getObject(const std::string & s);

    // mapped to getNumPages
    int getNumPages(const std::wstring & s);

    // mapped to last_url4_ovr0 (there are overloaded methods)
    const std::string * last_url4();

    // mapped to last_url4_ovr1 (there are overloaded methods)
    std::string * last_url4(int i);

    // mapped to last_url4_ovr2 (there are overloaded methods)
    std::string * last_url4(long int li);

    virtual ~UlxrIdlTestClient();

    virtual ulxr::MethodResponse sendCall(const ulxr::MethodCall &calldata) const;

  private:

    ulxr::Requester &requester;
    ulxr::CppString rpc_realm;
    ulxr::CppString rpc_user;
    ulxr::CppString rpc_pass;

    mutable std::string ulxr_refFor_first_url4_ovr3;
    mutable std::string ulxr_refFor_first_url4_ovr2;
    mutable std::string ulxr_refFor_first_url4_ovr1;
    mutable std::wstring ulxr_refFor_first_url4_ovr0;
    mutable std::string ulxr_refFor_constRefTest;
    mutable long int ulxr_refFor_firstViewConst;
    mutable std::string ulxr_refFor_firstView;
    mutable std::wstring ulxr_refFor_nextView1;
    mutable std::string ulxr_refFor_nextView2;
    mutable std::wstring ulxr_refFor_getObject;
    mutable std::string ulxr_refFor_last_url4_ovr0;
    mutable std::string ulxr_refFor_last_url4_ovr1;
    mutable std::string ulxr_refFor_last_url4_ovr2;

    mutable ulxr::MethodCall callTo_first_url4_ovr4;  // mapped to: void UlxrIdlTest::first_url4()
    mutable ulxr::MethodCall callTo_first_url4_ovr3;  // mapped to: std::string * UlxrIdlTest::first_url4(int i)
    mutable ulxr::MethodCall callTo_first_url4_ovr2;  // mapped to: std::string * UlxrIdlTest::first_url4(long int li)
    mutable ulxr::MethodCall callTo_first_url4_ovr1;  // mapped to: const char * UlxrIdlTest::first_url4(float li)
    mutable ulxr::MethodCall callTo_first_url4_ovr0;  // mapped to: wchar_t * const UlxrIdlTest::first_url4(double li)
    mutable ulxr::MethodCall callTo_constTest;  // mapped to: std::string UlxrIdlTest::constTest() const
    mutable ulxr::MethodCall callTo_constRefTest;  // mapped to: std::string & UlxrIdlTest::constRefTest() const
    mutable ulxr::MethodCall callTo_firstViewConst;  // mapped to: long int * UlxrIdlTest::firstViewConst() const
    mutable ulxr::MethodCall callTo_firstView;  // mapped to: std::string * UlxrIdlTest::firstView()
    mutable ulxr::MethodCall callTo_nextView1;  // mapped to: std::wstring * UlxrIdlTest::nextView1(int i, long int * l, std::string & s, const bool * b, char c) const
    mutable ulxr::MethodCall callTo_nextView2;  // mapped to: std::basic_string<char> * UlxrIdlTest::nextView2(int i, long int l, std::string & s, bool * const b, char c) const
    mutable ulxr::MethodCall callTo_getNumObjects;  // mapped to: void UlxrIdlTest::getNumObjects(std::string s)
    mutable ulxr::MethodCall callTo_getObject;  // mapped to: const std::basic_string<wchar_t> * UlxrIdlTest::getObject(const std::string & s)
    mutable ulxr::MethodCall callTo_getNumPages;  // mapped to: int UlxrIdlTest::getNumPages(const std::wstring & s)
    mutable ulxr::MethodCall callTo_last_url4_ovr0;  // mapped to: const std::string * UlxrIdlTest::last_url4()
    mutable ulxr::MethodCall callTo_last_url4_ovr1;  // mapped to: std::string * UlxrIdlTest::last_url4(int i)
    mutable ulxr::MethodCall callTo_last_url4_ovr2;  // mapped to: std::string * UlxrIdlTest::last_url4(long int li)
};

#endif // UlxrIdlTestUlxrClient_H

