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

#include "CacheManager.h"
#include <RakNetConnection.h>
#include <FileList.h>
#include <FileOperations.h>
#include <CTLog.h>

using namespace RakNet;
using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
CacheManager::CacheManager() :
    mCachePath("")
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::CacheManager()");
}

//-------------------------------------------------------------------------------------
CacheManager::~CacheManager()
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::~CacheManager()");
}

//-------------------------------------------------------------------------------------
void CacheManager::initialize(const std::string& cachePath)
{
    mCachePath = cachePath;

    std::string filename = mCachePath + "\\cache.xml";

    LOGHANDLER_LOGF(LogHandler::VL_INFO, "CacheManager::initialize() loading cache from %s", filename.c_str());

    try
    {
        TiXmlDocument xmlFileDoc(filename.c_str());
        if (!xmlFileDoc.LoadFile())
            throw std::string("Parsing error");
        TiXmlElement* cacheElt = xmlFileDoc.FirstChildElement("cache");
        if (cacheElt == 0)
            throw std::string("Parsing error");
        TiXmlElement* filesElt;
        if ((filesElt = cacheElt->FirstChildElement("files")) != 0)
        {
            const char* attr = 0;
            for (TiXmlElement* fileElt = filesElt->FirstChildElement("file"); fileElt != 0; fileElt = fileElt->NextSiblingElement("file"))
            {
                LodContentFileStruct lodContentFileStruct;
                if (!XmlHelpers::fromXmlEltLodContentFileStruct(fileElt, lodContentFileStruct))
                    throw std::string("Parsing error");
                addFile(lodContentFileStruct.mFilename, lodContentFileStruct.mVersion);
            }
        }
    }
    catch (std::string exceptionStr)
    {
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "CacheManager::initialize() Unable to read/parse cache from %s, Resetting it.", filename);
        mCache.clear();
    }
}

//-------------------------------------------------------------------------------------
void CacheManager::finalize()
{
    std::string filename = mCachePath + "\\cache.xml";

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::finalize() saving cache into %s", filename.c_str());

    TiXmlDocument xmlFileDoc(filename.c_str());
    // root node
    TiXmlElement* cacheElt = new TiXmlElement("cache");
    // Add entries
    TiXmlElement* filesElt = new TiXmlElement("files");
    cacheElt->LinkEndChild(filesElt); 
    for (CacheMap::iterator it = mCache.begin(); it != mCache.end(); ++it)
    {
        if (it->second.mState != ESTransferComplete) continue;
        LodContentFileStruct lodContentFileStruct;
        lodContentFileStruct.mFilename = it->first;
        lodContentFileStruct.mVersion = it->second.mVersion;
        TiXmlElement* fileElt = XmlHelpers::toXmlEltLodContentFileStruct("file", lodContentFileStruct);
        filesElt->LinkEndChild(fileElt);
    }
    xmlFileDoc.LinkEndChild(cacheElt); 
    xmlFileDoc.SaveFile();
}

//-------------------------------------------------------------------------------------
bool CacheManager::OnFile(OnFileStruct *onFileStruct)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFile() filename:%s", onFileStruct->fileName);

    std::string filename = onFileStruct->fileName;
    std::string pathname = mCachePath + "\\" + filename;
    WriteFileWithDirectories(pathname.c_str(), (char*)onFileStruct->fileData, (unsigned int)onFileStruct->finalDataLength);

    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "CacheManager::OnFile() filename:%s not found in requested files !", filename.c_str());
    else
    {
        entryIt->second.mState = ESTransferComplete;

        // Call callbacks
        PendingDownloadList& pendingDownloadList = entryIt->second.mPendingDownloadList;
        for (PendingDownloadList::iterator pendingDownloadIt = pendingDownloadList.begin(); pendingDownloadIt != pendingDownloadList.end(); ++pendingDownloadIt)
            pendingDownloadIt->mCallback->onTransferComplete(filename);
        pendingDownloadList.clear();

        // Send pending uploads
        FileList fileList;
        std::string pathname = mCachePath + "\\" + filename;
        fileList.AddFile(pathname.c_str(), filename.c_str(), 0);
        PendingUploadList& pendingUploadList = entryIt->second.mPendingUploadList;
        for (PendingUploadList::iterator pendingUploadIt = pendingUploadList.begin(); pendingUploadIt != pendingUploadList.end(); ++pendingUploadIt)
        {
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFile() Sending now file %s, version:%d to recipient:%s, fileListTransferSetID:%d", filename.c_str(), entryIt->second.mVersion, pendingUploadIt->mRecipient.ToString(), pendingUploadIt->mFileListTransferSetID);
            RakNetConnection::getSingletonPtr()->mFileListTransfer.Send(&fileList, RakNetConnection::getSingletonPtr()->mRakPeer, pendingUploadIt->mRecipient, pendingUploadIt->mFileListTransferSetID, LOW_PRIORITY, 0, false);
        }
        pendingUploadList.clear();
    }

    return true;
}

//-------------------------------------------------------------------------------------
void CacheManager::OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int partLength)
{
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFileProgress() filename:%s", onFileStruct->fileName);

    std::string filename = onFileStruct->fileName;
    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "CacheManager::OnFileProgress() filename:%s not found in requested files !", filename.c_str());
    else
    {
        entryIt->second.mState = (EntryState)(std::min(99, (int)(partCount*100/partTotal)));
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFileProgress() filename:%s state:%d", filename.c_str(), entryIt->second.mState);
    }
}

//-------------------------------------------------------------------------------------
void CacheManager::addFile(const std::string& filename, const FileVersion& version)
{
    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
    {
        Entry entry;
        entry.mVersion = version;
        entry.mState = ESTransferComplete;
        mCache[filename] = entry;
        entryIt = mCache.find(filename);
    }
    entryIt->second.mVersion = version;
}

//-------------------------------------------------------------------------------------
void CacheManager::requestFile(const SystemAddress& sender, const std::string& filename, const FileVersion& version, CacheManagerCallback* callback)
{
    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
    {
        Entry entry;
        entry.mVersion = version;
        entry.mState = ESTransferToRequest;
        mCache[filename] = entry;
        entryIt = mCache.find(filename);
    }
    else if (entryIt->second.mVersion != version)
    {
        entryIt->second.mState = ESTransferToRequest;
    }
    if (entryIt->second.mState == ESTransferToRequest)
    {
        entryIt->second.mVersion = version;
        entryIt->second.mState = 0;
        BitStream bitStream;
        bitStream.Write((MessageID)RakNetConnection::ID_REQUESTING_FILETRANSFER);
        PendingDownload pendingDownload;
        pendingDownload.mFileListTransferSetID = RakNetConnection::getSingletonPtr()->mFileListTransfer.SetupReceive(this, false, sender);
        pendingDownload.mCallback = callback;
        entryIt->second.mPendingDownloadList.push_back(pendingDownload);
        bitStream.Write(pendingDownload.mFileListTransferSetID);
        RakNetConnection::SerializeString(&bitStream, filename);
        bitStream.Write(entryIt->second.mVersion);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::requestFile() Requesting file %s, version:%d, fileListTransferSetID:%d", filename.c_str(), version, pendingDownload.mFileListTransferSetID);
        // Send the request to the server
        RakNetConnection::getSingletonPtr()->mRakPeer->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, sender, false);
    }
    else if (entryIt->second.mState == ESTransferComplete)
        callback->onTransferComplete(filename);
}

//-------------------------------------------------------------------------------------
void CacheManager::removeFile(const std::string& filename, CacheManagerCallback* callback)
{
    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
        // entry not found !
        return;
    for (PendingDownloadList::iterator pendingDownloadIt = entryIt->second.mPendingDownloadList.begin(); pendingDownloadIt != entryIt->second.mPendingDownloadList.end(); ++pendingDownloadIt)
        if (pendingDownloadIt->mCallback == callback)
        {
            entryIt->second.mPendingDownloadList.erase(pendingDownloadIt);
            if (entryIt->second.mPendingDownloadList.empty())
            {
                mCache.erase(entryIt);
            }
            return;
        }
    // callback not found !
}

//-------------------------------------------------------------------------------------
void CacheManager::sendFile(const SystemAddress& recipient, unsigned short fileListTransferSetID, std::string& filename, const FileVersion& version)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::sendFile() Sending file %s, version:%d to recipient:%s, fileListTransferSetID:%d", filename.c_str(), version, recipient.ToString(), fileListTransferSetID);

    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
        // entry not found !
        return;

    if (entryIt->second.mState == ESTransferComplete)
    {
        // Send it now !
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::sendFile() Sending now file %s, version:%d to recipient:%s, fileListTransferSetID:%d", filename.c_str(), version, recipient.ToString(), fileListTransferSetID);
        FileList fileList;
        std::string pathname = mCachePath + "\\" + filename;
        fileList.AddFile(pathname.c_str(), filename.c_str(), 0);
        RakNetConnection::getSingletonPtr()->mFileListTransfer.Send(&fileList, RakNetConnection::getSingletonPtr()->mRakPeer, recipient, fileListTransferSetID, LOW_PRIORITY, 0, false);
    }
    else
    {
        // Add it to the pending upload list, it will be sent as soon as it will be downloaded
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::sendFile() Add in pendingUpload file %s, version:%d to recipient:%s, fileListTransferSetID:%d", filename.c_str(), version, recipient.ToString(), fileListTransferSetID);
        PendingUpload pendingUpload;
        pendingUpload.mFileListTransferSetID = fileListTransferSetID;
        pendingUpload.mRecipient = recipient;
        entryIt->second.mPendingUploadList.push_back(pendingUpload);
    }
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
