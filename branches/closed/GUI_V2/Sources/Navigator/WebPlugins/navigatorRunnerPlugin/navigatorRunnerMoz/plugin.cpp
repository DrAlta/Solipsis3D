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

/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */


#include <windows.h>
#include <windowsx.h>
#include <sys/stat.h>
#include <stdio.h>
#include <process.h>

#include "plugin.h"
#include "scriptablePluginObject.h"


// last location of navigator DLL
char _szLastNavigatorLocation[1024] = "";

// navigator related defines
#define SZ_NPSOLNAVRUNDLL           "npsolnavrun.dll"
#define SZ_NAVIGATORSUBDIR          "solipsis_navigator"
#define SZ_NAVIGATORREGKEY          "SOFTWARE\\Solipsis\\Navigator" // reg key in HKEY_CURRENT_USER
#define SZ_NAVIGATORINSTALLDIR      "Install Directory"
#define SZ_NAVIGATOREXE              "Navigator.exe"

//////////////////////////////////////
//
// general initialization and shutdown
//
NPError NS_PluginInitialize()
{
  return NPERR_NO_ERROR;
}

void NS_PluginShutdown()
{
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//

nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  if (!aCreateDataStruct)
    return NULL;

  nsPluginInstance * plugin = new nsPluginInstance(aCreateDataStruct);
  return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
  if (aPlugin)
    delete (nsPluginInstance *)aPlugin;
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//

nsPluginInstance::nsPluginInstance(nsPluginCreateData * aCreateDataStruct) : nsPluginInstanceBase(),
  mInstance(aCreateDataStruct->instance),
  mInitialized(FALSE),
  mhWnd(NULL),
  mOldProc(NULL),
  mScriptableObject(NULL)
{
    // check type
    assert(!strcmp(aCreateDataStruct->type, "application/x-solnavrun"));

    // parameters
    assert(aCreateDataStruct->argc > 0);
    for ( int i = 0; i < aCreateDataStruct->argc; i++)
    {
        if (!stricmp(aCreateDataStruct->argn[i], "url2go"))
        {
            mUrl2go = std::string(aCreateDataStruct->argv[i]);
        }
    }
}

nsPluginInstance::~nsPluginInstance()
{
    if (mScriptableObject)
        NPN_ReleaseObject(mScriptableObject);
}

static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);

NPBool nsPluginInstance::init(NPWindow* aWindow)
{
  if (aWindow == NULL)
    return FALSE;

  mhWnd = (HWND)aWindow->window;
  if (mhWnd == NULL)
    return FALSE;

    // subclass window so we can intercept window messages and
    // do our drawing to it
    mOldProc = SubclassWindow(mhWnd, (WNDPROC)PluginWinProc);

  // associate window with our nsPluginInstance object so we can access 
  // it in the window procedure
  SetWindowLong(mhWnd, GWL_USERDATA, (LONG)this);

  mInitialized = TRUE;
  return TRUE;
}

void nsPluginInstance::shut()
{
    // subclass it back
    SubclassWindow(mhWnd, mOldProc);

  mhWnd = NULL;
  mInitialized = FALSE;
}

NPBool nsPluginInstance::isInitialized()
{
  return mInitialized;
}

NPError nsPluginInstance::NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)         
{ 
    if (stream->notifyData == NULL)
    {
    }
    return NPERR_NO_ERROR; 
}

NPError nsPluginInstance::DestroyStream (NPStream *stream, NPError reason)
{
    return NPERR_NO_ERROR; 
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// here the plugin is asked by Mozilla to tell if it is scriptable
// we should return a valid interface id and a pointer to 
// nsScriptablePeer interface which we should have implemented
// and which should be defined in the corressponding *.xpt file
// in the bin/components folder
NPError nsPluginInstance::GetValue(NPPVariable variable, void *value)
{
    NPError rv = NPERR_NO_ERROR;

    switch (variable) {
    case NPPVpluginNameString:
        *((char **)value) = "npsolnavrun";
        break;
    case NPPVpluginDescriptionString:
        *((char **)value) = "Solipsis Navigator Runner Plug-in for Mozilla";
        break;
    case NPPVpluginScriptableNPObject:
        *(NPObject **)value = getScriptableObject();
        break;
    default:
        rv = NPERR_GENERIC_ERROR;
        break;
    }

    return rv;
}

LRESULT CALLBACK nsPluginInstance::PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	nsPluginInstance* p = (nsPluginInstance*)(GetWindowLong(hWnd, GWL_USERDATA));

    switch (msg)
    {
    case WM_SETCURSOR:
        {
        }
        break;
    case WM_SIZE:
        {
        }
        break;
    case WM_KEYDOWN:
        {
        }
        break;
    case WM_LBUTTONDOWN:
        {
        }
        break;
    case WM_LBUTTONUP:
        {
            if (p)
                p->_runNavigatorApp();
        }
        break;
    case WM_MOUSEMOVE:
        {
        }
        break;
    case WM_RBUTTONUP:
        {
        }
        break;
    case WM_RBUTTONDOWN:
        {
        }
        break;
    case WM_MBUTTONUP:
        {
        }
        break;
    case WM_MBUTTONDOWN:
        {
        }
        break;
    case WM_PAINT:
        {
            // draw a frame and display the string
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc;
            GetClientRect(hWnd, &rc);
            FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));

            // get our plugin instance object and ask it for the version string
            nsPluginInstance *plugin = (nsPluginInstance *)GetWindowLong(hWnd, GWL_USERDATA);
            if (plugin) {
                const char * string = "Click Me !";
                DrawText(hdc, string, strlen(string), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }
            else {
                char string[] = "Error occured";
                DrawText(hdc, string, strlen(string), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }

            EndPaint(hWnd, &ps);
        }
        break;
    default:
        break;
    }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

// return instance of the scriptable object
NPObject* nsPluginInstance::getScriptableObject()
{
    if (!mScriptableObject) {
        mScriptableObject = NPN_CreateObject(getInstance(), &ScriptablePluginObject::sScriptablePluginObject_NPClass);
        ((ScriptablePluginObject*) mScriptableObject)->setPlugin(this);
    }

    if (mScriptableObject)
        NPN_RetainObject(mScriptableObject);

    return mScriptableObject;
}

void nsPluginInstance::_getPluginPath(const char* dllname, char* p, int size)
{
    // get where plugin dll is
    int len = GetModuleFileName(GetModuleHandle(dllname), p, size);
    for ( int i = len - 1;  i >= 0; i--)
        if ( '\\' == p[i] )
        {
            p[i + 1] = 0;
            break;
        }
}

void nsPluginInstance::_readRegistryKey(const char* regKey, const char* regName, char* value, int sizeIn)
{
    DWORD ret;
    DWORD size = sizeIn;
    HKEY hKey = NULL;
    
    ret = RegOpenKeyEx(HKEY_CURRENT_USER, regKey, 0, KEY_READ, &hKey);
    
    if (hKey)
    {
        ret = RegQueryValueEx(hKey, regName, NULL, NULL, (LPBYTE)value, &size);
        value[size - 1] = 0;
        RegCloseKey(hKey);
    }
}

bool nsPluginInstance::_navigatorExecutableExist(const char* path, char* location)
{
    std::string executablePath = std::string(path);
    std::string executable = executablePath + std::string(SZ_NAVIGATOREXE);
    struct _stat statBuffer;
    if (_stat(executable.c_str(), &statBuffer) == 0)
    {
        strcpy(location, executablePath.c_str());
        return true;
    }
    return false;
}

bool nsPluginInstance::_findNavigatorExecutable(char* location)
{
    std::string executablePath;

    location[0] = '\0';

    // 1 ) look in the same folder as this module
    char pluginPath[1024];
    _getPluginPath(SZ_NPSOLNAVRUNDLL, &pluginPath[0], 1024);
    if (_navigatorExecutableExist(pluginPath, location))
        return true;
    // 2 ) if not, look in the subdirectory
    executablePath = std::string(pluginPath) + std::string(SZ_NAVIGATORSUBDIR) + std::string("\\");
    if (_navigatorExecutableExist(executablePath.c_str(), location))
        return true;
    // 3 ) if not, look in the registry
    char registryPath[1024] = "";
    _readRegistryKey(SZ_NAVIGATORREGKEY, SZ_NAVIGATORINSTALLDIR, registryPath, 1024);
    if (strlen(registryPath) != 0)
    {
        if (registryPath[strlen(registryPath) - 1] != '\\')
            strcat(registryPath, "\\");
        if (_navigatorExecutableExist(registryPath, location))
            return true;
    }

    return false;
}

bool nsPluginInstance::_runNavigatorApp()
{
    bool error = false;

    // find navigator dll
    char location[1024];
    location[0] = '\0';
    _findNavigatorExecutable(location);

    // if found, remember the location
    if (strlen(location) > 0)
        strcpy(_szLastNavigatorLocation, location);
    else
    {
        error = true;
        // some error message to the user...
        ::MessageBox(NULL, SZ_NAVIGATOREXE" could not be found ! Please re-install plugin ...", "Error running "SZ_NAVIGATOREXE" executable", MB_OK);
    }

    if (!error)
    {
        SetCurrentDirectory(_szLastNavigatorLocation);
        std::string executable;
        executable = std::string(_szLastNavigatorLocation) + std::string(SZ_NAVIGATOREXE);
        char *args[3];
        args[0] = (char*)executable.c_str();
        args[1] = (char*)mUrl2go.c_str();
        args[2] = 0;
        ::spawnv(_P_DETACH, executable.c_str(), args);
    }

    return !error;
}