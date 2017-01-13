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

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "pluginbase.h"
#include <string>

class nsPluginInstance :
    public nsPluginInstanceBase
{
public:
  nsPluginInstance(nsPluginCreateData * aCreateDataStruct);
  ~nsPluginInstance();

  NPBool init(NPWindow* aWindow);
  void shut();
  NPBool isInitialized();

    // overridden from nsPluginInstanceBase
    NPError NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);
    NPError DestroyStream (NPStream *stream, NPError reason);
    // we need to provide implementation of this method as it will be
    // used by Mozilla to retrive the scriptable peer
    virtual NPError GetValue(NPPVariable variable, void *value);
    // end overridden

    // get and set
    NPP& getInstance() { return mInstance; }
    NPObject* getScriptableObject();
    void setUrl2go(const std::string url2go) { mUrl2go = url2go; }

protected:
    // keyboard and window messages hanlders
    static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK nsPluginInstance::kbHookProc(int code, WPARAM wParam, LPARAM lParam);

private:
    void _getPluginPath(const char* dllname, char* p, int size);
    void _readRegistryKey(const char* regKey, const char* regName, char* value, int sizeIn);
    bool _navigatorExecutableExist(const char* path, char* location);
    bool _findNavigatorExecutable(char* location);
    bool _runNavigatorApp();

private:
  NPP mInstance;
  NPBool mInitialized;
  HWND mhWnd;

    // old window proc for window messages
    WNDPROC mOldProc;

    // URL where navigator will go on startup
    std::string mUrl2go;

    // scriptable object for javascript calls
    NPObject* mScriptableObject;
};

#endif // __PLUGIN_H__
