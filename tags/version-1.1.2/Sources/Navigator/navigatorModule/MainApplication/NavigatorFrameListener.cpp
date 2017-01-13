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

#include "Prerequisites.h"

#include <NaviManager.h>
#include <Navi.h>
#include <OgreTimer.h>
#include <OgreExternalTextureSourceManager.h>
#include "OgreTools/ExternalTextureSourceEx.h"
#include "NavigatorFrameListener.h"
#include "OgreTools/OgreHelpers.h"
#include "Tools/DebugHelpers.h"
#include "Cameras/OrbitalCameraSupport.h"
#include "Cameras/FirstPersonCameraSupport.h"

#include "navigatorGui/Mouse.h"
#include "navigatorGui/GUI_ContextMenu.h"
#include "navigatorGui/GUI_Modeler.h"
#include "navigatorGui/GUI_ModelerProperties.h"
#include "navigatorGui/GUI_Avatar.h"
#include "navigatorGui/GUI_AvatarProperties.h"
#include "navigatorGui/GUI_Chat.h"
#include "navigatorGui/GUI_Debug.h"

using namespace NaviLibrary;
using namespace Solipsis;
using namespace CommonTools;

#define MOUSE_WHEEL_FACTOR (1.0f/120.0f)*0.25f
#define ESCAPE_HITS_CANCEL_FOCUS_DURATION 1000
#define ESCAPE_HITS_CANCEL_FOCUS 2

//-------------------------------------------------------------------------------------
NavigatorFrameListener::NavigatorFrameListener(Navigator* navigator) :
    OgreFrameListener(navigator->getRenderWindowPtr(),navigator->getCameraPtr(),navigator->getSceneMgrPtr()),
    mNavigator(navigator),
    mBoundingBoxesShows(false),
    mEscapeHitsB4CancellingFocus(0),
    mLastEscapeHitTimer(0),
    mMouseLeftPressed(false),
    mMouseMiddlePressed(false),
	mMouseRightPressed(false)
{
    mStandardOverlay = OverlayManager::getSingleton().getByName("Solipsis/StandardOverlay");
    if (mStandardOverlay != 0)
        mStandardOverlay->show();
}


//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::frameStarted(const FrameEvent& evt)
{
#ifdef UIDEBUG
    DebugHelpers::frameStarted(evt, mNavigator, mSceneMgr);
#endif

    // Updating GUI
    if (mNavigator->getNavigatorGUI() != 0)
        mNavigator->getNavigatorGUI()->update();

    // Updating Navi
    if (mNavigator->isNaviSupported())
        NaviManager::Get().Update();

    // Process received events
    if (mNavigator->isConnected())
        mNavigator->processEvents();

    // Update peers
    mNavigator->getOgrePeerManager()->frameStarted(evt);

    // Send events
    if (mNavigator->isConnected())
        mNavigator->sendEvents();

    // Updating sound
    if (mNavigator->getNavigatorSound() != 0)
        mNavigator->getNavigatorSound()->update();

    return OgreFrameListener::frameStarted(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::keyPressed(const KeyboardEvt& evt)
{ 
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();
    Modeler* modeler = mNavigator->getModeler();
    AvatarEditor* avatarEditor = mNavigator->getAvatarEditor();

#if 1 // GILLES MDLR
    mNavigator->computeGizmo();
#endif

    // Escape hits count to cancel focus Navi/VNC/...
    if (evt.mKey == KC_ESCAPE)
    {
        unsigned long now = Root::getSingleton().getTimer()->getMilliseconds();
        if (mEscapeHitsB4CancellingFocus == 0)
            mLastEscapeHitTimer = now;
        if (now - mLastEscapeHitTimer < ESCAPE_HITS_CANCEL_FOCUS_DURATION)
            mEscapeHitsB4CancellingFocus++;
        else
            mEscapeHitsB4CancellingFocus = 1;
        mLastEscapeHitTimer = now;
    }

    // RControl = Middle mouse for GlovePIE control mapping
    if (mNavigator->getNavigationInterface()!=Navigator::NIMouseKeyboard && evt.mKey == KC_RCONTROL)
    {
        MouseEvt mEvt;
        mEvt.mState.mButtons=MBMiddle;
        mNavigator->isOnRightCTRL = true;
        mousePressed (mEvt);
    }

    // Cancel focus on Navi ?
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused())
    {
        if (mEscapeHitsB4CancellingFocus >= ESCAPE_HITS_CANCEL_FOCUS)
        {
            mEscapeHitsB4CancellingFocus = 0;
            mNavigator->resetMousePicking();
            NaviManager::Get().deFocusAllNavis();
            return true;
        }
    }

    // Switching panels
    if ((navigatorGUI != 0) && (mNavigator->getState()!= Navigator::SLogin))
    {
        switch (evt.mKey)
        {
        case KC_F7: // Show/Hide chat panel
            GUI_Chat::showHide();
            break;
#ifdef UIDEBUG
        case KC_PAUSE: // Show/Hide debug panel
            GUI_Debug::switchPanel();
            break;
#endif
        }
    }

    // is modeling ?
    if (mNavigator->getState() == Navigator::SModeling && modeler != 0)
    {
        if (modeler->isOnGizmo())
        {
            switch (evt.mKey)
            {
            case KC_F9:
                modeler->lockGizmo(false);
                if (navigatorGUI != 0)
                {
                    GUI_Modeler::unload();
                    mNavigator->setCameraMode(mNavigator->getLastCameraMode());
                }
                return OgreFrameListener::keyPressed(evt);

            case KC_UP:
                mNavigator->MdlrModifGizmo(Vector3(.1,0,0));
                return OgreFrameListener::keyPressed(evt);
            case KC_W:
                if (mNavigator->isOnLeftCTRL)
                {
                    if( !modeler->isSelectionEmpty() )
                        modeler->getSelected()->undo();
                    return OgreFrameListener::keyPressed(evt);
                }
                else if (!mNavigator->isOnRightCTRL)
                {   
                    mNavigator->MdlrModifGizmo(Vector3(.1,0,0));
                    return OgreFrameListener::keyPressed(evt);
                }
                OgreFrameListener::keyPressed(evt);
                break;
                    
                OgreFrameListener::keyPressed(evt);
                break;   

            case KC_DOWN:
                mNavigator->MdlrModifGizmo(Vector3(-.1,0,0));
                return OgreFrameListener::keyPressed(evt);
            case KC_S:
                if (!mNavigator->isOnRightCTRL)
                {
                    mNavigator->MdlrModifGizmo(Vector3(-.1,0,0));
                    return OgreFrameListener::keyPressed(evt);
                }
                OgreFrameListener::keyPressed(evt);
                break;
            case KC_LEFT:
                mNavigator->MdlrModifGizmo(Vector3(0,0,-.1));
                return OgreFrameListener::keyPressed(evt);
            case KC_A:
                if (!mNavigator->isOnRightCTRL)
                {
                    mNavigator->MdlrModifGizmo(Vector3(0,0,-.1));
                    return OgreFrameListener::keyPressed(evt);
                }
                OgreFrameListener::keyPressed(evt);
                break;
            case KC_RIGHT:
                mNavigator->MdlrModifGizmo(Vector3(0,0,.1));
                return OgreFrameListener::keyPressed(evt);
            case KC_D:
                if (!mNavigator->isOnRightCTRL)
                {
                    mNavigator->MdlrModifGizmo(Vector3(0,0,.1));
                    return OgreFrameListener::keyPressed(evt);
                }
                OgreFrameListener::keyPressed(evt);
                break;
            case KC_PGUP:
                mNavigator->MdlrModifGizmo(Vector3(0,.1,0));
                return OgreFrameListener::keyPressed(evt);
            case KC_E:
                if (!mNavigator->isOnRightCTRL)
                {
                    mNavigator->MdlrModifGizmo(Vector3(0,.1,0));
                    return OgreFrameListener::keyPressed(evt);
                }
                OgreFrameListener::keyPressed(evt);
                break;
            case KC_PGDOWN:
            case KC_C:
                if (!mNavigator->isOnRightCTRL)
                {                
                    mNavigator->MdlrModifGizmo(Vector3(0,-.1,0));
                    return OgreFrameListener::keyPressed(evt);
                }
                OgreFrameListener::keyPressed(evt);
                break;
            }
        }

        switch (evt.mKey)
        {
        case KC_F9:
            if (modeler->isSelectionLocked() && !GUI_Modeler::isPanelVisible())
            {
                GUI_ModelerProperties::hidePanel();
                GUI_Modeler::createAndShowPanel();
            }
            else 
            {
                GUI_Modeler::unload();
                mNavigator->setCameraMode(mNavigator->getLastCameraMode());
            }
            return OgreFrameListener::keyPressed(evt);

        case KC_LCONTROL:
            modeler->getSelection()->set_lock( true );
            mNavigator->isOnLeftCTRL = true;
            return OgreFrameListener::keyPressed(evt);

        case KC_DELETE:
            if (!GUI_ModelerProperties::isPanelVisible())
            {
                if( !modeler->isSelectionEmpty() )
                {
                    // remove the current selection
                    modeler->removeSelection();
                    // reset mouse picking
                    mNavigator->resetMousePicking();
                }
                if (modeler->isSelectionLocked())
                {
                    GUI_ModelerProperties::unloadPanel();
                    GUI_Modeler::createAndShowPanel();
                }    
            }
            return OgreFrameListener::keyPressed(evt);

        case KC_W:
            if (!GUI_ModelerProperties::isPanelVisible())
            {
                if (mNavigator->isOnLeftCTRL) 
                {
                    if( !modeler->isSelectionEmpty() )
                        modeler->getSelected()->undo();
                    return OgreFrameListener::keyPressed(evt);
                }
            }
            OgreFrameListener::keyPressed(evt);
            break;
        }
    }

    // is editing the avatar ?
    else if (avatarEditor != 0)
    {
        switch (evt.mKey)
        {
        case KC_F8:
            if ((mNavigator->getState() == Navigator::SInWorld))
            {
                mNavigator->setCameraMode(Navigator::CMAroundPerson);
                GUI_Avatar::createAndShowPanel();
                return OgreFrameListener::keyPressed(evt);
            }
            else if (mNavigator->getState() == Navigator::SAvatarEdit)
            {
                GUI_Avatar::unload();
                mNavigator->setCameraMode(mNavigator->getLastCameraMode());
                return OgreFrameListener::keyPressed(evt);
            }
        }
    }

    // Navi focused -> key processed by the navi
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused())
        return true;

    GUI_ContextMenu::destroyMenu();

    // 2D Panels
    if ((Panel2DMgr::getSingleton().getFocusedPanel() != 0) && (mEscapeHitsB4CancellingFocus >= ESCAPE_HITS_CANCEL_FOCUS))
    {
        mEscapeHitsB4CancellingFocus = 0;
        Panel2DMgr::getSingleton().defocus();
        return true;
    }
    if (Panel2DMgr::getSingleton().keyPressed(evt))
        return true;

    // VNC panel ?
    if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFVNCPanel))
    {
        if (mEscapeHitsB4CancellingFocus >= ESCAPE_HITS_CANCEL_FOCUS)
        {
            mEscapeHitsB4CancellingFocus = 0;
            mNavigator->resetMousePicking();
            return true;
        }
        MovableObject* vncMovableObj = mNavigator->getPickedMovable();
        Entity* pickedEntity = static_cast<Entity*>(vncMovableObj->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
        ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
        Evt vncEvt;
        vncEvt.mType = evt.mType;
        vncEvt.mKeyboard = evt;
        vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
        return true;
    }
    // SWF object ?
    else if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFSWFPanel))
    {
        if (mEscapeHitsB4CancellingFocus >= ESCAPE_HITS_CANCEL_FOCUS)
        {
            mEscapeHitsB4CancellingFocus = 0;
            mNavigator->resetMousePicking();
            return true;
        }
        MovableObject* swfMovableObj = mNavigator->getPickedMovable();
        Entity* pickedEntity = static_cast<Entity*>(swfMovableObj->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("swf");
        ExternalTextureSourceEx* swfExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("swf"));
        Evt swfEvt;
        swfEvt.mType = evt.mType;
        swfEvt.mKeyboard = evt;
        swfExtTextSrc->handleEvt(mtlName, Event(0, &swfEvt));
        return true;
    }

    switch (evt.mKey)
    {
    case KC_ESCAPE:
        if (mNavigator->getState() != Navigator::SLogin)
        {
            if (mNavigator->getState()==Navigator::SModeling)
			{
				GUI_Modeler::unload();
                mNavigator->setCameraMode(mNavigator->getLastCameraMode());
			}
			else if (mNavigator->getState()==Navigator::SAvatarEdit)
			{
				GUI_Avatar::unload();
                mNavigator->setCameraMode(mNavigator->getLastCameraMode());
			}
			else
				mNavigator->disconnect(false);
            return true;
        }
        break;

    case KC_F8:
        if (navigatorGUI != 0)
            if(mNavigator->getState() == Navigator::SInWorld)
            {
                mNavigator->setCameraMode(Navigator::CMAroundPerson);
                GUI_Avatar::unload();
            }
            break;

    case KC_F9:
        if (navigatorGUI != 0)
            if(mNavigator->getState() == Navigator::SInWorld)
            {
                mNavigator->setCameraMode(Navigator::CMModeling);
                GUI_Modeler::createAndShowPanel();

            }
            break;

    case KC_F12:
        mBoundingBoxesShows = !mBoundingBoxesShows;
        mSceneMgr->showBoundingBoxes(mBoundingBoxesShows);
        break;
   
    }
//    Navigator::NavigationInterface navInterface mNavigator->getNavigationInterface();
    Avatar* userAvatar = mNavigator->getUserAvatar();
    if (userAvatar != 0)
    {
        switch (evt.mKey)
        {
        case KC_1: // Switch to 1st person camera
            mNavigator->setCameraMode(Navigator::CM1stPerson);
            break;
        case KC_2: // Switch to 1st person camera with mouse
            mNavigator->setCameraMode(Navigator::CM1stPersonWithMouse);
            break;
        case KC_3: // Switch to 3rd person camera
            mNavigator->setCameraMode(Navigator::CM3rdPerson);
            break;
        case KC_4: // Switch to TurnAround person camera
            mNavigator->setCameraMode(Navigator::CMAroundPerson);
            break;

		case KC_V:
            {
                switch (mNavigator->getCameraMode())
                {
                    case Navigator::CM1stPerson :
                    {
                        mNavigator->setCameraMode(Navigator::CM1stPersonWithMouse);
                        break;
                    }
                    case Navigator::CM1stPersonWithMouse :
                    {
                        mNavigator->setCameraMode(Navigator::CM3rdPerson);
                        break;
                    }
                    case Navigator::CM3rdPerson :
                    {
                        mNavigator->setCameraMode(Navigator::CMAroundPerson);
                        break;
                    }
                    case Navigator::CMAroundPerson :
                    {
                        if (mNavigator->getNavigationInterface() == Navigator::NIMouseKeyboard)
                            mNavigator->setCameraMode(Navigator::CM1stPerson);
                        else
                            mNavigator->setCameraMode(Navigator::CM1stPersonWithMouse);
                        break;
                    }
                }
            break;
            }
			
        case KC_UP:
            userAvatar->movementKeyPressed(KC_UP);
            break;
        case KC_W:
            if (mNavigator->isOnRightCTRL)
            {
                CameraSupport* camSupport=mNavigator->getMainCameraSupportManager()->getActiveCameraSupport();
                if (camSupport->getMode() == CameraSupport::CSMOrbital)
                    camSupport->pitch(Degree(-2.0));
                else if (camSupport->getMode() == CameraSupport::CSMFirstPerson)
                    mCamera->pitch(Degree(2.0));
            }
            else
                userAvatar->movementKeyPressed(KC_UP);
            break;

        case KC_DOWN:
            userAvatar->movementKeyPressed(KC_DOWN);
            break;
        case KC_S:
            if (mNavigator->isOnRightCTRL)
            {
                CameraSupport* camSupport=mNavigator->getMainCameraSupportManager()->getActiveCameraSupport();
                if (camSupport->getMode() == CameraSupport::CSMOrbital)
                    camSupport->pitch(Degree(2.0));
                else if (camSupport->getMode() == CameraSupport::CSMFirstPerson)
                    mCamera->pitch(Degree(-2.0));
            }
            else
                userAvatar->movementKeyPressed(KC_DOWN);
            break;

        case KC_LEFT:
            userAvatar->movementKeyPressed(KC_LEFT);
            break;
        case KC_A:
            if (mNavigator->isOnRightCTRL)
            {
                CameraSupport* camSupport=mNavigator->getMainCameraSupportManager()->getActiveCameraSupport();
                if (camSupport->getMode() == CameraSupport::CSMOrbital)
                    camSupport->yaw(Degree(-2.0));
                else if (camSupport->getMode() == CameraSupport::CSMFirstPerson)
                    userAvatar->yaw(Degree(2.0));
            }
            else
                userAvatar->movementKeyPressed(KC_LEFT);
            break;

        case KC_RIGHT:
            userAvatar->movementKeyPressed(KC_RIGHT);
            break;
        case KC_D:
            if (mNavigator->isOnRightCTRL)
            {
                CameraSupport* camSupport=mNavigator->getMainCameraSupportManager()->getActiveCameraSupport();
                if (camSupport->getMode() == CameraSupport::CSMOrbital)
                    camSupport->yaw(Degree(2.0));
                else if (camSupport->getMode() == CameraSupport::CSMFirstPerson)
                    userAvatar->yaw(Degree(-2.0));
            }
            else
                userAvatar->movementKeyPressed(KC_RIGHT);
            break;

        case KC_PGUP:
        case KC_E:
            userAvatar->movementKeyPressed(KC_PGUP);
            break;

        case KC_PGDOWN:
        case KC_C:
            userAvatar->movementKeyPressed(KC_PGDOWN);
            break;

        case KC_END:
            userAvatar->movementKeyPressed(KC_END);
            break;
        }
    }

    return OgreFrameListener::keyPressed(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::keyReleased(const KeyboardEvt& evt)
{ 
    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();

#if 1 // GILLES MDLR
    mNavigator->computeGizmo();
#endif

    // In modeler ?
    if (mNavigator->getState() == Navigator::SModeling)
    {
        switch (evt.mKey) 
        {
        case KC_LCONTROL:
            mNavigator->isOnLeftCTRL = false;
            mNavigator->getModeler()->getSelection()->set_lock( false );
            //return OgreFrameListener::keyReleased(evt);
            break;
        }
    }

    // RControl = Middle mouse for GlovePIE control mapping
    if (mNavigator->getNavigationInterface()!=Navigator::NIMouseKeyboard && evt.mKey == KC_RCONTROL)
    {
        MouseEvt mEvt;
        mEvt.mState.mButtons=MBMiddle;
        mNavigator->isOnRightCTRL = false;
        mouseReleased(mEvt);
    }

    // Navi focused -> key processed by the navi
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused() && mNavigator->getState() != Navigator::SAvatarEdit) 
        return true;

    // 2D Panels
    Panel2DMgr::getSingleton().keyReleased(evt);

    // VNC panel ?
    if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFVNCPanel))
    {
        MovableObject* vncMovableObj = mNavigator->getPickedMovable();
        Entity* pickedEntity = static_cast<Entity*>(vncMovableObj->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
        ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
        Evt vncEvt;
        vncEvt.mType = evt.mType;
        vncEvt.mKeyboard = evt;
        vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
        return true;
    }
    else if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFSWFPanel))
    {
        MovableObject* swfMovableObj = mNavigator->getPickedMovable();
        Entity* pickedEntity = static_cast<Entity*>(swfMovableObj->getParentSceneNode()->getAttachedObject(0));
        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("swf");
        ExternalTextureSourceEx* swfExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("swf"));
        Evt swfEvt;
        swfEvt.mType = evt.mType;
        swfEvt.mKeyboard = evt;
        swfExtTextSrc->handleEvt(mtlName, Event(0, &swfEvt));
        return true;
    }

    Avatar* userAvatar = mNavigator->getUserAvatar();
    if (userAvatar != 0)
    {
        switch (evt.mKey)
        {
        case KC_UP:
        case KC_W:
            userAvatar->movementKeyReleased(KC_UP);
            break;

        case KC_DOWN:
        case KC_S:
            userAvatar->movementKeyReleased(KC_DOWN);
            break;

        case KC_LEFT:
        case KC_A:
            userAvatar->movementKeyReleased(KC_LEFT);
            break;

        case KC_RIGHT:
        case KC_D:
            userAvatar->movementKeyReleased(KC_RIGHT);
            break;

        case KC_PGUP:
        case KC_E:
            userAvatar->movementKeyReleased(KC_PGUP);
            break;

        case KC_PGDOWN:
        case KC_C:
            userAvatar->movementKeyReleased(KC_PGDOWN);
            break;
        }
    }

    return OgreFrameListener::keyReleased(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::mouseMoved(const MouseEvt& evt)
{
	if (Mouse::getSingletonPtr() != 0)
        Mouse::getSingletonPtr()->move(evt.mState.mX, evt.mState.mY);

    // Updating Navi with the mouse motion
    // 3D picking of Navi panels if any NaviMaterial focused
    if (mNavigator->isNaviSupported())
    {
#if 1 // GILLES MDLR
        mNavigator->computeGizmo();
#endif

        if ((mNavigator->getState() == Navigator::SInWorld) &&
            NaviManager::Get().isAnyNaviFocused() 
            && NaviManager::Get().getFocusedNavi()->isMaterialOnly()
            && (Panel2DMgr::getSingleton().getFocusedPanel() == 0)
            && (mNavigator->getCameraMode() != Navigator::CM1stPerson))
        {
            std::string focusedNavi = NaviManager::Get().getFocusedNavi()->getName();
            if (evt.mState.mZrel != 0) NaviManager::Get().getFocusedNavi()->injectMouseWheel(evt.mState.mZrel);
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
            // Compute Navi panel mouse location
            Real closestDistance = -1.0f;
            Vector2 closestUV;
            Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
            int naviX, naviY;
            if (OgreHelpers::isEntityHitByMouse(mouseRay, mNavigator->getNaviEntity(focusedNavi),
                closestDistance,
                closestUV,
                closestTriUV0, closestTriUV1, closestTriUV2))
            {
                // compute texture coordinates of the hit
                mNavigator->computeNaviHit(focusedNavi,
                    closestUV,
                    closestTriUV0, closestTriUV1, closestTriUV2,
                    naviX, naviY);
                NaviManager::Get().getFocusedNavi()->injectMouseMove(naviX, naviY);
            }
        }
        else
            if (evt.mState.mZrel != 0) NaviManager::Get().injectMouseWheel(evt.mState.mZrel);

        // Here we call also the 2D version of injectMouseMove because it will refresh the mouse cursor !
        NaviManager::Get().injectMouseMove(evt.mState.mX, evt.mState.mY);
    }

    // 2D Panels
    Panel2DMgr::getSingleton().mouseMoved(evt);

    if (!GUI_ContextMenu::isContextVisible())
    {
        // VNC panel ?
        if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFVNCPanel))
        {
            MovableObject* vncMovableObj = mNavigator->getPickedMovable();
            Entity* pickedEntity = static_cast<Entity*>(vncMovableObj->getParentSceneNode()->getAttachedObject(0));
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
            // Compute VNC panel mouse location
            Real closestDistance = -1.0f;
            Vector2 closestUV;
            Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
            Vector2 vncXY;
            if (OgreHelpers::isEntityHitByMouse(mouseRay, pickedEntity,
                closestDistance,
                closestUV,
                closestTriUV0, closestTriUV1, closestTriUV2))
            {
                // compute texture coordinates of the hit
                mNavigator->computeVncHit(closestUV,
                    closestTriUV0, closestTriUV1, closestTriUV2,
                    vncXY);
                String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
                ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
                Evt vncEvt;
                vncEvt.mType = evt.mType;
                vncEvt.mMouse.mState = evt.mState;
                vncEvt.mMouse.mState.mXreal = vncXY.x;
                vncEvt.mMouse.mState.mYreal = vncXY.y;
                vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
            }
        }

        // SWF panel ?
        if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFSWFPanel))
        {
            MovableObject* swfMovableObj = mNavigator->getPickedMovable();
            Entity* pickedEntity = static_cast<Entity*>(swfMovableObj->getParentSceneNode()->getAttachedObject(0));
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
            // Compute SWF panel mouse location
            Real closestDistance = -1.0f;
            Vector2 closestUV;
            Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
            Vector2 swfXY;
            if (OgreHelpers::isEntityHitByMouse(mouseRay, pickedEntity,
                closestDistance,
                closestUV,
                closestTriUV0, closestTriUV1, closestTriUV2))
            {
                // compute texture coordinates of the hit
                mNavigator->computeSwfHit(closestUV,
                    closestTriUV0, closestTriUV1, closestTriUV2,
                    swfXY);
                String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("swf");
                ExternalTextureSourceEx* swfExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("swf"));
                Evt swfEvt;
                swfEvt.mType = evt.mType;
                swfEvt.mMouse.mState = evt.mState;
                swfEvt.mMouse.mState.mXreal = swfXY.x;
                swfEvt.mMouse.mState.mYreal = swfXY.y;
                swfExtTextSrc->handleEvt(mtlName, Event(0, &swfEvt));
            }
        }
    }

    CameraSupportManager* mainCSM = mNavigator->getMainCameraSupportManager();
    if (mainCSM==0)
        return false;

    // zoom and orbit camera in Modeling, 3rd Person or AvatarEdit Mode
    Real mouseWheel = evt.mState.mZrel;
    if (!NaviManager::Get().isAnyNaviFocused() &&
        (Panel2DMgr::getSingleton().getFocusedPanel() == 0) &&
        ((mNavigator->getCameraMode() == Navigator::CMAroundPerson) || (mNavigator->getCameraMode() == Navigator::CM3rdPerson) || (mNavigator->getCameraMode() == Navigator::CMModeling) || (mNavigator->getCameraMode() == Navigator::CMAroundObject)))
    {
		SceneNode *camNode = 0, *camPitchNode = 0, *camDistNode = 0, *camYawNode = 0;

        // zoom camera when the wheel mouse has changed
        if (!Ogre::Math::RealEqual(mouseWheel, 0))
        {
            if (mainCSM->getActiveCameraSupport()->getMode() == CameraSupport::CSMOrbital)
            {
                Vector3 pos;
			    Quaternion orientation;
                OrbitalCameraSupport* orbitalCameraSupport = (OrbitalCameraSupport*) mainCSM->getActiveCameraSupport();
    #if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
			    pos = mNavigator->getUserAvatar()->getSceneNode()->getWorldPosition();
			    orientation = mNavigator->getUserAvatar()->getSceneNode()->getWorldOrientation();
    #else
			    pos = mNavigator->getUserAvatar()->getSceneNode()->_getDerivedPosition();
			    orientation = mNavigator->getUserAvatar()->getSceneNode()->_getDerivedOrientation();
    #endif
			    Vector3 size = mNavigator->getUserAvatar()->getEntity()->getBoundingBox().getSize();

                // WARNING, here we force update of view by resetting the orientation
			    mCamera->setOrientation(Quaternion::IDENTITY);
			    Real DistFrontAvatar=0.0f;
                if (mNavigator->getCameraMode() == Navigator::CMModeling)
                    DistFrontAvatar=DIST_AVATAR_OBJECT;
                

#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
			    Vector3 posAbs = pos + orientation*Vector3(DistFrontAvatar, 0.5*size.y, 0);
                Vector3 camAbs = mCamera->getWorldPosition();
			    if (posAbs.squaredDistance(camAbs) >= 2.5)
                {
                    orbitalCameraSupport->moveToTarget(-mouseWheel*MOUSE_WHEEL_FACTOR);
                    camAbs = mCamera->getWorldPosition();
                    if (posAbs.squaredDistance(camAbs) < 2.5)
                    {
                        if (mNavigator->getCameraMode() == Navigator::CM3rdPerson)
                        {
                            if (mNavigator->getNavigationInterface() == Navigator::NIMouseKeyboard)
                                mNavigator->setCameraMode(Navigator::CM1stPerson);
                            else
                                mNavigator->setCameraMode(Navigator::CM1stPersonWithMouse);           
                        }
                        else
                            orbitalCameraSupport->moveToTarget(mouseWheel*MOUSE_WHEEL_FACTOR);
                    }
                }
#else
			    Vector3 posAbs = pos + orientation*Vector3(DistFrontAvatar, 0.5*size.y, 0);
                Vector3 camAbs = mCamera->getRealPosition();
			    if (posAbs.squaredDistance(camAbs) >= 2.5)
                {
                    orbitalCameraSupport->moveToTarget(-mouseWheel*MOUSE_WHEEL_FACTOR);
                    camAbs = mCamera->getRealPosition();
                    if (posAbs.squaredDistance(camAbs) < 2.5)
                    {
                        if (mNavigator->getCameraMode() == Navigator::CM3rdPerson)
                        {
                            if (mNavigator->getNavigationInterface() == Navigator::NIMouseKeyboard)
                                mNavigator->setCameraMode(Navigator::CM1stPerson);
                            else
                                mNavigator->setCameraMode(Navigator::CM1stPersonWithMouse);           
                        }
                        else
                            orbitalCameraSupport->moveToTarget(mouseWheel*MOUSE_WHEEL_FACTOR);
                    }
                }
#endif
            }
			return true;
        }



        // orbit camera when the middle click is pressed 
        if (mMouseMiddlePressed)
        {
            //apply the rotation around the avatar
            static Real yaw = 0;
            static Real pitch = 0;
            yaw = -mRotate*evt.mState.mXrel;
            pitch = mRotate*evt.mState.mYrel * .5;

            CameraSupport* activeCameraSupport = mainCSM->getActiveCameraSupport();
            // rotate around the avatar or object
			activeCameraSupport->yaw(Degree(yaw));
            //camNode->yaw(Degree(yaw));

            if (mNavigator->getCameraMode() == Navigator::CMModeling)
			{
                // turn the camera up and down around the object
				// limit the roll to -90� to +90
                Real camPitch =activeCameraSupport->getPitch().valueDegrees();
                if( -90. <= (camPitch+pitch) && (camPitch+pitch) <= 90. )
                     activeCameraSupport->pitch(Degree(pitch));   			
			}
			else
			{
				// look in front of / upper the avatar
				// limit the roll to -5� to -50
				Real camPitch = activeCameraSupport->getPitch().valueDegrees();
				if( -50. <= (camPitch+pitch) && (camPitch+pitch) <= -5. ) 
					activeCameraSupport->pitch(Degree(pitch));
			}
			return true;

        }
    }

    // modeling mode
    if (mNavigator->getState() == Navigator::SModeling &&
        !NaviManager::Get().isAnyNaviFocused() &&
        (Panel2DMgr::getSingleton().getFocusedPanel() == 0))
    {
        if (mNavigator->getCameraMode() == Navigator::CM1stPerson)
        {
            mNavigator->getUserAvatar()->getSceneNode()->yaw(Degree(-mRotate*evt.mState.mXrel));
            mCamNode->getChild(0)->pitch(Degree(mRotate*evt.mState.mYrel));
        }

        Modeler* modeler = Modeler::getSingletonPtr();
        Selection* selection = modeler->getSelection();
        Vector3 dragNdrop;

        if( mNavigator->isOnGizmo )//&& !selection->isEmpty() )
        {
            //Calculate drag and drop :
			Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
#if 1 // GILLES MDLR
            SceneNode *gizmoNode = mNavigator->getSceneMgrPtr()->getSceneNode("NodeSelection");
            Quaternion lYawGizmo(gizmoNode->getOrientation().getYaw(),Vector3::UNIT_Y);
            Quaternion lYawGizmoInverse(-gizmoNode->getOrientation().getYaw(),Vector3::UNIT_Y);
            //gizmoNode->setOrientation(Quaternion::IDENTITY);
            // dragNdrop = selection->mTransformation->drapNdrop(selection->mTransformation->getMousePosOnDummyPlane(mouseRay) );		

            //remove (&& selection->mTransformation->getAxeClicked() == Transformations::Z) to rotate in function of gizmo view
            if(selection->mTransformation->getMode() == Transformations::MOVE
                || (selection->mTransformation->getMode() == Transformations::ROTATE
                && selection->mTransformation->getAxeClicked() == Transformations::Z))
            {
                dragNdrop = selection->mTransformation->drapNdrop( lYawGizmoInverse * selection->mTransformation->getMousePosOnDummyPlane(mouseRay) );                   
                dragNdrop = lYawGizmo*dragNdrop;
            }
            else
                dragNdrop = selection->mTransformation->drapNdrop( lYawGizmoInverse * selection->mTransformation->getMousePosOnDummyPlane(mouseRay) );
#else
			dragNdrop = selection->mTransformation->drapNdrop( selection->mTransformation->getMousePosOnDummyPlane(mouseRay) );			
#endif
            if( dragNdrop != Vector3::ZERO )
            {
                //Apply the transformation
                switch( selection->mTransformation->getMode() )
                {
                case Transformations::MOVE :	//Move objects
                    mNavigator->MdlrModifGizmo( dragNdrop );
                    break;
                case Transformations::ROTATE :	//Rotate objects
                    mNavigator->MdlrModifGizmo( dragNdrop * 10. );
                    break;
                case Transformations::SCALE :	//Scale objects, but not gizmos
                    dragNdrop -= Vector3::UNIT_SCALE;
                    mNavigator->MdlrModifGizmo( dragNdrop * 100. );
                    break;
                }
            }
        }

        return true;
    }

    if (mNavigator->getState() != Navigator::SInWorld)// && mNavigator->getState() != Navigator::SAvatarEdit)
        return true;

    // if 1 NaviMaterial got focus then mouse wheel is not applied on camera 
    if (mNavigator->isNaviSupported() && NaviManager::Get().isAnyNaviFocused() && NaviManager::Get().getFocusedNavi()->isMaterialOnly())
        return true;

    // zoom with the wheel mouse
    if (!NaviManager::Get().isAnyNaviFocused() &&
        (Panel2DMgr::getSingleton().getFocusedPanel() == 0) &&
        !Ogre::Math::RealEqual(mouseWheel, 0))
    {
        if ((mNavigator->getCameraMode() == Navigator::CM1stPerson ||  
            (mNavigator->getCameraMode() == Navigator::CM1stPersonWithMouse ))
            && mouseWheel < 0.0f)
        {
            mNavigator->setCameraMode(Navigator::CM3rdPerson);
            if (mNavigator->getMainCameraSupportManager()->getActiveCameraSupport()->getMode()==CameraSupport::CSMOrbital)
            {
                OrbitalCameraSupport* thirdCamSupport=(OrbitalCameraSupport*)mNavigator->getMainCameraSupportManager()->getActiveCameraSupport();
                thirdCamSupport->moveToTarget(-mouseWheel*MOUSE_WHEEL_FACTOR);
            }
        }
    }

    if (mNavigator->getCameraMode() == Navigator::CM1stPerson || (mMouseMiddlePressed && ( mNavigator->getCameraMode() == Navigator::CM1stPersonWithMouse )))
    {
        mNavigator->getUserAvatar()->getSceneNode()->yaw(Degree(-mRotate*evt.mState.mXrel));
        mNavigator->getMainCameraSupportManager()->getActiveCameraSupport()->pitch(Degree(-mRotate*evt.mState.mYrel));
    }

	// Orbit around a right-clickable object 
	if (mMouseRightPressed)
	{
		// Initialisation
		if (mNavigator->getMainCameraSupportManager()->getActiveCameraSupportName() == "ThirdPersonCameraSupport"/*getCameraMode() == CM3rdPerson*/)
		{
			//Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
		}
	}
	else if (mNavigator->getMainCameraSupportManager()->getActiveCameraSupportName() == "AroundObjectCameraSupport"/*getCameraMode() == CMAroundObject*/) // We come back to the third person camera
	{

	}


    return true;
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::mousePressed(const MouseEvt& evt)
{
    if (evt.mState.mButtons & MBMiddle)
        mMouseMiddlePressed = true;

	if (evt.mState.mButtons & MBRight)
        mMouseRightPressed = true;

    if (evt.mState.mButtons & MBLeft)
        mMouseLeftPressed = true;

    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();
    if ((navigatorGUI != 0) && mNavigator->getNavigatorGUI()->isMouseVisible())
    {
        int buttonsId = (evt.mState.mButtons & MBLeft) ? LeftMouseButton : ((evt.mState.mButtons & MBRight) ? RightMouseButton : MiddleMouseButton);

        if (buttonsId == MiddleMouseButton)
        {
            // Defocus Navi if middle button pressed
            NaviManager::Get().deFocusAllNavis();
            // Defocus panels if middle button pressed
            Panel2DMgr::getSingleton().defocus();
        }
        else
        {
            if (Panel2DMgr::getSingleton().getFocusedPanel() != 0)
                Panel2DMgr::getSingleton().getFocusedPanel()->saveFocus();
            // Updating Navi with the mouse pressed
            NaviManager::Get().injectMouseDown(buttonsId);
        }

        if (GUI_ContextMenu::isContextVisible() && !GUI_ContextMenu::isContextFocused())
            GUI_ContextMenu::destroyMenu();

        // 2D Panels
        if (!NaviManager::Get().isAnyNaviFocused())
        {
            if (Panel2DMgr::getSingleton().getFocusedPanel() != 0)
                Panel2DMgr::getSingleton().getFocusedPanel()->restoreFocus();
            if (Panel2DMgr::getSingleton().mousePressed(evt))
                return OgreFrameListener::mousePressed(evt);
        }

        // 3D picking if no 2D panel focused
        MovableObject* previousPickedObj = mNavigator->getPickedMovable();
        mNavigator->resetMousePicking();
#if 1 // GILLES MDLR
        mNavigator->computeGizmo();
#endif
        if (!NaviManager::Get().isAnyNaviFocused() &&
            (Panel2DMgr::getSingleton().getFocusedPanel() == 0) &&
            (mNavigator->getState() == Navigator::SInWorld))
        {
            if ((evt.mState.mButtons & MBRight) && !GUI_ContextMenu::isContextVisible() &&
                mMouseMiddlePressed && (mNavigator->getMainCameraSupportManager()->getActiveCameraSupport()->getMode() == CameraSupport::CSMOrbital))
            {
                mNavigator->setCameraMode(mNavigator->getCameraMode());
            }
            else
            {
                // normalize (x, y) on 0..1 and get the ray emitted from the camera
                Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
                // compute the picking
                mNavigator->computeMousePicking(mouseRay);
                // Avatar/WWW Navi/VLC/VNC/SWF ?
                Avatar* avatar;
                String naviName;
                int naviX, naviY;
                MovableObject* movableObj = 0;
                Vector2 vncXY, swfXY;
                if (mNavigator->is1AvatarHitByMouse(avatar))
                {
                    if ((evt.mState.mButtons & MBRight) && !GUI_ContextMenu::isContextVisible())
                        GUI_ContextMenu::createAndShowPanel(evt.mState.mX, evt.mState.mY, GUI_ContextMenu::NAVI_CTXTAVATAR, "config#create#chat#talk");
                }
                else if (mNavigator->is1NaviHitByMouse(naviName, naviX, naviY))
                {
                    if ((evt.mState.mButtons & MBRight) && !GUI_ContextMenu::isContextVisible())
                        GUI_ContextMenu::createAndShowPanel(evt.mState.mX, evt.mState.mY, GUI_ContextMenu::NAVI_CTXTWWW, naviName);
                    else if (evt.mState.mButtons & MBLeft)
                    {
                        NaviLibrary::Navi* navi = NaviManager::Get().getNavi(naviName);
                        NaviManager::Get().focusNavi(navi);
                        navi->injectMouseDown(naviX, naviY);
                    }
                }
                else if (mNavigator->is1VLCHitByMouse(movableObj))
                {
                    if ((evt.mState.mButtons & MBRight) && !GUI_ContextMenu::isContextVisible())
                    {
                        Entity* pickedEntity = static_cast<Entity*>(movableObj->getParentSceneNode()->getAttachedObject(0));
                        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                        GUI_ContextMenu::createAndShowPanel(evt.mState.mX, evt.mState.mY, GUI_ContextMenu::NAVI_CTXTVLC, mtlName);
                    }
                }
                else if (mNavigator->is1VNCHitByMouse(movableObj, vncXY))
                {
                    if ((evt.mState.mButtons & MBRight) && !GUI_ContextMenu::isContextVisible() && (previousPickedObj != movableObj))
                    {
                        Entity* pickedEntity = static_cast<Entity*>(movableObj->getParentSceneNode()->getAttachedObject(0));
                        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                        GUI_ContextMenu::createAndShowPanel(evt.mState.mX, evt.mState.mY, GUI_ContextMenu::NAVI_CTXTVNC, mtlName);
                    }
                    else
                    {
                        Entity* pickedEntity = static_cast<Entity*>(movableObj->getParentSceneNode()->getAttachedObject(0));
                        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
                        ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
                        Evt vncEvt;
                        vncEvt.mType = evt.mType;
                        vncEvt.mMouse.mState = evt.mState;
                        vncEvt.mMouse.mState.mXreal = vncXY.x;
                        vncEvt.mMouse.mState.mYreal = vncXY.y;
                        vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
                    }
                }
                else if (mNavigator->is1SWFHitByMouse(movableObj, swfXY))
                {
                    if ((evt.mState.mButtons & MBRight) && !GUI_ContextMenu::isContextVisible() && (previousPickedObj != movableObj))
                    {
                        Entity* pickedEntity = static_cast<Entity*>(movableObj->getParentSceneNode()->getAttachedObject(0));
                        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                        GUI_ContextMenu::createAndShowPanel(evt.mState.mX, evt.mState.mY, GUI_ContextMenu::NAVI_CTXTSWF, mtlName);
                    }
                    else
                    {
                        Entity* pickedEntity = static_cast<Entity*>(movableObj->getParentSceneNode()->getAttachedObject(0));
                        String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                        ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("swf");
                        ExternalTextureSourceEx* swfExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("swf"));
                        Evt swfEvt;
                        swfEvt.mType = evt.mType;
                        swfEvt.mMouse.mState = evt.mState;
                        swfEvt.mMouse.mState.mXreal = swfXY.x;
                        swfEvt.mMouse.mState.mYreal = swfXY.y;
                        swfExtTextSrc->handleEvt(mtlName, Event(0, &swfEvt));
                    }
                }
            }
        }
        else if (!NaviManager::Get().isAnyNaviFocused() &&
            (Panel2DMgr::getSingleton().getFocusedPanel() == 0) &&
            ((mNavigator->getState() == Navigator::SModeling) ||
            (mNavigator->getState() == Navigator::SAvatarEdit))
            )
        {
            // the mouse is out of a naviPanel
            // normalize (x, y) on 0..1 and get the ray emitted from the camera
            Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
            // compute the picking
            mNavigator->computeMousePicking(mouseRay);
        }
        else
        {
            // the mouse is on a naviPanel
            return OgreFrameListener::mousePressed(evt);
        }
    }

    return OgreFrameListener::mousePressed(evt);
}

//-------------------------------------------------------------------------------------
bool NavigatorFrameListener::mouseReleased(const MouseEvt& evt)
{
	if (evt.mState.mButtons & MBMiddle)
		mMouseMiddlePressed = false;

	if (evt.mState.mButtons & MBRight)
		mMouseRightPressed = false;

    if (evt.mState.mButtons & MBLeft)
        mMouseLeftPressed = false;

    NavigatorGUI* navigatorGUI = mNavigator->getNavigatorGUI();
    if ((navigatorGUI != 0) && mNavigator->getNavigatorGUI()->isMouseVisible())
    {
        int buttonsId = (evt.mState.mButtons & MBLeft) ? LeftMouseButton : ((evt.mState.mButtons & MBRight) ? RightMouseButton : MiddleMouseButton);

#if 1 // GILLES MDLR
        mNavigator->computeGizmo();
#endif

        // Navi panels
        NaviManager::Get().injectMouseUp(buttonsId);

        // 2D Panels
        Panel2DMgr::getSingleton().mouseReleased(evt);

        if (!GUI_ContextMenu::isContextVisible())
        {
            // Updating Navi with the mouse released
            // 3D picking of Navi panels if any NaviMaterial focused
            if ((mNavigator->getState() == Navigator::SInWorld) &&
                NaviManager::Get().isAnyNaviFocused() && NaviManager::Get().getFocusedNavi()->isMaterialOnly() &&
                (Panel2DMgr::getSingleton().getFocusedPanel() == 0))
            {
                std::string focusedNavi = NaviManager::Get().getFocusedNavi()->getName();
                // normalize (x, y) on 0..1 and get the ray emitted from the camera
                Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
                // Compute Navi panel mouse location
                Real closestDistance = -1.0f;
                Vector2 closestUV;
                Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
                int naviX = 0, naviY = 0;
                if (OgreHelpers::isEntityHitByMouse(mouseRay, mNavigator->getNaviEntity(focusedNavi),
                    closestDistance,
                    closestUV,
                    closestTriUV0, closestTriUV1, closestTriUV2))
                {
                    // compute texture coordinates of the hit
                    mNavigator->computeNaviHit(focusedNavi,
                        closestUV,
                        closestTriUV0, closestTriUV1, closestTriUV2,
                        naviX, naviY);
                }
                NaviManager::Get().getFocusedNavi()->injectMouseUp(naviX, naviY);
            }
            // VNC panel ?
            else if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFVNCPanel))
            {
                MovableObject* vncMovableObj = mNavigator->getPickedMovable();
                Entity* pickedEntity = static_cast<Entity*>(vncMovableObj->getParentSceneNode()->getAttachedObject(0));
                // normalize (x, y) on 0..1 and get the ray emitted from the camera
                Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
                // Compute VNC panel mouse location
                Real closestDistance = -1.0f;
                Vector2 closestUV;
                Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
                Vector2 vncXY;
                if (OgreHelpers::isEntityHitByMouse(mouseRay, pickedEntity,
                    closestDistance,
                    closestUV,
                    closestTriUV0, closestTriUV1, closestTriUV2))
                {
                    // compute texture coordinates of the hit
                    mNavigator->computeVncHit(closestUV,
                        closestTriUV0, closestTriUV1, closestTriUV2,
                        vncXY);
                    String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                    ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("vnc");
                    ExternalTextureSourceEx* vncExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("vnc"));
                    Evt vncEvt;
                    vncEvt.mType = evt.mType;
                    vncEvt.mMouse.mState = evt.mState;
                    vncEvt.mMouse.mState.mXreal = vncXY.x;
                    vncEvt.mMouse.mState.mYreal = vncXY.y;
                    vncExtTextSrc->handleEvt(mtlName, Event(0, &vncEvt));
                }
            }
            // SWF panel ?
            else if (mNavigator->getPickedMovable() && (mNavigator->getPickedMovable()->getQueryFlags() & Navigator::QFSWFPanel))
            {
                MovableObject* swfMovableObj = mNavigator->getPickedMovable();
                Entity* pickedEntity = static_cast<Entity*>(swfMovableObj->getParentSceneNode()->getAttachedObject(0));
                // normalize (x, y) on 0..1 and get the ray emitted from the camera
                Ray mouseRay = mCamera->getCameraToViewportRay((Real)evt.mState.mX/(Real)mCamera->getViewport()->getActualWidth(), (Real)evt.mState.mY/(Real)mCamera->getViewport()->getActualHeight());
                // Compute SWF panel mouse location
                Real closestDistance = -1.0f;
                Vector2 closestUV;
                Vector2 closestTriUV0, closestTriUV1, closestTriUV2;
                Vector2 swfXY;
                if (OgreHelpers::isEntityHitByMouse(mouseRay, pickedEntity,
                    closestDistance,
                    closestUV,
                    closestTriUV0, closestTriUV1, closestTriUV2))
                {
                    // compute texture coordinates of the hit
                    mNavigator->computeSwfHit(closestUV,
                        closestTriUV0, closestTriUV1, closestTriUV2,
                        swfXY);
                    String mtlName = pickedEntity->getSubEntity(0)->getMaterialName();
                    ExternalTextureSourceManager::getSingleton().setCurrentPlugIn("swf");
                    ExternalTextureSourceEx* swfExtTextSrc = dynamic_cast<ExternalTextureSourceEx*>(ExternalTextureSourceManager::getSingleton().getExternalTextureSource("swf"));
                    Evt swfEvt;
                    swfEvt.mType = evt.mType;
                    swfEvt.mMouse.mState = evt.mState;
                    swfEvt.mMouse.mState.mXreal = swfXY.x;
                    swfEvt.mMouse.mState.mYreal = swfXY.y;
                    swfExtTextSrc->handleEvt(mtlName, Event(0, &swfEvt));
                }
            }
            else if (!NaviManager::Get().isAnyNaviFocused() &&
                (Panel2DMgr::getSingleton().getFocusedPanel() == 0) &&
                ((mNavigator->getState() == Navigator::SModeling) ||
                (mNavigator->getState() == Navigator::SAvatarEdit))
                )
            {
                if (mNavigator->getModeler()->isOnGizmo()) 
                    mNavigator->onMouseReleased(evt);
            }
        }
    }

    return OgreFrameListener::mouseReleased(evt);
}
