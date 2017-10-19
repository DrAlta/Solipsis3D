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

#include "CTStringHelpers.h"
#include <sstream>
#include <algorithm>

using namespace std;

namespace CommonTools {

//-------------------------------------------------------------------------------------
string StringHelpers::toString(float val, unsigned short precision, unsigned short width, char fill, ios::fmtflags flags)
{
    ostringstream stream;
    stream.precision(precision);
    stream.width(width);
    stream.fill(fill);
    if (flags)
        stream.setf(flags);
    stream << val;
    return stream.str();
}

//-------------------------------------------------------------------------------------
string StringHelpers::toString(int val, unsigned short width, char fill, ios::fmtflags flags)
{
    ostringstream stream;
	stream.width(width);
    stream.fill(fill);
    if (flags)
        stream.setf(flags);
    stream << val;
    return stream.str();
}

//-------------------------------------------------------------------------------------
string StringHelpers::toString(unsigned int val, unsigned short width, char fill, ios::fmtflags flags)
{
    ostringstream stream;
    stream.width(width);
    stream.fill(fill);
    if (flags)
        stream.setf(flags);
    stream << val;
    return stream.str();
}

//-------------------------------------------------------------------------------------
string StringHelpers::toString(long val, unsigned short width, char fill, ios::fmtflags flags)
{
    ostringstream stream;
	stream.width(width);
    stream.fill(fill);
    if (flags)
        stream.setf(flags);
    stream << val;
    return stream.str();
}

//-------------------------------------------------------------------------------------
string StringHelpers::toString(unsigned long val, unsigned short width, char fill, ios::fmtflags flags)
{
    ostringstream stream;
	stream.width(width);
    stream.fill(fill);
    if (flags)
        stream.setf(flags);
    stream << val;
    return stream.str();
}

//-------------------------------------------------------------------------------------
string StringHelpers::toString(bool val, bool yesNo)
{
    if (val)
    {
        if (yesNo)
        {
            return "yes";
        }
        else
        {
            return "true";
        }
    }
    else
        if (yesNo)
        {
            return "no";
        }
        else
        {
            return "false";
        }
}

//-------------------------------------------------------------------------------------
string StringHelpers::toHexString(unsigned int val)
{
    char valStr[9];
    _snprintf(valStr, sizeof(valStr) - 1, "%08X", val);
    valStr[8] = '\0';
    return valStr;
}

//-------------------------------------------------------------------------------------
unsigned int StringHelpers::convertHexStringToUInt(const string& str)
{
    unsigned int value;
    sscanf(str.c_str(), "%08X", &value);
    return value;
}

//-------------------------------------------------------------------------------------
std::wstring StringHelpers::convertStringToWString(const std::string& str)
{
    size_t size = mbstowcs(0, str.c_str(), 0) + 1;
    wchar_t *temp = new wchar_t[size];
    mbstowcs(temp, str.c_str(), size);
    std::wstring result(temp);
    delete[] temp;
    return result;
}

//-------------------------------------------------------------------------------------
std::string StringHelpers::convertWStringToString(const std::wstring& wstr)
{
    size_t size = wcstombs(0, wstr.c_str(), 0) + 1;
    char *temp = new char[size];
    wcstombs(temp, wstr.c_str(), size);
    std::string result(temp);
    delete[] temp;
    return result;
}

//-------------------------------------------------------------------------------------
void StringHelpers::autoInsertHyphens(unsigned int maxLineChars, std::wstring& wstr)
{
    if (maxLineChars < 1) return;

    static std::wstring hypenatizedCharsInsert = convertStringToWString("-,;:.!?");
    static std::wstring hypenatizedCharsReplace = convertStringToWString(" ");
    static std::wstring newLine = convertStringToWString("\x0D");
    std::wstring::size_type maxReturn = std::min(maxLineChars, (unsigned int)10);
    std::wstring::size_type size = wstr.size();
    std::wstring::size_type c = 0;

    while (c + maxLineChars < size)
    {
        c += maxLineChars - 1;
        std::wstring::size_type cmin = c - maxReturn + 1;
        while (c > cmin)
        {
            if (hypenatizedCharsInsert.find_first_of(wstr[c]) != std::wstring::npos)
            {
                c += 1;
                wstr.insert(c, newLine);
                break;
            }
            else if (hypenatizedCharsReplace.find_first_of(wstr[c]) != std::wstring::npos)
            {
                wstr.replace(c, 1, newLine);
                break;
            }
            c--;
        }
        if (c == cmin)
        {
            c += maxReturn;
            wstr.insert(c, newLine);
        }
        c++;
    }
}

//-------------------------------------------------------------------------------------
string StringHelpers::getVersionString(unsigned int version)
{
    return toString(version >> 16) + "." + toString((version & 0x0000FF00) >> 8) + "." + toString(version & 0x000000FF);
}

//-------------------------------------------------------------------------------------
void StringHelpers::replaceSubStr(string& str, const string& subStr, const string& replacementStr)
{
    string::size_type p0 = 0;
    while (true)
    {
        string::size_type p1 = str.find(subStr, p0);
        if (p1 == string::npos) break;
        str.replace(p1, subStr.length(), replacementStr);
        p0 = p1 + replacementStr.length();
    }
}

//-------------------------------------------------------------------------------------
void StringHelpers::tokenize(const string& str, const string& delimiter, vector<string>& tokens)
{
    tokens.clear();
    string::size_type p0 = 0, p1 = string::npos;
    while (p0 != string::npos)
    {
        p1 = str.find_first_of(delimiter, p0);
        if (p1 != p0)
        {
            string token = str.substr(p0, p1 - p0);
            tokens.push_back(token);
        }
        p0 = str.find_first_not_of(delimiter, p1);
    }
}

//-------------------------------------------------------------------------------------
string StringHelpers::getURL(const string& host, unsigned short port)
{
    char portStr[5 + 1];
    _snprintf(portStr, sizeof(portStr) - 1, "%d", port);
    portStr[sizeof(portStr) - 1] = '\0';
    return host + ":" + portStr;
}

//-------------------------------------------------------------------------------------
void StringHelpers::getURLHostPort(const string& url, string& host, string& port)
{
    host.clear();
    port.clear();
    string::size_type p = url.find("://");
    if (p != string::npos)
        host = url.substr(p + 3);
    else
        host = url;
    p = host.find_last_of(":");
    if (p != string::npos)
    {
        port = host.substr(p + 1);
        host = host.substr(0, p);
    }
}

//-------------------------------------------------------------------------------------
void StringHelpers::getURLHostPort(const string& url, string& host, unsigned short& port)
{
    string portStr;
    getURLHostPort(url, host, portStr);
    port = atoi(portStr.c_str());
}

//-------------------------------------------------------------------------------------
bool StringHelpers::isAValidLogin(const string& login)
{
    static string validLoginExtrasChars = "$-_.@+!*'(),";
    bool validLogin = ((login.length() > 2) && (login != "null"));
    for(int i=0;i<(int)login.length();i++)
    {
        if (!validLogin)
            break;
        validLogin = ((login[i] >= '0') && (login[i] <= '9') ||
            (login[i] >= 'a') && (login[i] <= 'z') ||
            (login[i] >= 'A') && (login[i] <= 'Z') ||
            (validLoginExtrasChars.find_first_of(login[i]) != string::npos));
    }
    return validLogin;
}

//-------------------------------------------------------------------------------------
bool StringHelpers::isAValidPassword(const string& pwd)
{
    static string validPwdExtrasChars = "$-_.@!*'(),";
    bool validPwd = ((pwd.length() >= 4) && (pwd != "null"));
    for(int i=0;i<(int)pwd.length();i++)
    {
        if (!validPwd)
            break;
        validPwd = ((pwd[i] >= '0') && (pwd[i] <= '9') ||
            (pwd[i] >= 'a') && (pwd[i] <= 'z') ||
            (pwd[i] >= 'A') && (pwd[i] <= 'Z') ||
            (validPwdExtrasChars.find_first_of(pwd[i]) != string::npos));
    }
    return validPwd;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools