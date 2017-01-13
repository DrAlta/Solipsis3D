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

#ifndef __CTSystem_h__
#define __CTSystem_h__

#include <string>

namespace CommonTools {

/** This static class contains several helper methods above system.
 */
class System
{
public:
    /// Sleeping function
    static void sleep(unsigned long durationMs);

    /// Set mouse cursor visibility
    static void setMouseCursorVisibility(bool visible);

    /// Display a modal message box
    static void showMessageBox(const std::string& text, const std::string& caption, bool okButton, bool okCancelButton, bool iconAsterisk, bool iconExclamation, bool iconHand);

    /// Display the open filename modal dialog box
    static bool showDlgOpenFilename(std::string& filename, const char* filter, const std::string& extension);

    /// Run the external Web browser on the system displaying the specified url
    static void runExternalWebBrowser(const std::string& url);

    /// Run the external File browser on the system displaying the specified path
    static void runExternalFileBrowser(const std::string& path);

    /// Returns the current process identifier
    static int getPID();

    /// Returns the current date/time formatted like "YYYYMMDDHHMMSS"
    static std::string getDateTimeYYYYMMDDHHMMSS();


};

} // namespace CommonTools

#endif // #ifndef __CTSystem_h__