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

#ifndef __CTStringHelpers_h__
#define __CTStringHelpers_h__

#include <string>
#include <vector>

namespace CommonTools {

/** This static class contains several helper methods above system.
 */
class StringHelpers
{
public:
    /// Converts a float to a string
    static std::string toString(float val, unsigned short precision = 6, unsigned short width = 0, char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
    /// Converts an int to a string
    static std::string toString(int val, unsigned short width = 0, char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
    /// Converts an unsigned int to a string
    static std::string toString(unsigned int val, unsigned short width = 0, char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
    /// Converts a long to a string
    static std::string toString(long val, unsigned short width = 0, char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
    /// Converts an unsigned long to a string
    static std::string toString(unsigned long val, unsigned short width = 0, char fill = ' ', std::ios::fmtflags flags = std::ios::fmtflags(0));
    /// Converts a boolean to a string
    static std::string toString(bool val, bool yesNo = false);
    /// Converts an unsigned int to an hexadecimal string
    static std::string toHexString(unsigned int val);
    /// Converts an hexadecimal string to an unsigned int
    static unsigned int convertHexStringToUInt(const std::string& str);
    /// Converts a multibyte string to a wide-character string
    static std::wstring convertStringToWString(const std::string& wstr);
    /// Converts a wide-character string to a multibyte string
    static std::string convertWStringToString(const std::wstring& str);
    /// Insert hyphens according to the maximum number of characters per line
    static void autoInsertHyphens(unsigned int maxLineChars, std::wstring& wstr);
    /// Converts an unsigned int version to a string
    static std::string getVersionString(unsigned int version);
    /// Replace 1 sub-string by 1 replacement string into 1 string
    static void replaceSubStr(std::string& str, const std::string& subStr, const std::string& replacementStr);
    /// Tokenize function
    static void tokenize(const std::string& str, const std::string& delimiter, std::vector<std::string>& tokens);
    /// Concatenate host and port to form 1 url
    static std::string getURL(const std::string& host, unsigned short port);
    /// Retrieve host and port of 1 url
    static void getURLHostPort(const std::string& url, std::string& host, std::string& port);
    /// Retrieve host and decimal port of 1 url
    static void getURLHostPort(const std::string& url, std::string& host, unsigned short& port);
    /// Returns true if a login is valid
    static bool isAValidLogin(const std::string& login);
    /// Returns true if a password is valid
    static bool isAValidPassword(const std::string& pwd);
};

} // namespace CommonTools

#endif // #ifndef __CTStringHelpers_h__