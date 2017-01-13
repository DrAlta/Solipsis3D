/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

// navigatorActiveXCtrl.cpp : Implementation of CnavigatorActiveXCtrl
#include "stdafx.h"
#include "navigatorActiveXCtrl.h"

using namespace Solipsis;

// ==============================
// key map
// ==============================
static KeyCode sKeyMap[0xFF];
void initKeyMap()
{
    int i;

    /* Map the VK keysyms */
    for ( i=0; i<0xFF; ++i )
        sKeyMap[i] = KC_UNASSIGNED;

	sKeyMap['0'] = KC_0;
	sKeyMap['1'] = KC_1;
	sKeyMap['2'] = KC_2;
	sKeyMap['3'] = KC_3;
	sKeyMap['4'] = KC_4;
	sKeyMap['5'] = KC_5;
	sKeyMap['6'] = KC_6;
	sKeyMap['7'] = KC_7;
	sKeyMap['8'] = KC_8;
	sKeyMap['9'] = KC_9;

	sKeyMap['A'] = KC_A;
	sKeyMap['B'] = KC_B;
	sKeyMap['C'] = KC_C;
	sKeyMap['D'] = KC_D;
	sKeyMap['E'] = KC_E;
	sKeyMap['F'] = KC_F;
	sKeyMap['G'] = KC_G;
	sKeyMap['H'] = KC_H;
	sKeyMap['I'] = KC_I;
	sKeyMap['J'] = KC_J;
	sKeyMap['K'] = KC_K;
	sKeyMap['L'] = KC_L;
	sKeyMap['M'] = KC_M;
	sKeyMap['N'] = KC_N;
	sKeyMap['O'] = KC_O;
	sKeyMap['P'] = KC_P;
	sKeyMap['Q'] = KC_Q;
	sKeyMap['R'] = KC_R;
	sKeyMap['S'] = KC_S;
	sKeyMap['T'] = KC_T;
	sKeyMap['U'] = KC_U;
	sKeyMap['V'] = KC_V;
	sKeyMap['W'] = KC_W;
	sKeyMap['X'] = KC_X;
	sKeyMap['Y'] = KC_Y;
	sKeyMap['Z'] = KC_Z;

	sKeyMap[VK_DELETE] = KC_DELETE;

    sKeyMap[VK_UP] = KC_UP;
    sKeyMap[VK_DOWN] = KC_DOWN;
    sKeyMap[VK_RIGHT] = KC_RIGHT;
    sKeyMap[VK_LEFT] = KC_LEFT;
    sKeyMap[VK_INSERT] = KC_INSERT;
    sKeyMap[VK_HOME] = KC_HOME;
    sKeyMap[VK_END] = KC_END;
    sKeyMap[VK_PRIOR] = KC_PGUP;
    sKeyMap[VK_NEXT] = KC_PGDOWN;

    sKeyMap[VK_F1] = KC_F1;
    sKeyMap[VK_F2] = KC_F2;
    sKeyMap[VK_F3] = KC_F3;
    sKeyMap[VK_F4] = KC_F4;
    sKeyMap[VK_F5] = KC_F5;
    sKeyMap[VK_F6] = KC_F6;
    sKeyMap[VK_F7] = KC_F7;
    sKeyMap[VK_F8] = KC_F8;
    sKeyMap[VK_F9] = KC_F9;
    sKeyMap[VK_F10] = KC_F10;
    sKeyMap[VK_F11] = KC_F11;
    sKeyMap[VK_F12] = KC_F12;
}

// ==============================
// navigator DLL loading
// ==============================

// navaigator application
IApplication* application = 0;

// last location of navigator DLL
char _szLastNavigatorDllLocation[1024] = "";

// navigator related defines
#define SZ_NAVIGATORACTIVEXDLL      "navigatorActiveX.dll"
#define SZ_NAVIGATORDLL             "Navigator.dll"

void _getPluginPath(const char* dllname, char* p, int size)
{
    // get where plugin dll is
    int len = GetModuleFileName(GetModuleHandle(dllname), p, size);
    assert(len>0);
    for ( int i = len - 1;  i >= 0; i--)
        if ( '\\' == p[i] )
        {
            p[i + 1] = 0;
            break;
        }
}

IApplication* _createApplication()
{
    IApplication* application = 0;
    bool error = true;

    // find navigator dll
    char location[1024];
    location[0] = '\0';
    // First check if navigator ActiveX dll is loaded and return its path
    // Else, hModule will be NULL, in which case, the following will return the application's path
    _getPluginPath(SZ_NAVIGATORACTIVEXDLL, &location[0], 1024);

    // if loaded, remember the location
    if (strlen(location) > 0)
        strcpy(_szLastNavigatorDllLocation, location);

    SetCurrentDirectory(_szLastNavigatorDllLocation);

    application = IApplication::createApplication(location);
    if (application != 0)
        error = false;

    if (error)
    {
        // some error message to the user...
        MessageBox(0, SZ_NAVIGATORDLL" could not be found ! Please re-install plugin ...", "Error loading "SZ_NAVIGATORDLL" plugin", MB_OK);
    }

    return application;
}

// CnavigatorActiveXCtrl

std::map<HWND, IInstance*> CnavigatorActiveXCtrl::mInstances;

CnavigatorActiveXCtrl::CnavigatorActiveXCtrl() :
    mInitialized(false),
    mhWnd(0),
    mKeyboardHook(0),
    mFirstPersonMode(false),
    mNavigatorInstance(0),
    mWidth(0),
    mHeight(0)
{
	// important, ATL by default creates windowless controls
	m_bWindowOnly = true;

    initKeyMap();
}

HRESULT CnavigatorActiveXCtrl::OnDraw(ATL_DRAWINFO& di)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnDraw\n");
/*	if (!mInitialized)
    {
		IOleInPlaceActiveObjectImpl<CnavigatorActiveXCtrl>::GetWindow(&mhWnd);
		RECT& rc = *(RECT*)di.prcBounds;
		mWidth = rc.right - rc.left;
		mHeight = rc.bottom - rc.top;

        int result = true;
        HRESULT hr;
        CComPtr<IOleObject> spOleObject;
        CComPtr<IUnknown> unknownPtr;
        hr = CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (void**)&browser);
        if (hr != S_OK) goto end;
        hr = browser->QueryInterface(IID_IOleObject, (void**)&spOleObject);
        if (hr != S_OK) goto end;
        HWND browserWin = CreateWindow("BUTTON", "", WS_CHILD|WS_VISIBLE, 0, 0, 512, 512, GetDesktopWindow(), (HMENU)0, NULL, NULL);
        hr = AtlAxAttachControl(browser, browserWin, &unknownPtr);
        if (hr != S_OK) goto end;
        hr = browser->Navigate(L"file:///C:/Work/solipsis/trunk/Media/NaviLocal/uilogin.html", NULL, NULL, NULL, NULL);
        if (hr != S_OK) goto end;
        mInitialized = true;
        return S_OK;
    }
    else
    {
        int result = true;
        HRESULT hr;
        IDispatch *doc = NULL;
        hr = browser->get_Document(&doc);
        if (hr != S_OK) goto end;
        IHTMLDocument2 *htmlDoc = (IHTMLDocument2*)doc;
        IHTMLElement *pElement = NULL;
        IHTMLElementRender *pRender = NULL;
        READYSTATE ready_state;
        browser->get_ReadyState(&ready_state);
        hr = htmlDoc->get_body(&pElement);
        if (hr != S_OK) goto end;
        if (pElement == NULL) goto end;
        hr = pElement->QueryInterface(IID_IHTMLElementRender, (void **) &pRender);
        if (hr != S_OK) goto end;
        if (pRender == NULL) goto end;
        HBITMAP hbm = CreateCompatibleBitmap(GetWindowDC(), 512, 512);
        HDC hdc = CreateCompatibleDC(GetWindowDC());
        HGDIOBJ old = SelectObject(hdc, hbm);
        ExtFloodFill(hdc, 0, 0, RGB(60, 120, 180), FLOODFILLBORDER);
        hr = pRender->DrawToDC(hdc);
        if (hr != S_OK) goto end;
        BITMAPINFO bminfo;
        #define RASWIDTH(width, bpp) ((((width) * (bpp) + 31) >> 5) << 2)
        #define W 512
        #define H 512
        #define D 24
        int spanbytes = RASWIDTH(W, D);
        int imagesize = spanbytes*H;
        bminfo.bmiHeader.biSize = sizeof(bminfo.bmiHeader);
        bminfo.bmiHeader.biWidth = W;
        bminfo.bmiHeader.biHeight = -H;
        bminfo.bmiHeader.biSizeImage = imagesize;
        bminfo.bmiHeader.biBitCount = D;
        bminfo.bmiHeader.biCompression = BI_RGB;
        bminfo.bmiHeader.biPlanes = 1;
        bminfo.bmiHeader.biXPelsPerMeter = 0;
        bminfo.bmiHeader.biYPelsPerMeter = 0;
        bminfo.bmiHeader.biClrUsed = 0;
        bminfo.bmiHeader.biClrImportant = 0;
        char *buffer = new char[imagesize];
        hr = GetDIBits(hdc, hbm, 0, 512, (LPVOID)buffer, &bminfo, DIB_RGB_COLORS);
        delete buffer;
        SelectObject(hdc, old);
end:
        result = false;
        return S_OK;
    }*/
	if (!mInitialized)
    {
		IOleInPlaceActiveObjectImpl<CnavigatorActiveXCtrl>::GetWindow(&mhWnd);
		RECT& rc = *(RECT*)di.prcBounds;
		mWidth = rc.right - rc.left;
		mHeight = rc.bottom - rc.top;

        // create the navigator application
        if (application == 0)
            application = _createApplication();

        // create the navigator instance and set this window
        if (application)
        {
            std::string envVar;
            envVar = CONTAINER_NAME_ENV"=activex";
            _putenv(envVar.c_str());
            envVar = NAVI_SUPPORT_ENV"=no";
            _putenv(envVar.c_str());

            mNavigatorInstance = application->createInstance();
            assert(mNavigatorInstance);
            mInstances[mhWnd] = mNavigatorInstance;

            bool ret = mNavigatorInstance->setWindow(this);
            assert(ret);

            mInitialized = true;
        }
    }
/*    RECT& rc = *(RECT*)di.prcBounds;
    // Set Clip region to the rectangle specified by di.prcBounds
    HRGN hRgnOld = NULL;
    if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
    hRgnOld = NULL;
    bool bSelectOldRgn = false;

    HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

    if (hRgnNew != NULL)
    {
    bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
    }

    Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
    SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
    LPCTSTR pszText = _T("ATL 7.0 : navigatorActiveXCtrl");
    TextOut(di.hdcDraw, 
    (rc.left + rc.right) / 2, 
    (rc.top + rc.bottom) / 2, 
    pszText, 
    lstrlen(pszText));

    if (bSelectOldRgn)
    SelectClipRgn(di.hdcDraw, hRgnOld);
*/
    return S_OK;
}

LRESULT CnavigatorActiveXCtrl::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnSetCursor\n");
	// if we process this, then the container cannot set a cursor
	return 0; 
}

LRESULT CnavigatorActiveXCtrl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnSize\n");
    return S_OK;
}

LRESULT CnavigatorActiveXCtrl::OnMouseLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnMouseLButtonDblClk\n");
    mFirstPersonMode = !mFirstPersonMode;
    if (mFirstPersonMode)
    {
        RECT rect;
        POINT p;
        ::GetWindowRect(mhWnd, &rect);
        p.x = rect.left + ((rect.right - rect.left)>>1);
        p.y = rect.top + ((rect.bottom - rect.top)>>1);
        ::SetCursorPos(p.x, p.y);
        ::SetCapture(mhWnd);
    }
    else
        ::ReleaseCapture();
    return S_OK;
}

LRESULT CnavigatorActiveXCtrl::OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnMouseLButtonDown\n");
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMousePressed;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        mouseEvt.mMouse.mState.mXrel = 0;
        mouseEvt.mMouse.mState.mYrel = 0;
        mouseEvt.mMouse.mState.mZrel = 0;
        mouseEvt.mMouse.mState.mButtons = MBLeft;
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT CnavigatorActiveXCtrl::OnMouseLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnMouseLButtonUp\n");
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMouseReleased;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        mouseEvt.mMouse.mState.mXrel = 0;
        mouseEvt.mMouse.mState.mYrel = 0;
        mouseEvt.mMouse.mState.mZrel = 0;
        mouseEvt.mMouse.mState.mButtons = MBLeft;
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT CnavigatorActiveXCtrl::OnMouseRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnMouseRButtonDown\n");
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMousePressed;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        mouseEvt.mMouse.mState.mXrel = 0;
        mouseEvt.mMouse.mState.mYrel = 0;
        mouseEvt.mMouse.mState.mZrel = 0;
        mouseEvt.mMouse.mState.mButtons = MBRight;
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT CnavigatorActiveXCtrl::OnMouseRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnMouseRButtonUp\n");
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMouseReleased;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        mouseEvt.mMouse.mState.mXrel = 0;
        mouseEvt.mMouse.mState.mYrel = 0;
        mouseEvt.mMouse.mState.mZrel = 0;
        mouseEvt.mMouse.mState.mButtons = MBRight;
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT CnavigatorActiveXCtrl::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//    sprintf(text, "CnavigatorActiveXCtrl::OnMouseMove (%d, %d) wParam=%d\n", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
//    ::OutputDebugString(text);
    if (mNavigatorInstance != 0)
    {
        Evt mouseEvt;
        mouseEvt.mType = ETMouseMoved;
        mouseEvt.mMouse.mState.mX = (int)GET_X_LPARAM(lParam);
        mouseEvt.mMouse.mState.mY = (int)GET_Y_LPARAM(lParam);
        mouseEvt.mMouse.mState.mZ = 0;
        if (mFirstPersonMode)
        {
            RECT rect;
            POINT p;
            ::GetWindowRect(mhWnd, &rect);
            p.x = rect.left + ((rect.right - rect.left)>>1);
            p.y = rect.top + ((rect.bottom - rect.top)>>1);
            POINT np;
            ::GetCursorPos(&np);
            if ((np.x == p.x) && (np.y == p.y))
                return S_OK;
            mouseEvt.mMouse.mState.mXrel = np.x - p.x;
            mouseEvt.mMouse.mState.mYrel = np.y - p.y;
            mouseEvt.mMouse.mState.mZrel = 0;
            ::SetCursorPos(p.x, p.y);
        }
        else
        {
            if (lastMouseEvt.mType == ETNone)
            {
                mouseEvt.mMouse.mState.mXrel = 0;
                mouseEvt.mMouse.mState.mYrel = 0;
                mouseEvt.mMouse.mState.mZrel = 0;
            }
            else
            {
                mouseEvt.mMouse.mState.mXrel = mouseEvt.mMouse.mState.mX - lastMouseEvt.mState.mX;
                mouseEvt.mMouse.mState.mYrel = mouseEvt.mMouse.mState.mY - lastMouseEvt.mState.mY;
                mouseEvt.mMouse.mState.mZrel = mouseEvt.mMouse.mState.mZ - lastMouseEvt.mState.mZ;
            }
            lastMouseEvt = mouseEvt.mMouse;
        }
        mouseEvt.mMouse.mState.mButtons = MBNone;
        if (wParam & MK_LBUTTON) mouseEvt.mMouse.mState.mButtons = (MouseButton)((int)mouseEvt.mMouse.mState.mButtons | MBLeft);
        if (wParam & MK_MBUTTON) mouseEvt.mMouse.mState.mButtons = (MouseButton)((int)mouseEvt.mMouse.mState.mButtons | MBMiddle);
        if (wParam & MK_RBUTTON) mouseEvt.mMouse.mState.mButtons = (MouseButton)((int)mouseEvt.mMouse.mState.mButtons | MBRight);
        mNavigatorInstance->processEvent(Event(0, &mouseEvt));
    }
    return S_OK;
}

LRESULT CnavigatorActiveXCtrl::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnSetFocus\n");
    if (mNavigatorInstance != 0)
    {
        if (mKeyboardHook == 0)
            mKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)CnavigatorActiveXCtrl::fnHookKeyboard, 0, ::GetCurrentThreadId());
        lastMouseEvt.mType = ETNone;
    }
    return S_OK;
}

LRESULT CnavigatorActiveXCtrl::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ::OutputDebugString("CnavigatorActiveXCtrl::OnKillFocus\n");
    if (mNavigatorInstance != 0)
    {
        if (mKeyboardHook != 0)
            ::UnhookWindowsHookEx(mKeyboardHook);
        mKeyboardHook = 0;
        mFirstPersonMode = false;
    }
    return S_OK;
}

LRESULT CALLBACK CnavigatorActiveXCtrl::fnHookKeyboard(int code, WPARAM wParam, LPARAM lParam)
{
    if (code == HC_NOREMOVE)
        return 0;
    if (code < 0)
        return ::CallNextHookEx(0, code, wParam, lParam);

    IInstance* instance = 0;
    HWND hWnd = ::GetFocus();
    std::map<HWND, IInstance*>::const_iterator it;
    it = mInstances.find(hWnd);
    if (it != mInstances.end())
        instance = it->second;
    else
        instance = mInstances.begin()->second;
    if (instance)
    {
        Evt keyboardEvt;
        keyboardEvt.mType = (HIWORD(lParam) & KF_UP) ? ETKeyReleased : ETKeyPressed;
        keyboardEvt.mKeyboard.mKey = sKeyMap[wParam];
        instance->processEvent(Event(0, &keyboardEvt));
        ::SetFocus(hWnd);
    }

    return 1;
}

void CnavigatorActiveXCtrl::FinalRelease()
{
    if (application)
    {
        if (mNavigatorInstance)
        {
	        // unregister the keyboard hook now
	        // so we stop sending keyboard messages
            if (mKeyboardHook != 0)
                ::UnhookWindowsHookEx(mKeyboardHook);

            // remove/destroy this instance
            mInstances.erase(mhWnd);
            application->destroyInstance(mNavigatorInstance);
            mNavigatorInstance = 0;
        }
    }

    mhWnd = 0;
//    mInitialized = false;
    ::ShowCursor(TRUE);

    // destroy the navigator application if no more instances
    if (application && (mInstances.size() == 0))
	{
        application->destroy();
		application = 0;
	}
}
