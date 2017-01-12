/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author RealXTend, updated by JAN Gregory

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

#include "TightVNCConnection.h"
#include "TightVNCTextureSystem.h"
//#include "../llcommon/llerror.h"

#include <vncviewer.h>

#include <OgreHardwarePixelBuffer.h>
#include <OgreRoot.h>
#include <OgreLogManager.h>
#include <OgreStringConverter.h>
#include <OgreTextureManager.h>


PBITMAPINFO CreateBitmapInfo(HBITMAP hBmp);

static const float VNC_TEXTURE_UPDATE_DELAY = 0.5;
int TightVNCConnection::mTexIDCounter = 0;

// ----------------------------------------------------------------------------

// GREG BEGIN
/*TightVNCConnection::TightVNCConnection(int id, TightVNCTextureSystem* textureSystem,
                                       const std::string& host, int port)
    : mUpdateTimer(VNC_TEXTURE_UPDATE_DELAY)
    , mScreenDirty(false)*/
TightVNCConnection::TightVNCConnection(int id, TightVNCTextureSystem* textureSystem,
                                       const std::string& host, int port, const std::string& pwd, int fps)
    : mScreenDirty(false)
// GREG END
    , mScreen(0)
    , mHost(host)
    , mPort(port)
// GREG BEGIN
    , mGrabScreenIfDirty(true)
    , wParam(0)
    , mPwd(pwd)
    , mFps(fps)
// GREG END
    , mConn(0)
    , mTextureSystem(textureSystem)
    , mID(id)
    , mAlive(true)
    , mSafeToDelete(true)
{
    Ogre::Root::getSingleton().addFrameListener(this);
// GREG BEGIN
    mUpdateTimer = 1.0/mFps;
// GREG END
}

TightVNCConnection::~TightVNCConnection()
{
    while (!mSafeToDelete) {}

    if (mConn)
    {
        // NOTE: Connection can't be deleted here! see omni_thread::~omni_thread
        mConn->RemoveUpdateListener(this);
        mConn->KillThread();
//         delete mConn;
        mConn = 0;
    }

    mTextureSystem->connectionClosed(mID);
    mTextureSystem = 0;

// GREG BEGIN
    delete mScreen;
// GREG END
}

bool TightVNCConnection::connect(VNCviewerApp* app)
{
// GREG BEGIN
//    mConn = app->NewConnection((TCHAR*)mHost.c_str(), mPort);
    mConn = app->NewConnection((TCHAR*)mHost.c_str(), mPort, mPwd.empty() ? NULL : (TCHAR*)mPwd.c_str());
// GREG END
    if (mConn)
    {
        mConn->AddUpdateListener(this);
        return true;
    }

    return false;
}

bool TightVNCConnection::isConnected() const
{
    return (mConn != 0);
}

bool TightVNCConnection::isAlive() const
{
    if (mAlive && mConn)
        return mConn->IsRunning();
    else
        return false;
}


void TightVNCConnection::destroy()
{
    mSafeToDelete = false;
    mAlive = false;
}

void TightVNCConnection::textureReceived()
{
    Ogre::String textureName = "TightVNCTexture" + Ogre::StringConverter::toString(mTexIDCounter++);

    Ogre::TextureManager& tmgr = Ogre::TextureManager::getSingleton();
    // GREG BEGIN
/*    mTexture = tmgr.createManual(textureName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);*/
    mTexture = tmgr.createManual(textureName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_BYTE_BGRA, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    // GREG END

    mTextureSystem->connectionCreated(mID, textureName);
}

void TightVNCConnection::textureResized()
{
    Ogre::LogManager::getSingleton().logMessage("TightVNCConnection - Reloading VNC texture (resized)");

    Ogre::String name = mTexture->getName();
    Ogre::TextureManager& tmgr = Ogre::TextureManager::getSingleton();
//     tmgr.unload(m_pInfo->m_texture->getHandle());
    tmgr.remove(mTexture->getHandle());

    // GREG BEGIN
/*    mTexture = tmgr.createManual(name,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);*/
    mTexture = tmgr.createManual(name,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
        mWidth, mHeight, 0, Ogre::PF_BYTE_BGRA, Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    // GREG END
}

std::string TightVNCConnection::getURL() const
{
    return "vnc://" + getHost() + ":" + Ogre::StringConverter::toString(getPort());
}


// ----------------------------------------------------------------------------
// VNC ScreenUpdateListener callback

// GREG BEGIN
/*
void TightVNCConnection::screenUpdated(HDC dc, HBITMAP bitmap)
{
    omni_mutex_lock lock(mUpdateMutex);

    PBITMAPINFO info = CreateBitmapInfo(bitmap);
    if (!info)
        return;

    delete mScreen;
    mScreen = new BYTE[info->bmiHeader.biSizeImage];
    if (!GetDIBits(dc, bitmap, 0, info->bmiHeader.biHeight, mScreen, info, DIB_RGB_COLORS))
    {
        delete mScreen;
        mScreen = 0;
        return;
    }

    mWidth = info->bmiHeader.biWidth;
    mHeight = info->bmiHeader.biHeight;

    mScreenDirty = true;
}
*/
void TightVNCConnection::screenUpdated(HDC dc, HBITMAP bitmap)
{
    omni_mutex_lock lock(mUpdateMutex);

    if (!mGrabScreenIfDirty)
        return;

    PBITMAPINFO info = CreateBitmapInfo(bitmap);
    if (!info)
        return;

    if ((mScreen == 0) || (info->bmiHeader.biWidth != mWidth) || (info->bmiHeader.biHeight != mHeight))
    {
        mWidth = info->bmiHeader.biWidth;
        mHeight = info->bmiHeader.biHeight;
        delete mScreen;
        mScreen = new BYTE[info->bmiHeader.biSizeImage];
    }
    info->bmiHeader.biHeight = -info->bmiHeader.biHeight;
    if (!GetDIBits(dc, bitmap, 0, -info->bmiHeader.biHeight, mScreen, info, DIB_RGB_COLORS))
    {
        delete mScreen;
        mScreen = 0;
        return;
    }

    mScreenDirty = true;
    mGrabScreenIfDirty = false;
}
// GREG END

// ----------------------------------------------------------------------------
// Ogre FrameListener callbacks

bool TightVNCConnection::frameStarted(const Ogre::FrameEvent& e)
{
    omni_mutex_lock lock(mUpdateMutex);

    if (!mAlive)
        return true;

// GREG BEGIN
/*    // Allocate new texture if data is available and texture hasn't been created
    if (mScreen && mTexture.isNull())
        textureReceived();

    // If no VNC data has been received yet
    if (!mScreen || mTexture.isNull())
        return true;

    // Check for resize
    if ((mWidth != mTexture->getWidth()) || (mHeight != mTexture->getHeight()))
        textureResized();
*/
// GREG END

    mUpdateTimer -= e.timeSinceLastFrame;
    if (mUpdateTimer > 0)
        return true;

// GREG BEGIN
//    mUpdateTimer = VNC_TEXTURE_UPDATE_DELAY;
    mUpdateTimer = 1.0/mFps;
    mGrabScreenIfDirty = true;
// GREG END

    // Update texture

    if (mScreenDirty)
    {
// GREG BEGIN
        // Allocate new texture if data is available and texture hasn't been created
        if (mScreen && mTexture.isNull())
            textureReceived();

        // If no VNC data has been received yet
        if (!mScreen || mTexture.isNull())
            return true;

        // Check for resize
        if ((mWidth != mTexture->getWidth()) || (mHeight != mTexture->getHeight()))
            textureResized();
// GREG END
        Ogre::HardwarePixelBufferSharedPtr pbuf = mTexture->getBuffer();
        pbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        const Ogre::PixelBox& pixelBox = pbuf->getCurrentLock();
// GREG BEGIN
//        memcpy(pixelBox.data, mScreen, pixelBox.getConsecutiveSize());
        long srcPixelSize = 4;
        // if height is negative -> top-down DIB
        // if height is positive -> bottom-up DIB
//        long srcPitch = (mHeight < 0) ? mWidth*srcPixelSize : -mWidth*srcPixelSize;
//        unsigned char* pSrc = (mHeight < 0) ? mScreen : (mScreen + (mHeight - 1)*mWidth*srcPixelSize);
        long srcPitch = mWidth*srcPixelSize;
        unsigned char* pSrc = mScreen;
	    size_t texPixelSize = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
	    size_t texPitch = (pixelBox.rowPitch*texPixelSize);
	    unsigned char* pDst = static_cast<unsigned char*>(pixelBox.data);
        for(int y = 0; y < mHeight; y++)
        {
            memcpy(pDst, pSrc, mWidth*srcPixelSize);
/*            unsigned char* pSrcTmp = pSrc;
            unsigned char* pDstTmp = pDst;
            for(int x = 0; x < mWidth; x++)
            {
                pDstTmp[0] = pSrcTmp[0];
                pDstTmp[1] = pSrcTmp[1];
                pDstTmp[2] = pSrcTmp[2];
                pDstTmp[3] = 0;
                pSrcTmp += srcPixelSize;
                pDstTmp += texPixelSize;
            }*/
            pSrc += srcPitch;
            pDst += texPitch;
        }
// GREG END
        pbuf->unlock();
        mScreenDirty = false;
    }

   return true;
}

bool TightVNCConnection::frameEnded(const Ogre::FrameEvent& e)
{
    omni_mutex_lock lock(mUpdateMutex);

    if (!mAlive)
    {
        Ogre::Root::getSingletonPtr()->removeFrameListener(this);
        mSafeToDelete = true;
        return true;
    }

    return true;
}

// GREG BEGIN
Ogre::String TightVNCConnection::handleEvt(const Ogre::String& evt)
{
    Ogre::String result = "";

    if (mConn == 0) return result;

    std::vector<Ogre::String> tokens;
    std::string delimiter = "?";
    size_t p0 = 0, p1 = Ogre::String::npos;
    while (p0 != Ogre::String::npos)
    {
        p1 = evt.find_first_of(delimiter, p0);
        if (p1 != p0)
        {
            Ogre::String token = evt.substr(p0, p1 - p0);
            tokens.push_back(token);
        }
        p0 = evt.find_first_not_of(delimiter, p1);
    }

    if (tokens.size() < 1) return result;
    if (tokens[0].compare("getaddress") == 0)
        return mHost + ":" + Ogre::StringConverter::toString(mPort);

    return result;
}

void TightVNCConnection::handleEvt(const Solipsis::Event& evt)
{
    using namespace Solipsis;
    if (!mConn) return;

    const Evt& e = evt.getEvt();
    if ((e.mType == ETMousePressed) ||
        (e.mType == ETMouseReleased) ||
        (e.mType == ETMouseMoved))
    {
        int width = (int)mTexture->getSrcWidth();
        int height = (int)mTexture->getSrcHeight();
        int x = ((int)(e.mMouse.mState.mXreal*width))%width;
        int y = ((int)(e.mMouse.mState.mYreal*height))%height;
        UINT iMsg;
        int typeIdx = 0;
        switch (e.mType)
        {
        case ETMouseMoved: typeIdx = 0; break;
        case ETMousePressed: typeIdx = 1; break;
        case ETMouseReleased: typeIdx = 2; break;
        }
        int btnIdx = 0;
        switch (e.mMouse.mState.mButtons)
        {
        case MBLeft: btnIdx = 1; break;
        case MBRight: btnIdx = 2; break;
        case MBMiddle: btnIdx = 3; break;
        }
        static UINT eMouseKbdEventMapping[3][4] = {
            {WM_MOUSEMOVE, WM_MOUSEMOVE, WM_MOUSEMOVE, WM_MOUSEMOVE},
            {WM_MOUSEMOVE, WM_LBUTTONDOWN, WM_RBUTTONDOWN, WM_MBUTTONDOWN}, 
            {WM_MOUSEMOVE, WM_LBUTTONUP, WM_RBUTTONUP, WM_MBUTTONUP}
        };
        iMsg = eMouseKbdEventMapping[typeIdx][btnIdx];
        static UINT eMouseKbdEventMappingBtnStateOr[3][4] = {
            {0 ,0 ,0 ,0},
            {0, MK_LBUTTON, MK_RBUTTON, MK_MBUTTON},
            {0 ,0 ,0 ,0}
        };
        static UINT eMouseKbdEventMappingBtnStateAnd[3][4] = {
            {0 ,0 ,0 ,0},
            {0 ,0 ,0 ,0},
            {0, MK_LBUTTON, MK_RBUTTON, MK_MBUTTON}
        };
        wParam |= eMouseKbdEventMappingBtnStateOr[typeIdx][btnIdx];
        wParam &= ~eMouseKbdEventMappingBtnStateAnd[typeIdx][btnIdx];

        mConn->mouseEvt(x, y, iMsg, wParam);
    }
    else if ((e.mType == ETKeyPressed) || (e.mType == ETKeyReleased))
    {
        DWORD lParam = 0;
        if (e.mType == ETKeyReleased) lParam |= 0x80000000l; // down
        lParam |= 0x1000000; // extended
	    HKL layout = GetKeyboardLayout(0);
//	    if (GetKeyboardState(keyState) == 0) return;
	    unsigned int vk = MapVirtualKeyEx(e.mKeyboard.mKey, 3, layout);
	    if (vk == 0) return;
        mConn->kbdEvt(vk, lParam);
    }
}
// GREG END

// ---------------------------------------------------------------------------
// Taken from MSDN

PBITMAPINFO CreateBitmapInfo(HBITMAP hBmp)
{
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
        return 0;

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 

     if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits)); 

     // There is no RGBQUAD array for the 24-bit-per-pixel format. 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98/Me, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
}
