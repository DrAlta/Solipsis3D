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

#ifndef __CacheManager_h__
#define __CacheManager_h__

#include <string>
#include <list>
#include <RakPeerInterface.h>
#include <FileListTransferCBInterface.h>
#include <XmlDatas.h>

namespace Solipsis {

/** This class defines the callback interface of the cache manager.
*/
class CacheManagerCallback
{
public:
    /** Called when transfer completed. */
    virtual void onTransferComplete(const std::string& filename) = 0;
};

/** This class manages 1 files cache.
*/
class CacheManager : public FileListTransferCBInterface
{
public:
    /// Entry state
    typedef char EntryState;
    static const EntryState ESTransferToRequest = (EntryState)-1;
    static const EntryState ESTransferComplete = (EntryState)100;

    /// Pending download
    typedef struct {
        unsigned short mFileListTransferSetID;
        CacheManagerCallback* mCallback;
    } PendingDownload;
    /// Pending downloads list
    typedef std::list<PendingDownload> PendingDownloadList;

    /// Pending upload
    typedef struct {
        unsigned short mFileListTransferSetID;
        SystemAddress mRecipient;
    } PendingUpload;
    /// Pending uploads list
    typedef std::list<PendingUpload> PendingUploadList;

    /// Cache entry
    typedef struct
    {
        FileVersion mVersion;
        EntryState mState;
        PendingDownloadList mPendingDownloadList;
        PendingUploadList mPendingUploadList;
    } Entry;
    /// Cache map of <filename, Entry>
    typedef std::map<std::string, Entry> CacheMap;

protected:
    /// Cache path
    std::string mCachePath;

    /// Map of files in cache
    CacheMap mCache;

public:
    /** Constructor. */
    CacheManager();
    /** Destructor. */
    virtual ~CacheManager();

    /** Initialize cache by loading last saved state. */
    void initialize(const std::string& cachePath);
    /** Finalize cache management by saving current state. */
    void finalize();

    /** See FileListTransferCBInterface. */
    virtual bool OnFile(OnFileStruct *onFileStruct);
    /** See FileListTransferCBInterface. */
    virtual void OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int partLength);

    /** Add 1 file in cache. */
    void addFile(const std::string& filename, const FileVersion& version);
    /** Request 1 file in cache. */
    void requestFile(const SystemAddress& sender, const std::string& filename, const FileVersion& version, CacheManagerCallback* callback);
    /** Remove 1 file to manage. */
    void removeFile(const std::string& filename, CacheManagerCallback* callback);

    /** Send 1 file to a recipient. */
    void sendFile(const SystemAddress& recipient, unsigned short fileListTransferSetID, std::string& filename, const FileVersion& version);
};

} // namespace Solipsis

#endif // #ifndef __CacheManager_h__