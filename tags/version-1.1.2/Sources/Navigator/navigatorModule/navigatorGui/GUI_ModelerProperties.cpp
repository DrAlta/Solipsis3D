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

#include "GUI_ModelerProperties.h"
#include "GUI_Modeler.h"
#include "GUI_MessageBox.h"


#include <CTLog.h>
#include <CTIO.h>
#include <CTSystem.h>
#include <CTStringHelpers.h>
#include <Navi.h>
#include <ctnetsocket.h>

#include "GUI_ChooseWorld.h"
#include "GUI_Options.h"
#include "GUI_AuthentFacebook.h"
#include "GUI_AuthentWorldServer.h"

using namespace Solipsis;
using namespace CommonTools;

GUI_ModelerProperties * GUI_ModelerProperties::stGUI_ModelerProperties = NULL;

//-------------------------------------------------------------------------------------
GUI_ModelerProperties::GUI_ModelerProperties() : GUI_Panel("uimdlrprop") ,
    mLockAmbientDiffuse(false)
{
    stGUI_ModelerProperties = this;
    mNavigator = Navigator::getSingletonPtr();
}

//-------------------------------------------------------------------------------------
/*static*/ bool GUI_ModelerProperties::createAndShowPanel()
{
    if (!stGUI_ModelerProperties)
    {
        new GUI_ModelerProperties();
    }

    return stGUI_ModelerProperties->show();
}

//-------------------------------------------------------------------------------------
/*static*/ void GUI_ModelerProperties::unloadPanel()
{
    if (!stGUI_ModelerProperties)
    {
        return;
    } 

    stGUI_ModelerProperties->destroy();
}

//-------------------------------------------------------------------------------------
/*static*/ bool GUI_ModelerProperties::isPanelVisible()
{
    return (stGUI_ModelerProperties && stGUI_ModelerProperties->isVisible());
}

//-------------------------------------------------------------------------------------
/*static*/ void GUI_ModelerProperties::hidePanel()
{
    if (!stGUI_ModelerProperties)
    {
        return;
    } 
    
    stGUI_ModelerProperties->hide();
}

//-------------------------------------------------------------------------------------
bool GUI_ModelerProperties::show()
{
    if (m_curState == NSNotCreated)
    {
        // Reset the remoteMRL on local IP address with UDP
        CommonTools::NetSocket::IPAddressVector myIPAddesses;
        if (!CommonTools::NetSocket::getMyIP(myIPAddesses))
            myIPAddesses.push_back("");

        std::string firstLocalIP = myIPAddesses.front();

        // Create Navi UI modeler
        createNavi(NaviPosition(TopRight), 512, 512);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_ModelerProperties::modelerPropPageLoaded));
        mNavi->loadFile("uimdlrprop.html?localIP=" + firstLocalIP);
        mNavi->setMovable(true);
        mNavi->hide();
        mNavi->setMask("uimdlrprop.png");//Eliminate the black shadow at the margin of the menu
        mNavi->setOpacity(0.75f);

        // page loaded
        mNavi->bind("pageClosed", NaviDelegate(this, &GUI_ModelerProperties::modelerPropPageClosed));

        // generic function for all the binds
        //mNavi->bind("MdlrProperties", NaviDelegate(this, &GUI_ModelerProperties::modelerProperties));

        // detect a changement on the properties tabber
        mNavi->bind("ClickOnTabber", NaviDelegate(this, &GUI_ModelerProperties::modelerTabberChange));
        mNavi->bind("ClickOnTabber", NaviDelegate(this, &GUI_ModelerProperties::modelerPropObjectName));

        // properties
        mNavi->bind("MdlrObjectName", NaviDelegate(this, &GUI_ModelerProperties::modelerPropObjectName));
        mNavi->bind("MdlrCreator", NaviDelegate(this, &GUI_ModelerProperties::modelerPropCreator));
        mNavi->bind("MdlrOwner", NaviDelegate(this, &GUI_ModelerProperties::modelerPropOwner));
        mNavi->bind("MdlrGroup", NaviDelegate(this, &GUI_ModelerProperties::modelerPropGroup));
        mNavi->bind("MdlrDescription", NaviDelegate(this, &GUI_ModelerProperties::modelerPropDescription));
        mNavi->bind("MdlrTags", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTags));
        mNavi->bind("MdlrModification", NaviDelegate(this, &GUI_ModelerProperties::modelerPropModification));
        mNavi->bind("MdlrCopy", NaviDelegate(this, &GUI_ModelerProperties::modelerPropCopy));
        // model
        mNavi->bind("MdlrTaperX", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTaperX));
        mNavi->bind("MdlrTaperY", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTaperY));
        mNavi->bind("MdlrTopShearX", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTopShearX));
        mNavi->bind("MdlrTopShearY", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTopShearY));
        mNavi->bind("MdlrTwistBegin", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTwistBegin));
        mNavi->bind("MdlrTwistEnd", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTwistEnd));
        mNavi->bind("MdlrDimpleBegin", NaviDelegate(this, &GUI_ModelerProperties::modelerPropDimpleBegin));
        mNavi->bind("MdlrDimpleEnd", NaviDelegate(this, &GUI_ModelerProperties::modelerPropDimpleEnd));
        mNavi->bind("MdlrPathBegin", NaviDelegate(this, &GUI_ModelerProperties::modelerPropPathCutBegin));
        mNavi->bind("MdlrPathEnd", NaviDelegate(this, &GUI_ModelerProperties::modelerPropPathCutEnd));
        mNavi->bind("MdlrHoleX", NaviDelegate(this, &GUI_ModelerProperties::modelerPropHoleSizeX));
        mNavi->bind("MdlrHoleY", NaviDelegate(this, &GUI_ModelerProperties::modelerPropHoleSizeY));
        mNavi->bind("MdlrHollowShape", NaviDelegate(this, &GUI_ModelerProperties::modelerPropHollowShape));
        mNavi->bind("MdlrSkew", NaviDelegate(this, &GUI_ModelerProperties::modelerPropSkew));
        mNavi->bind("MdlrRevolution", NaviDelegate(this, &GUI_ModelerProperties::modelerPropRevolution));
        mNavi->bind("MdlrRadiusDelta", NaviDelegate(this, &GUI_ModelerProperties::modelerPropRadiusDelta));
        mNavi->bind("ActionUndo", NaviDelegate(this, &GUI_ModelerProperties::modelerActionUndo));
        // material
        mNavi->bind("Ambient", NaviDelegate(this, &GUI_ModelerProperties::modelerColorAmbient));
        mNavi->bind("Diffuse", NaviDelegate(this, &GUI_ModelerProperties::modelerColorDiffuse));
        mNavi->bind("Specular", NaviDelegate(this, &GUI_ModelerProperties::modelerColorSpecular));
        mNavi->bind("MdlrLockAmbientDiffuse", NaviDelegate(this, &GUI_ModelerProperties::modelerColorLockAmbientDiffuse));
        mNavi->bind("MdlrDoubleSide", NaviDelegate(this, &GUI_ModelerProperties::modelerDoubleSide));
        mNavi->bind("MdlrShininess", NaviDelegate(this, &GUI_ModelerProperties::modelerPropShininess));
        mNavi->bind("MdlrTransparency", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTransparency));
        mNavi->bind("MdlrScrollU", NaviDelegate(this, &GUI_ModelerProperties::modelerPropScrollU));
        mNavi->bind("MdlrScrollV", NaviDelegate(this, &GUI_ModelerProperties::modelerPropScrollV));
        mNavi->bind("MdlrScaleU", NaviDelegate(this, &GUI_ModelerProperties::modelerPropScaleU));
        mNavi->bind("MdlrScaleV", NaviDelegate(this, &GUI_ModelerProperties::modelerPropScaleV));
        mNavi->bind("MdlrRotateU", NaviDelegate(this, &GUI_ModelerProperties::modelerPropRotateU));
        mNavi->bind("MdlrAddTexture", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTextureAdd));
        mNavi->bind("MdlrRemoveTexture", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTextureRemove));
        mNavi->bind("MdlrApplyTexture", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTextureApply));
        mNavi->bind("MdlrPrevTexture", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTexturePrev));
        mNavi->bind("MdlrNextTexture", NaviDelegate(this, &GUI_ModelerProperties::modelerPropTextureNext));
        mNavi->bind("MdlrApplyWWWTexture", NaviDelegate(this, &GUI_ModelerProperties::modelerPropWWWTextureApply));
        mNavi->bind("MdlrApplySWFTexture", NaviDelegate(this, &GUI_ModelerProperties::modelerPropSWFTextureApply));
        mNavi->bind("MdlrApplyVLCTexture", NaviDelegate(this, &GUI_ModelerProperties::modelerPropVLCTextureApply));
        mNavi->bind("MdlrVLCMrlBrowse", NaviDelegate(this, &GUI_ModelerProperties::modelerPropVLCMrlBrowse));
        mNavi->bind("MdlrApplyVNCTexture", NaviDelegate(this, &GUI_ModelerProperties::modelerPropVNCTextureApply));
        // 3D
        mNavi->bind("MdlrPositionX", NaviDelegate(this, &GUI_ModelerProperties::modelerPropPositionX));
        mNavi->bind("MdlrPositionY", NaviDelegate(this, &GUI_ModelerProperties::modelerPropPositionY));
        mNavi->bind("MdlrPositionZ", NaviDelegate(this, &GUI_ModelerProperties::modelerPropPositionZ));
        mNavi->bind("MdlrOrientationX", NaviDelegate(this, &GUI_ModelerProperties::modelerPropOrientationX));
        mNavi->bind("MdlrOrientationY", NaviDelegate(this, &GUI_ModelerProperties::modelerPropOrientationY));
        mNavi->bind("MdlrOrientationZ", NaviDelegate(this, &GUI_ModelerProperties::modelerPropOrientationZ));
        mNavi->bind("MdlrScaleX", NaviDelegate(this, &GUI_ModelerProperties::modelerPropScaleX));
        mNavi->bind("MdlrScaleY", NaviDelegate(this, &GUI_ModelerProperties::modelerPropScaleY));
        mNavi->bind("MdlrScaleZ", NaviDelegate(this, &GUI_ModelerProperties::modelerPropScaleZ));
        mNavi->bind("MdlrCollision", NaviDelegate(this, &GUI_ModelerProperties::modelerPropCollision));
        mNavi->bind("MdlrGravity", NaviDelegate(this, &GUI_ModelerProperties::modelerPropGravity));

        m_curState = NSCreated;
    }
    else //if(!isModelerMainVisible())
    {
        GUI_Panel::show();

        // Update the properties panel from the selected object datas
        modelerTabberLoad(1);
    }

    Modeler *modeler = mNavigator->getModeler();
    if (modeler)
    {
        modeler->lockSelection(true);

        if (!modeler->isSelectionEmpty())
        {
            // hide the gizmos axes
            modeler->lockGizmo(0);
            modeler->getSelection()->mTransformation->showGizmosMove(false);
            modeler->getSelection()->mTransformation->showGizmosRotate(false);
            modeler->getSelection()->mTransformation->showGizmosScale(false);
        }

        // Goto the tab 'Model' (1 on 0..6)
        mNavi->evaluateJS("document.getElementById('myTabber').tabber.tabShow(1)");
    }

    return true;
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::destroy()
{
    if (m_curState != NSNotCreated)
    {
        // Destroy Navi UI modeler
        mNavi->hide();
        NaviManager::Get().destroyNavi(mNavi);
        m_curState = NSNotCreated;

        // Update the selected objet dats from the properties panel
        modelerTabberSave();

        Modeler *modeler = mNavigator->getModeler();
        if (modeler)
            modeler->lockSelection(false);
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerActionUndo(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_Modeler::modelerActionUndo()");

    //mNavigator->undo();
    Modeler *modeler = mNavigator->getModeler();
    if (!modeler->isSelectionEmpty())
        modeler->getSelected()->undo();

    modelerUpdateDeformationSliders();
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropPageLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_ModelerProperties::modelerPropPageLoaded()");

    // Update the properties panel from the selected object datas
    modelerTabberLoad( 0 );

    // Show Navi UI
    if (m_curState == NSCreated)
        mNavi->show(true);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropPageClosed(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_ModelerProperties::modelerPropPageClosed()");

    if (mNavigator->getModeler()->isSelectionLocked() && !GUI_Modeler::isPanelVisible())
    {
        hide();
        GUI_Modeler::createAndShowPanel();
    }
    else 
        GUI_Modeler::unload();
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropObjectName(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('objectName').value").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
        obj->setName(value);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropCreator(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('creator').value").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
        obj->setOwner(value.c_str());
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropOwner(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('owner').value").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
        obj->setCreator(value.c_str());
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropGroup(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('group').value").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
        obj->setGroup(value.c_str());
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropDescription(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('description').value").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
        obj->setDesc(value.c_str());
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTags(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('tags').value").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
        obj->setTags(value.c_str());
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropModification(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('modification').checked").get().toString();
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropCopy(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('copy').checked").get().toString();
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTaperX(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("taperX.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::TAPERX, obj );
        obj->apply( Object3D::TAPERX, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTaperY(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("taperY.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::TAPERY, obj );
        obj->apply( Object3D::TAPERY, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTopShearX(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("topShearX.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::TOP_SHEARX, obj );
        obj->apply( Object3D::TOP_SHEARX, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTopShearY(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("topShearY.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::TOP_SHEARY, obj );
        obj->apply( Object3D::TOP_SHEARY, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTwistBegin(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("twistBegin.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::TWIST_BEGIN, obj );
        obj->apply( Object3D::TWIST_BEGIN, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTwistEnd(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("twistEnd.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::TWIST_END, obj );
        obj->apply( Object3D::TWIST_END, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropDimpleBegin(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("dimpleBegin.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::DIMPLE_BEGIN, obj );
        obj->apply( Object3D::DIMPLE_BEGIN, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropDimpleEnd(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("dimpleEnd.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::DIMPLE_END, obj );
        obj->apply( Object3D::DIMPLE_END, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropPathCutBegin(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("pathCutBegin.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::PATH_CUT_BEGIN, obj );
        obj->apply( Object3D::PATH_CUT_BEGIN, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropPathCutEnd(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("pathCutEnd.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::PATH_CUT_END, obj );
        obj->apply( Object3D::PATH_CUT_END, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropHoleSizeX(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("holeSizeX.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::HOLE_SIZEX, obj );
        obj->apply( Object3D::HOLE_SIZEX, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropHoleSizeY(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("holeSizeY.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::HOLE_SIZEY, obj );
        obj->apply( Object3D::HOLE_SIZEY, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropHollowShape(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = args[0].toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::HOLLOW_SHAPE, obj );
        obj->apply( Object3D::HOLLOW_SHAPE, atoi(value.c_str()) );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropSkew(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("skew.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::SKEW, obj );
        obj->apply( Object3D::SKEW, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropRevolution(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("revolution.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::REVOLUTION, obj );
        obj->apply( Object3D::REVOLUTION, atoi(value.c_str())/1. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropRadiusDelta(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("radiusDelta.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        modelerUpdateCommand( Object3D::RADIUS_DELTA, obj );
        obj->apply( Object3D::RADIUS_DELTA, atoi(value.c_str())/100. );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerColorAmbient(Navi* caller, const Awesomium::JSArguments& args)
{
    unsigned idRGB = 0;
    std::string str, color;
    int rgb[3]; 
    str = args[0].toString();

    for (unsigned id = 0; id < str.length(); id++)
    {
        if(str[id] != ',')
            color.insert(color.end(),str[id]);
        else
        {
            rgb[idRGB++] = atoi(color.c_str());
            color = "";
        }
    }
    rgb[idRGB] = atoi(color.c_str());

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        obj->setAmbient( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
        if(mLockAmbientDiffuse)
            obj->setDiffuse( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerColorDiffuse(Navi* caller, const Awesomium::JSArguments& args)
{
    unsigned idRGB = 0;
    std::string str, color;
    int rgb[3]; 
    str = args[0].toString();

    for (unsigned id = 0; id < str.length(); id++)
    {
        if(str[id] != ',')
            color.insert(color.end(),str[id]);
        else
        {
            rgb[idRGB++] = atoi(color.c_str());
            color = "";
        }
    }
    rgb[idRGB] = atoi(color.c_str());

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        obj->setDiffuse( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
        if(mLockAmbientDiffuse)
            obj->setAmbient( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerColorSpecular(Navi* caller, const Awesomium::JSArguments& args)
{
    unsigned idRGB = 0;
    std::string str, color;
    int rgb[3]; 
    str = args[0].toString();

    for (unsigned id = 0; id < str.length(); id++)
    {
        if(str[id] != ',')
            color.insert(color.end(),str[id]);
        else
        {
            rgb[idRGB++] = atoi(color.c_str());
            color = "";
        }
    }
    rgb[idRGB] = atoi(color.c_str());

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
        obj->setSpecular( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerColorLockAmbientDiffuse(Navi* caller, const Awesomium::JSArguments& args)
{ 
    std::string value = mNavi->evaluateJSWithResult("$('lockAmbientdiffuse').checked").get().toString();
    mLockAmbientDiffuse = (value == "1")?true:false;
}
//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerDoubleSide(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("$('doubleSide').checked").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {	
        MaterialPtr mat = obj->getMaterialManager()->getModifiedMaterial()->getOwner();
        mat->getTechnique(0)->getPass(0)->setCullingMode( (value == "true")?CULL_NONE:CULL_CLOCKWISE );
    }
}
//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropShininess(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("shininess.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
        obj->setShininess( atoi(value.c_str())/1.28 );
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTransparency(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("transparency.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        //        obj->setSceneBlendType(SBT_TRANSPARENT_ALPHA);
        obj->setAlpha( atoi(value.c_str())/100. );
        //		obj->getMaterialManager()->getModifiedMaterial()->getOwner()->getTechnique(0)->getPass(0)->setDepthWriteEnabled( false );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropScrollU(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("scrollU.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScroll();
        obj->setTextureScroll(atoi(value.c_str())/100. - .5, UV.y);
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropScrollV(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("scrollV.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScroll();
        obj->setTextureScroll(UV.x, atoi(value.c_str())/100. - .5);
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropScaleU(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("scaleU.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScale();
        obj->setTextureScale( atoi(value.c_str())/100. + .5, UV.y );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropScaleV(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("scaleV.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScale();
        obj->setTextureScale( UV.x, atoi(value.c_str())/100. + .5 );
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropRotateU(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("rotateU.getValue()").get().toString();

    Object3D *obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
        obj->setTextureRotate( Ogre::Radian(atoi(value.c_str())/100.*Math::TWO_PI) );
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTextureAdd(Navi* caller, const Awesomium::JSArguments& args)
{
    char * PathTexture = FileBrowser::displayWindowForLoading( 
        "Image Files (*.png;*.bmp;*.jpg)\0*.png;*.bmp;*.jpg\0", string("") ); 

    Modeler *modeler = mNavigator->getModeler();
    if (PathTexture != NULL && modeler != 0)
    {
        Object3D * obj = modeler->getSelected();
        String TextureFilePath (PathTexture);

        //Create the new OGRE texture with the file selected :
        Path filePath(TextureFilePath);
        ResourceGroupManager::getSingleton().addResourceLocation(filePath.getFormatedRootPath(), "FileSystem");
        TextureExtParamsMap textureExtParamsMap;
        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), TextureFilePath, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, textureExtParamsMap);

        //Test if this texture is already in the list :
        if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
        {
            GUI_MessageBox::getMsgBox()->show("Modeler error", 
                GUI_Modeler::ms_ModelerErrors[GUI_Modeler::ME_TEXTUREALREADYOPEN], 
                GUI_MessageBox::MBB_OK, 
                GUI_MessageBox::MBB_INFO);

            return;
        }

        //Add texture for the object (with obj->mModifiedMaterialManager)
        obj->addTexture(PtrTexture);
        // obj->getMaterialManager()->setNextTexture();


        modelerUpdateTextures();
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTextureRemove(Navi* caller, const Awesomium::JSArguments& args)
{
    Modeler *modeler = mNavigator->getModeler();
    if( modeler != 0 )
    {
        Object3D * obj = modeler->getSelected();

        //get selected texture :
        if( obj->getMaterialManager()->getNbTexture() > 1 )
        {
            TexturePtr tPtr = obj->getMaterialManager()->getCurrentTexture();
            obj->deleteTexture( tPtr );
        }

        modelerUpdateTextures();
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTextureApply(Navi* caller, const Awesomium::JSArguments& args)
{

    Modeler *modeler = mNavigator->getModeler();
    if( modeler != 0 )
    {
        Object3D * obj = modeler->getSelected();

        //get selected texture :
        if( obj->getMaterialManager()->getNbTexture() > 1 )
        {
            TexturePtr tPtr = obj->getMaterialManager()->getCurrentTexture();
            obj->setCurrentTexture( tPtr );
        }
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropWWWTextureApply(Navi* caller, const Awesomium::JSArguments& args)
{
    Modeler *modeler = mNavigator->getModeler();
    if( modeler != 0 )
    {
        Object3D * obj = modeler->getSelected();

        // only 1 WWW per modifiedMaterial for instance ... TODO
        ModifiedMaterialManager* modifiedMaterialManager = obj->getMaterialManager();
        TexturePtr texture;
        TextureVectorIterator tvIter = modifiedMaterialManager->getTextureIterator();
        while (tvIter.hasMoreElements())
        {
            texture = tvIter.getNext();
            TextureExtParamsMap* textureExtParamsMap = modifiedMaterialManager->getTextureExtParamsMap(texture);
            if (textureExtParamsMap == 0) continue;
            TextureExtParamsMap::const_iterator it = textureExtParamsMap->find("plugin");
            if (it == textureExtParamsMap->end()) continue;
            if (it->second == "www")
            {
                obj->deleteTexture(texture);
                break;
            }
        }

        std::string urlStr = mNavi->evaluateJSWithResult("$('MaterialWWWUrl').value").get().toString();
        std::string widthStr = mNavi->evaluateJSWithResult("$('MaterialWWWWidth').value").get().toString();
        std::string heightStr = mNavi->evaluateJSWithResult("$('MaterialWWWHeight').value").get().toString();
        std::string fpsStr = mNavi->evaluateJSWithResult("$('MaterialWWWFps').value").get().toString();
        int width = atoi(widthStr.c_str());
        int height = atoi(heightStr.c_str());
        int fps = atoi(fpsStr.c_str());
        bool sound3d = true;
        float sound3dMin = 0;
        float sound3dMax = 10;

        TextureExtParamsMap textureExtParamsMap;
        textureExtParamsMap["plugin"] = "www";
        textureExtParamsMap["query_flags"] = StringConverter::toString(Navigator::QFNaviPanel);
        textureExtParamsMap["url"] = urlStr;
        textureExtParamsMap["width"] = StringConverter::toString(width);
        textureExtParamsMap["height"] = StringConverter::toString(height);
        textureExtParamsMap["frames_per_second"] = StringConverter::toString(fps);
        textureExtParamsMap["sound_params"] = (sound3d ? "3d " + StringConverter::toString(sound3dMin) + " " + StringConverter::toString(sound3dMax) : "");
        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), "", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, textureExtParamsMap);

        //Test if this texture is already in the list :
        if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
        {
            GUI_MessageBox::getMsgBox()->show("Modeler error", GUI_Modeler::ms_ModelerErrors[GUI_Modeler::ME_TEXTUREALREADYOPEN], 
                GUI_MessageBox::MBB_OK, 
                GUI_MessageBox::MBB_INFO);
            return;
        }

        //Add texture for the object (with obj->mModifiedMaterialManager)
        obj->addTexture(PtrTexture, textureExtParamsMap);
        obj->setCurrentTexture(PtrTexture);

        //modelerUpdateTextures();
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropSWFMrlBrowse(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string mrl;
    if (System::showDlgOpenFilename(mrl, "Flash Media File,(*.swf)\0*.swf\0", ""))
    {
        
        std::string mrlStr(mrl);
        StringHelpers::replaceSubStr(mrlStr, "\\", "\\\\");
        mNavi->evaluateJS("$('MaterialSWFMrl').value='" + mrlStr + "'");
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropSWFTextureApply(Navi* caller, const Awesomium::JSArguments& args)
{
    Modeler *modeler = mNavigator->getModeler();
    if( modeler != 0 )
    {
        Object3D * obj = modeler->getSelected();
        if( obj == 0 ) return;

        // only 1 SWF per modifiedMaterial for instance ... TODO
        ModifiedMaterialManager* modifiedMaterialManager = obj->getMaterialManager();
        TexturePtr texture;
        TextureVectorIterator tvIter = modifiedMaterialManager->getTextureIterator();
        while (tvIter.hasMoreElements())
        {
            texture = tvIter.getNext();
            TextureExtParamsMap* textureExtParamsMap = modifiedMaterialManager->getTextureExtParamsMap(texture);
            if (textureExtParamsMap == 0) continue;
            TextureExtParamsMap::const_iterator it = textureExtParamsMap->find("plugin");
            if (it == textureExtParamsMap->end()) continue;
            if (it->second == "swf")
            {
                obj->deleteTexture(texture);
                break;
            }
        }

        std::string urlStr = mNavi->evaluateJSWithResult("$('MaterialSWFUrl').value").get().toString();
        std::string widthStr = mNavi->evaluateJSWithResult("$('MaterialSWFWidth').value").get().toString();
        std::string heightStr = mNavi->evaluateJSWithResult("$('MaterialSWFHeight').value").get().toString();
        std::string fpsStr = mNavi->evaluateJSWithResult("$('MaterialSWFFps').value").get().toString();
        std::string sp3dStr = mNavi->evaluateJSWithResult("$('MaterialSWFSP3d').checked").get().toString();
        std::string spMinStr = mNavi->evaluateJSWithResult("$('MaterialSWFSPMin').value").get().toString();
        std::string spMaxStr = mNavi->evaluateJSWithResult("$('MaterialSWFSPMax').value").get().toString();

        int width = atoi(widthStr.c_str());
        int height = atoi(heightStr.c_str());
        int fps = atoi(fpsStr.c_str());
        bool sound3d = (sp3dStr == "1")?true:false;
        float sound3dMin = atof(spMinStr.c_str());
        float sound3dMax = atof(spMaxStr.c_str());

        TextureExtParamsMap textureExtParamsMap;
        textureExtParamsMap["plugin"] = "swf";
        textureExtParamsMap["query_flags"] = StringConverter::toString(Navigator::QFSWFPanel);
        textureExtParamsMap["url"] = urlStr;
        textureExtParamsMap["width"] = StringConverter::toString(width);
        textureExtParamsMap["height"] = StringConverter::toString(height);
        textureExtParamsMap["frames_per_second"] = StringConverter::toString(fps);
        textureExtParamsMap["sound_params"] = (sound3d ? "3d " + StringConverter::toString(sound3dMin) + " " + StringConverter::toString(sound3dMax) : "");

        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), "", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, textureExtParamsMap);

        //Test if this texture is already in the list :
        if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
        {
            GUI_MessageBox::getMsgBox()->show("Modeler error", GUI_Modeler::ms_ModelerErrors[GUI_Modeler::ME_TEXTUREALREADYOPEN], 
                GUI_MessageBox::MBB_OK, 
                GUI_MessageBox::MBB_INFO);
            return;
        }

        //Add texture for the object (with obj->mModifiedMaterialManager)
        obj->addTexture(PtrTexture, textureExtParamsMap);
        obj->setCurrentTexture(PtrTexture);

        //modelerUpdateTextures();
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropVLCTextureApply(Navi* caller, const Awesomium::JSArguments& args)
{
    Modeler *modeler = mNavigator->getModeler();
    if( modeler != 0 )
    {
        Object3D * obj = modeler->getSelected();

        // only 1 VLC per modifiedMaterial for instance ... TODO
        ModifiedMaterialManager* modifiedMaterialManager = obj->getMaterialManager();
        TexturePtr texture;
        TextureVectorIterator tvIter = modifiedMaterialManager->getTextureIterator();
        while (tvIter.hasMoreElements())
        {
            texture = tvIter.getNext();
            TextureExtParamsMap* textureExtParamsMap = modifiedMaterialManager->getTextureExtParamsMap(texture);
            if (textureExtParamsMap == 0) continue;
            TextureExtParamsMap::const_iterator it = textureExtParamsMap->find("plugin");
            if (it == textureExtParamsMap->end()) continue;
            if (it->second == "vlc")
            {
                obj->deleteTexture(texture);
                break;
            }
        }

        std::string mrlStr = mNavi->evaluateJSWithResult("$('MaterialVLCMrl').value").get().toString();
        std::string widthStr = mNavi->evaluateJSWithResult("$('MaterialVLCWidth').value").get().toString();
        std::string heightStr = mNavi->evaluateJSWithResult("$('MaterialVLCHeight').value").get().toString();
        std::string fpsStr = mNavi->evaluateJSWithResult("$('MaterialVLCFps').value").get().toString();
        std::string paramsStr = mNavi->evaluateJSWithResult("$('MaterialVLCParams').value").get().toString();
        std::string remoteMrlStr = mNavi->evaluateJSWithResult("$('MaterialVLCRemoteMrl').value").get().toString();
        std::string sp3dStr = mNavi->evaluateJSWithResult("$('MaterialVLCSP3d').checked").get().toString();
        std::string spMinStr = mNavi->evaluateJSWithResult("$('MaterialVLCSPMin').value").get().toString();
        std::string spMaxStr = mNavi->evaluateJSWithResult("$('MaterialVLCSPMax').value").get().toString();
        int width = atoi(widthStr.c_str());
        int height = atoi(heightStr.c_str());
        int fps = atoi(fpsStr.c_str());
        bool sound3d = (sp3dStr == "1")?true:false;
        float sound3dMin = atof(spMinStr.c_str());
        float sound3dMax = atof(spMaxStr.c_str());

        std::string finalMrl = mrlStr;
        if (remoteMrlStr.empty() && (mrlStr.find("://") == String::npos))
        {
            Path p(mrlStr);
         
            // Create Temporary directory to save textures
            if (!IO::FolderExist("solTmpTexture\\"))
                if (!IO::createDirectory("solTmpTexture\\"))
                    GUI_MessageBox::getMsgBox()->show("Modeler error", "Unable to create temporary directory", 
                        GUI_MessageBox::MBB_OK, 
                        GUI_MessageBox::MBB_INFO);  
        
            finalMrl = std::string("solTmpTexture\\") + p.getLastFileName();
            if (mrlStr.find(finalMrl) == -1) // File is not already in solTmpTexture
            {
                if (!SOLcopyFile(mrlStr.c_str(), finalMrl.c_str()))
                {
                    GUI_MessageBox::getMsgBox()->show("Modeler error", "Unable to copy file to temporary directory", 
                        GUI_MessageBox::MBB_OK, 
                        GUI_MessageBox::MBB_INFO);                
                }
            }
        }

        TextureExtParamsMap textureExtParamsMap;
        textureExtParamsMap["plugin"] = "vlc";
        textureExtParamsMap["query_flags"] = StringConverter::toString(Navigator::QFVLCPanel);
        textureExtParamsMap["mrl"] = finalMrl;
        textureExtParamsMap["width"] = StringConverter::toString(width);
        textureExtParamsMap["height"] = StringConverter::toString(height);
        textureExtParamsMap["frames_per_second"] = StringConverter::toString(fps);
        textureExtParamsMap["vlc_params"] = paramsStr;
        textureExtParamsMap["remote_mrl"] = remoteMrlStr;
        textureExtParamsMap["sound_params"] = (sound3d ? "3d " + StringConverter::toString(sound3dMin) + " " + StringConverter::toString(sound3dMax) : "");
        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), "", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, textureExtParamsMap);

        //Test if this texture is already in the list :
        if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
        {
            GUI_MessageBox::getMsgBox()->show("Modeler error", GUI_Modeler::ms_ModelerErrors[GUI_Modeler::ME_TEXTUREALREADYOPEN], 
                GUI_MessageBox::MBB_OK, GUI_MessageBox::MBB_INFO);
            return;
        }

        //Add texture for the object (with obj->mModifiedMaterialManager)
        obj->addTexture(PtrTexture, textureExtParamsMap);
        obj->setCurrentTexture(PtrTexture);

        //modelerUpdateTextures();
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropVLCMrlBrowse(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string mrl;
    if (System::showDlgOpenFilename(mrl, "Media File,(*.*)\0*.*\0", ""))
    {
        std::string mrlStr(mrl);
        StringHelpers::replaceSubStr(mrlStr, "\\", "\\\\");
        mNavi->evaluateJS("$('MaterialVLCMrl').innerHTML='" + mrlStr + "'");
        mNavi->evaluateJS("$('MaterialVLCMrl').value='" + mrlStr + "'");
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropVNCTextureApply(Navi* caller, const Awesomium::JSArguments& args)
{
    Modeler *modeler = mNavigator->getModeler();
    if( modeler != 0 )
    {
        Object3D * obj = modeler->getSelected();

        // only 1 VNC per modifiedMaterial for instance ... TODO
        ModifiedMaterialManager* modifiedMaterialManager = obj->getMaterialManager();
        TexturePtr texture;
        TextureVectorIterator tvIter = modifiedMaterialManager->getTextureIterator();
        while (tvIter.hasMoreElements())
        {
            texture = tvIter.getNext();
            TextureExtParamsMap* textureExtParamsMap = modifiedMaterialManager->getTextureExtParamsMap(texture);
            if (textureExtParamsMap == 0) continue;
            TextureExtParamsMap::const_iterator it = textureExtParamsMap->find("plugin");
            if (it == textureExtParamsMap->end()) continue;
            if (it->second == "vnc")
            {
                obj->deleteTexture(texture);
                break;
            }
        }

        std::string hostStr = mNavi->evaluateJSWithResult("$('MaterialVNCHost').value").get().toString();
        std::string portStr = mNavi->evaluateJSWithResult("$('MaterialVNCPort').value").get().toString();
        std::string pwdStr = mNavi->evaluateJSWithResult("$('MaterialVNCPwd').value").get().toString();
        unsigned short port = atoi(portStr.c_str());
        std::string address = "vnc://" + hostStr + ":" + StringConverter::toString(port);
        std::string password = "vncpwd:" + pwdStr;

        TextureExtParamsMap textureExtParamsMap;
        textureExtParamsMap["plugin"] = "vnc";
        textureExtParamsMap["query_flags"] = StringConverter::toString(Navigator::QFVNCPanel);
        textureExtParamsMap["address"] = address;
        textureExtParamsMap["password"] = password;
        TexturePtr PtrTexture = modeler->loadTexture(obj->getMaterialManager(), "", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, textureExtParamsMap);

        //Test if this texture is already in the list :
        if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
        {
            GUI_MessageBox::getMsgBox()->show("Modeler error", GUI_Modeler::ms_ModelerErrors[GUI_Modeler::ME_TEXTUREALREADYOPEN], 
                GUI_MessageBox::MBB_OK, GUI_MessageBox::MBB_INFO);
            return;
        }

        //Add texture for the object (with obj->mModifiedMaterialManager)
        obj->addTexture(PtrTexture, textureExtParamsMap);
        obj->setCurrentTexture(PtrTexture);

        //modelerUpdateTextures();
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTexturePrev(Navi* caller, const Awesomium::JSArguments& args)
{
    Modeler *modeler = mNavigator->getModeler();
    if( modeler != 0 )
    {
        Object3D * obj = modeler->getSelected();
        if( obj != 0 )
        obj->getMaterialManager()->setPreviousTexture();
    }
    //modelerUpdateTextures();
    modelerPropTextureApply(caller, args);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropTextureNext(Navi* caller, const Awesomium::JSArguments& args)
{
    Modeler *modeler = mNavigator->getModeler();
    if( modeler != 0 )
    {
        Object3D * obj = modeler->getSelected();
        if( obj->getMaterialManager()->getNbTexture() > 1 )
            obj->getMaterialManager()->setNextTexture();
    }
    //modelerUpdateTextures();
    modelerPropTextureApply(caller, args);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropPositionX(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('positionX').value * 10000").get().toString();

    Modeler *modeler = mNavigator->getModeler();
    Vector3 vec = modeler->getSelection()->getCenterPosition();
    modeler->getSelection()->moveTo(atoi(value.c_str())/10000., vec.y, vec.z);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropPositionY(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('positionY').value * 10000").get().toString();

    Modeler *modeler = mNavigator->getModeler();
    Vector3 vec = modeler->getSelection()->getCenterPosition();
    modeler->getSelection()->moveTo(vec.x, atoi(value.c_str())/10000., vec.z);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropPositionZ(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('positionZ').value * 10000").get().toString();

    Modeler *modeler = mNavigator->getModeler();
    Vector3 vec = modeler->getSelection()->getCenterPosition();
    modeler->getSelection()->moveTo(vec.x, vec.y, atoi(value.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropOrientationX(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('orientationX').value * 10000").get().toString();

    Modeler *modeler = mNavigator->getModeler();
    modeler->getSelection()->rotateTo(atoi(value.c_str())/10000., 0, 0);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropOrientationY(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('orientationY').value * 10000").get().toString();

    Modeler *modeler = mNavigator->getModeler();
    modeler->getSelection()->rotateTo(0, atoi(value.c_str())/10000., 0);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropOrientationZ(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('orientationZ').value * 10000").get().toString();

    Modeler *modeler = mNavigator->getModeler();
    modeler->getSelection()->rotateTo(0, 0, atoi(value.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropScaleX(Navi* caller, const Awesomium::JSArguments& args)
{
    //std::string value = mNavi->evaluateJSWithResult("document.getElementById('scaleX').value * 10000");
    std::string valueX = mNavi->evaluateJSWithResult("document.getElementById('scaleX').value * 10000").get().toString();
    std::string valueY = mNavi->evaluateJSWithResult("document.getElementById('scaleY').value * 10000").get().toString();
    std::string valueZ = mNavi->evaluateJSWithResult("document.getElementById('scaleZ').value * 10000").get().toString();

    Modeler *modeler = mNavigator->getModeler();
    //modeler->getSelection()->scaleTo(atoi(value.c_str())/10000., 1., 1. );
    modeler->getSelection()->scaleTo(atoi(valueX.c_str())/10000., atoi(valueY.c_str())/10000., atoi(valueZ.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropScaleY(Navi* caller, const Awesomium::JSArguments& args)
{
    //std::string value = mNavi->evaluateJSWithResult("document.getElementById('scaleY').value * 10000");
    std::string valueX = mNavi->evaluateJSWithResult("document.getElementById('scaleX').value * 10000").get().toString();
    std::string valueY = mNavi->evaluateJSWithResult("document.getElementById('scaleY').value * 10000").get().toString();
    std::string valueZ = mNavi->evaluateJSWithResult("document.getElementById('scaleZ').value * 10000").get().toString();

    Modeler *modeler = mNavigator->getModeler();
    //modeler->getSelection()->scaleTo(1, atoi(value.c_str())/10000., 1);
    modeler->getSelection()->scaleTo(atoi(valueX.c_str())/10000., atoi(valueY.c_str())/10000., atoi(valueZ.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropScaleZ(Navi* caller, const Awesomium::JSArguments& args)
{
    //std::string value = mNavi->evaluateJSWithResult("document.getElementById('scaleZ').value * 10000");
    std::string valueX = mNavi->evaluateJSWithResult("document.getElementById('scaleX').value * 10000").get().toString();
    std::string valueY = mNavi->evaluateJSWithResult("document.getElementById('scaleY').value * 10000").get().toString();
    std::string valueZ = mNavi->evaluateJSWithResult("document.getElementById('scaleZ').value * 10000").get().toString();

    Modeler *modeler = mNavigator->getModeler();
    //modeler->getSelection()->scaleTo(1, 1, atoi(value.c_str())/10000.);
    modeler->getSelection()->scaleTo(atoi(valueX.c_str())/10000., atoi(valueY.c_str())/10000., atoi(valueZ.c_str())/10000.);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropCollision(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('collision').checked").get().toString();
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerPropGravity(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("document.getElementById('gravity').checked").get().toString();
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerTabberChange(Navi* caller, const Awesomium::JSArguments& args)
{
    unsigned tab = args[0].toInteger();
    modelerTabberLoad (tab);
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerTabberLoad(unsigned pTab)
{
    // get the current object3D
    Object3D* obj = mNavigator->getModeler()->getSelected();
    if( obj != 0 )
    {
        ColourValue col;
        Ogre::Vector2 UV;
        std::string text;
        unsigned c,e;

        switch( pTab )
        {
        case 0:	// properties tab
            mNavi->evaluateJS("document.getElementById('objectUid').value = '" + obj->getEntityUID() + "'");
            mNavi->evaluateJS("document.getElementById('objectName').value = '" + obj->getName() + "'");
            mNavi->evaluateJS("document.getElementById('creator').value = '" + obj->getCreator() + "'");
            mNavi->evaluateJS("document.getElementById('owner').value = '" + obj->getOwner() + "'");
            mNavi->evaluateJS("document.getElementById('group').value = '" + obj->getGroup() + "'");
            // TODO : remplacer tous les retours chariot par \\n
            text = obj->getDesc();
            for(c=0, e=0; e<text.length(); c++,e++)
            {
                if(text[c] == '\n' || text[c] == '\r') 
                {
                    text[c] = 'n';
                    text.insert(c, "\\");
                    c+=1;
                }
            }
            mNavi->evaluateJS("document.getElementById('description').value = '" + text + "'");
            // TODO : remplacer tous les retours chariot par \\n
            text = obj->getTags();
            for(c=0, e=0; e<text.length(); c++,e++)
            {
                if(text[c] == '\n' || text[c] == '\r') 
                {
                    text[c] = 'n';
                    text.insert(c, "\\");
                    c+=1;
                }
            }
            mNavi->evaluateJS("document.getElementById('tags').value = '" + text + "'");
            mNavi->evaluateJS("document.getElementById('modification').checked = " + obj->getCanBeModified()?"true":"false");
            mNavi->evaluateJS("document.getElementById('copy').checked = " + obj->getCanBeCopied()?"true":"false");
  			modelerUpdateTextures();
            break;
        case 1:	// model
            modelerUpdateDeformationSliders();
            break;
        case 2:	// material tab
            {
                char color[32];

                sprintf(color, "%.2X%.2X%.2X", obj->getAmbient().r * 255, obj->getAmbient().g * 255, obj->getAmbient().b * 255);
                mNavi->evaluateJS("$S('pAmbient').background='#" + String(color) + "'");

                sprintf(color, "%.2X%.2X%.2X", obj->getDiffuse().r * 255, obj->getDiffuse().g * 255, obj->getDiffuse().b * 255);
                mNavi->evaluateJS("$S('pDiffuse').background='#" + String(color) + "'");

                sprintf(color, "%.2X%.2X%.2X", obj->getSpecular().r * 255, obj->getSpecular().g * 255, obj->getSpecular().b * 255);
                mNavi->evaluateJS("$S('pSpecular').background='#" + String(color) + "'");

                mNavi->evaluateJS("shininess.onchange = function() {}");
                mNavi->evaluateJS("transparency.onchange = function() {}");
                mNavi->evaluateJS("scrollU.onchange = function() {}");
                mNavi->evaluateJS("scrollV.onchange = function() {}");
                mNavi->evaluateJS("scaleU.onchange = function() {}");
                mNavi->evaluateJS("scaleV.onchange = function() {}");
                mNavi->evaluateJS("rotateU.onchange = function() {}");

                mNavi->evaluateJS("shininess.setValue(" + StringConverter::toString(Real(obj->getShininess()/128.)*100) + ")");
                mNavi->evaluateJS("transparency.setValue(" + StringConverter::toString(obj->getAlpha()*100) + ")");
                UV = obj->getMaterialManager()->getTextureScroll();
                mNavi->evaluateJS("scrollU.setValue(" + StringConverter::toString(UV.x*100+50) + ")");
                mNavi->evaluateJS("scrollV.setValue(" + StringConverter::toString(UV.y*100+50) + ")");
                UV = obj->getMaterialManager()->getTextureScale();
                mNavi->evaluateJS("scaleU.setValue(" + StringConverter::toString(UV.x*100-50) + ")");
                mNavi->evaluateJS("scaleV.setValue(" + StringConverter::toString(UV.y*100-50) + ")");
                mNavi->evaluateJS("rotateU.setValue(" + StringConverter::toString(obj->getMaterialManager()->getTextureRotate()/Math::TWO_PI*100) + ")");

                mNavi->evaluateJS("shininess.onchange = function() {elementClicked('MdlrShininess')}");
                mNavi->evaluateJS("transparency.onchange = function() {elementClicked('MdlrTransparency')}");
                mNavi->evaluateJS("scrollU.onchange = function() {elementClicked('MdlrScrollU')}");
                mNavi->evaluateJS("scrollV.onchange = function() {elementClicked('MdlrScrollV')}");
                mNavi->evaluateJS("scaleU.onchange = function() {elementClicked('MdlrScaleU')}");
                mNavi->evaluateJS("scaleV.onchange = function() {elementClicked('MdlrScaleV')}");
                mNavi->evaluateJS("rotateU.onchange = function() {elementClicked('MdlrRotateU')}");
                {
                    MaterialPtr mat = obj->getMaterialManager()->getModifiedMaterial()->getOwner();
                    CullingMode mode = mat->getTechnique(0)->getPass(0)->getCullingMode();
                    mNavi->evaluateJS("document.getElementById('doubleSide').checked = " + (mode == CULL_NONE)?"true":"false" );
                }
                //modelerUpdateTextures();
            }
            break;
        case 3:	// 3D tab
            mNavi->evaluateJS("document.getElementById('positionX').value = " + StringConverter::toString(obj->getPosition().x));
            mNavi->evaluateJS("document.getElementById('positionY').value = " + StringConverter::toString(obj->getPosition().y));
            mNavi->evaluateJS("document.getElementById('positionZ').value = " + StringConverter::toString(obj->getPosition().z));
            mNavi->evaluateJS("document.getElementById('orientationX').value = " + StringConverter::toString(obj->getRotate().x));
            mNavi->evaluateJS("document.getElementById('orientationY').value = " + StringConverter::toString(obj->getRotate().y));
            mNavi->evaluateJS("document.getElementById('orientationZ').value = " + StringConverter::toString(obj->getRotate().z));
            mNavi->evaluateJS("document.getElementById('scaleX').value = " + StringConverter::toString(obj->getScale().x));
            mNavi->evaluateJS("document.getElementById('scaleY').value = " + StringConverter::toString(obj->getScale().y));
            mNavi->evaluateJS("document.getElementById('scaleZ').value = " + StringConverter::toString(obj->getScale().z));
            mNavi->evaluateJS("document.getElementById('gravity').checked = " + obj->getEnableGravity()?"true":"false");
            mNavi->evaluateJS("document.getElementById('collision').checked = %s" + obj->getCollisionnable()?"true":"false");

            text = "document.getElementById('info').value = 'Vertex count : '+";
            text += StringConverter::toString(obj->getVertexCount());
            text += "+'\\nTriangle count : '+";
            text += StringConverter::toString(obj->getTriCount());
            text += "+'\\nPrimitives count : '+";
            text += StringConverter::toString(obj->getPrimitivesCount());
            text += "+'\\nMesh size : '+";
            text += StringConverter::toString(obj->getMeshSize().x);
            text += "+','+";
            text += StringConverter::toString(obj->getMeshSize().y);
            text += "+','+";
            text += StringConverter::toString(obj->getMeshSize().z);
            mNavi->evaluateJS(text);
            break;
        case 4:	// ...
            break;
        case 5:	// ...
            break;
        case 6: // ...
            break;
        }
        //mNaviMgr->Update();
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerTabberSave()
{
    /*
    std::string value;
    

    // properties tab
    value = mNavi->evaluateJS("document.getElementById('objectName').value");
    value = mNavi->evaluateJS("document.getElementById('creator').value");
    value = mNavi->evaluateJS("document.getElementById('owner').value");
    value = mNavi->evaluateJS("document.getElementById('group').value");
    value = mNavi->evaluateJS("document.getElementById('description').value");
    value = mNavi->evaluateJS("document.getElementById('tags').value");
    value = mNavi->evaluateJS("document.getElementById('modification').checked");
    value = mNavi->evaluateJS("document.getElementById('copy').checked");
    // model
    // (none)
    // material tab
    // (none)
    // 3D tab
    value = mNavi->evaluateJS("document.getElementById('positionX').value");
    value = mNavi->evaluateJS("document.getElementById('positionY').value");
    value = mNavi->evaluateJS("document.getElementById('positionZ').value");
    value = mNavi->evaluateJS("document.getElementById('orientationX').value");
    value = mNavi->evaluateJS("document.getElementById('orientationX').value");
    value = mNavi->evaluateJS("document.getElementById('orientationZ').value");
    value = mNavi->evaluateJS("document.getElementById('scaleX').value");
    value = mNavi->evaluateJS("document.getElementById('scaleY').value");
    value = mNavi->evaluateJS("document.getElementById('scaleZ').checked");
    value = mNavi->evaluateJS("document.getElementById('gravity').checked");
    value = mNavi->evaluateJS("document.getElementById('collision').value");
    */
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerUpdateCommand(Object3D::Command pCommand, Object3D* pObject3D)
{
    if( mNavigator->getModeler()->updateCommand( pCommand, pObject3D ) )
    {
        modelerUpdateDeformationSliders();

        // TODO 
        //modelerAddNewDeformation( cmdOld );	// add new button to the deformation stack
        // OR ??
        //modelerAddNewDeformation( cmdNew );	// add new button to the deformation stack
    }
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerUpdateDeformationSliders()
{
    Object3D* obj = mNavigator->getModeler()->getSelected();

    // break the callback from the interface sliders
    mNavi->evaluateJS("taperX.onchange = function() {}");
    mNavi->evaluateJS("taperY.onchange = function() {}");
    mNavi->evaluateJS("topShearX.onchange = function() {}");
    mNavi->evaluateJS("topShearY.onchange = function() {}");
    mNavi->evaluateJS("twistBegin.onchange = function() {}");
    mNavi->evaluateJS("twistEnd.onchange = function() {}");
    mNavi->evaluateJS("dimpleBegin.onchange = function() {}");
    mNavi->evaluateJS("dimpleEnd.onchange = function() {}");
    mNavi->evaluateJS("pathCutBegin.onchange = function() {}");
    mNavi->evaluateJS("pathCutEnd.onchange = function() {}");
    mNavi->evaluateJS("holeSizeX.onchange = function() {}");
    mNavi->evaluateJS("holeSizeY.onchange = function() {}");
    mNavi->evaluateJS("skew.onchange = function() {}");
    mNavi->evaluateJS("revolution.onchange = function() {}");
    mNavi->evaluateJS("radiusDelta.onchange = function() {}");

    // properties tab
    mNavi->evaluateJS("taperX.setValue(" + StringConverter::toString(obj->getTaperX()*100) + ")");
    mNavi->evaluateJS("taperY.setValue(" + StringConverter::toString(obj->getTaperY()*100) + ")");
    mNavi->evaluateJS("topShearX.setValue(" + StringConverter::toString(obj->getTopShearX()*100) + ")");
    mNavi->evaluateJS("topShearY.setValue(" + StringConverter::toString(obj->getTopShearY()*100) + ")");
    mNavi->evaluateJS("twistBegin.setValue(" + StringConverter::toString(obj->getTwistBegin()*100) + ")");
    mNavi->evaluateJS("twistEnd.setValue(" + StringConverter::toString(obj->getTwistEnd()*100) + ")");
    mNavi->evaluateJS("dimpleBegin.setValue(" + StringConverter::toString(obj->getDimpleBegin()*100) + ")");
    mNavi->evaluateJS("dimpleEnd.setValue(" + StringConverter::toString(obj->getDimpleEnd()*100) + ")");
    mNavi->evaluateJS("pathCutBegin.setValue(" + StringConverter::toString(obj->getPathCutBegin()*100) + ")");
    mNavi->evaluateJS("pathCutEnd.setValue(" + StringConverter::toString(obj->getPathCutEnd()*100) + ")");
    mNavi->evaluateJS("holeSizeX.setValue(" + StringConverter::toString(obj->getHoleSizeX()*100) + ")");
    mNavi->evaluateJS("holeSizeY.setValue(" + StringConverter::toString(obj->getHoleSizeY()*100) + ")");

    // TODO : uncomment this ->		mNavi->evaluateJS("document.getElementById('hollowShape').value = '1');"
    mNavi->evaluateJS("skew.setValue(" + StringConverter::toString(obj->getSkew()*100) + ")");
    mNavi->evaluateJS("revolution.setValue(" + StringConverter::toString(obj->getRevolutions()*1) + ")");
    mNavi->evaluateJS("radiusDelta.setValue(" + StringConverter::toString(obj->getRadiusDelta()*100) + ")");

    // link the callback from the interface sliders
    mNavi->evaluateJS("taperX.onchange = function() {elementClicked('MdlrTaperX')}");
    mNavi->evaluateJS("taperY.onchange = function() {elementClicked('MdlrTaperY')}");
    mNavi->evaluateJS("topShearX.onchange = function() {elementClicked('MdlrTopShearX')}");
    mNavi->evaluateJS("topShearY.onchange = function() {elementClicked('MdlrTopShearY')}");
    mNavi->evaluateJS("twistBegin.onchange = function() {elementClicked('MdlrTwistBegin')}");
    mNavi->evaluateJS("twistEnd.onchange = function() {elementClicked('MdlrTwistEnd')}");
    mNavi->evaluateJS("dimpleBegin.onchange = function() {elementClicked('MdlrDimpleBegin')}");
    mNavi->evaluateJS("dimpleEnd.onchange = function() {elementClicked('MdlrDimpleEnd')}");
    mNavi->evaluateJS("pathCutBegin.onchange = function() {elementClicked('MdlrPathBegin')}");
    mNavi->evaluateJS("pathCutEnd.onchange = function() {elementClicked('MdlrPathEnd')}");
    mNavi->evaluateJS("holeSizeX.onchange = function() {elementClicked('MdlrHoleX')}");
    mNavi->evaluateJS("holeSizeY.onchange = function() {elementClicked('MdlrHoleY')}");
    mNavi->evaluateJS("skew.onchange = function() {elementClicked('MdlrSkew')}");
    mNavi->evaluateJS("revolution.onchange = function() {elementClicked('MdlrRevolution')}");
    mNavi->evaluateJS("radiusDelta.onchange = function() {elementClicked('MdlrRadiusDelta')}");
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerUpdateTextures()
{
    Modeler *modeler = mNavigator->getModeler();
    Object3D* obj = modeler->getSelected();

    std::string texturePath, text;
    String str;
    Ogre::Image image;

    // Go back to the main directory
    _chdir(modeler->mExecPath.c_str());

    // create a temporary forlder for the thumbnail textures
#ifdef WIN32
    CreateDirectory( "NaviLocal\\NaviTmpTexture", NULL );
#else
    system( "md NaviLocal\\NaviTmpTexture" );
#endif






    /*
    // update Navi interface
    TextureExtParamsMap* textureExtParamsMap2 = obj->getMaterialManager()->getTextureExtParamsMap(obj->getMaterialManager()->getCurrentTexture());
    if (textureExtParamsMap2)
    {
    TextureExtParamsMap::iterator it = textureExtParamsMap2->find("plugin");
    if ((it->second == "vlc") || (it->second == "swf") || (it->second == "www"))
    text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(__" + it->second + ".jpg)')";
    else
    text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(__default_color.jpg)')";    
    }
    else // It is a picture
    {
    texturePath = obj->getMaterialManager()->getCurrentTexture()->getName();
    Path path(texturePath);
    size_t begin = path.getFormatedPath().find_last_of( '\\' );
    size_t end = path.getFormatedPath().find_last_of( '.' );
    std::string fileName( path.getFormatedPath(), begin+1, end-begin-1 );
    fileName += ".jpg";

    str = ResourceGroupManager::getSingleton().findGroupContainingResource(texturePath);

    //"<img src='file:///d:\\test.jpg";
    text += "<img src='./solTmpTexture/";
    text += fileName;
    text +=	"' width=128 height=128/>";

    vector<std::string> files;
    SOLlistDirectoryFiles( "NaviLocal\\NaviTmpTexture\\", &files );
    vector<std::string>::iterator iter = files.begin();
    bool found = false;
    while( iter != files.end() )
    {
    if( (*iter) ==  fileName )
    {
    found = true;
    break;
    }
    iter++;
    }
    files.clear();
    if( !found )
    {
    image.load( texturePath, str);
    image.resize( 128, 128 );
    image.save( "NaviLocal\\NaviTmpTexture\\" + fileName );
    }
    text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(./NaviTmpTexture/" + fileName + ")')";    
    }
    mNavi->evaluateJS(text);
    */




    /*
    text = "textTabTextures = \"";
    text += "<img src='./color/blank.jpg' width=128 height=128/>	";
    text += "<img src='file:///z:/5.png' width=128 height=128/>	";
    text += "\"";
    */

    TexturePtr texture; 
    // update Navi interface
    text = "textTabTextures = \"";

    //text += "<img src='./NaviLocal/__default_color.jpg' width=128 height=128/>";
    text += "<img src='./__default_color.jpg' width=100 height=100/> ";

    for(int t=1; t<obj->getMaterialManager()->getNbTexture(); t++)
    {
        texture = obj->getMaterialManager()->getTexture(t);
        TextureExtParamsMap* textureExtParamsMap = obj->getMaterialManager()->getTextureExtParamsMap(texture);
        if (textureExtParamsMap != 0)
        {
            //text += "<img src='./NaviLocal/";
            //text += (*textureExtParamsMap)["type"] + "_texture.jpg";
            //text +=	"' width=128 height=128/>";

            TextureExtParamsMap::iterator it = textureExtParamsMap->find("plugin");
            if ((it->second == "vlc") || (it->second == "swf") || (it->second == "www"))
            {
                //text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(__" + it->second + ".jpg)')";
                //text += "<img src='./NaviLocal/__";
                text += "<img src='./__";
                //text += (*textureExtParamsMap)["type"] + "_texture.jpg";
                text += it->second;
                text +=	".jpg' width=100 height=100/> ";
            }
            /*
            else
            {
            //text = "document.getElementById('imgSelec').setAttribute('style','background-image:url(__default_color.jpg)')";    
            //text += "<img src='./NaviLocal/__default_color.jpg' width=128 height=128/>";
            text += "<img src='./__default_color.jpg' width=128 height=128/>";
            }
            */
        }
        else
        {
            texturePath = texture->getName();
            Path path(texturePath);
            size_t begin = path.getFormatedPath().find_last_of( '\\' );
            size_t end = path.getFormatedPath().find_last_of( '.' );
            std::string fileName( path.getFormatedPath(), begin+1, end-begin-1 );
            fileName += ".jpg";

            str = ResourceGroupManager::getSingleton().findGroupContainingResource(texturePath);

            //text += "<img src='file:///d:\\test.jpg";
            text += "<img src='./NaviTmpTexture/";
#if 1 // GILLES
            /* JAVASCRIPT version
            var iChars = "!@#$%^&*()+=-[]\\\';,./{}|\":<>?~_"; 
            for (var i = 0; i < data.length; i++) {
                if (iChars.indexOf(data.charAt(i)) != -1) {
                    alert ("Your string has special characters. \nThese are not allowed.");
                    return false;
                }
            }
            */

            // remove bad characters from the fileName to be displayed as thumbnail
            std::string badChar( "!@#$%^&()+=-[]';,{}~" );
            for( unsigned int c=0; c<fileName.length(); c++)
            {
                if( badChar.find_first_of( fileName[c] ) <= badChar.length() )
                    fileName[c] = '_';
            }
#endif
            text += fileName;
            text +=	"' width=100 height=100/> ";

            vector<std::string> files;
            SOLlistDirectoryFiles( "NaviLocal\\NaviTmpTexture\\", &files );
            vector<std::string>::iterator iter = files.begin();
            bool found = false;
            while( iter != files.end() )
            {
                if( (*iter) ==  fileName )
                {
                    found = true;
                    break;
                }
                iter++;
            }
            files.clear();
            if( !found )
            {
                image.load( texturePath, str);
                image.resize( 100, 100 );
                image.save( "NaviLocal\\NaviTmpTexture\\" + fileName );
            }
        }
    }
    text += "\"";
    mNavi->evaluateJS(text);
    //mNavi->evaluateJS("updateTexture()");
    //mNavi->evaluateJS("updTexture()");
    //mNavi->evaluateJS("document.getElementById('divTexture').innerHTML=textTabTextures;"





    // Update WWW/VLC/VNC panels
    mNavi->evaluateJS("resetWWWVLCVNCtabs()");
    TextureExtParamsMap* textureExtParamsMap = obj->getCurrentTextureExtParamsMap();
    if ((textureExtParamsMap != 0) && !textureExtParamsMap->empty())
    {
        String plugin = (*textureExtParamsMap)["plugin"];
        if (plugin == "www")
        {
            mNavi->evaluateJS("document.getElementById('MaterialWWWUrl').value = '" + (*textureExtParamsMap)["url"] + "'");
            mNavi->evaluateJS("document.getElementById('MaterialWWWWidth').value = '" + (*textureExtParamsMap)["width"] + "'");
            mNavi->evaluateJS("document.getElementById('MaterialWWWHeight').value = '" + (*textureExtParamsMap)["height"] + "'");
            mNavi->evaluateJS("document.getElementById('MaterialWWWFps').value = '" + (*textureExtParamsMap)["frames_per_second"] + "'");
        }
        else if (plugin == "vlc")
        {
            // std::string mrl = NaviUtilities::encodeURIComponent(StringHelpers::convertStringToWString((*textureExtParamsMap)["mrl"])); 
            std::string mrl = (*textureExtParamsMap)["mrl"];
            StringHelpers::replaceSubStr(mrl, "\\", "\\\\");
            mNavi->evaluateJS("$('MaterialVLCMrl').value = '" + mrl + "'");
            mNavi->evaluateJS("$('MaterialVLCWidth').value = '" + (*textureExtParamsMap)["width"] + "'");
            mNavi->evaluateJS("$('MaterialVLCHeight').value = '" + (*textureExtParamsMap)["height"] + "'");
            mNavi->evaluateJS("$('MaterialVLCFps').value = '" + (*textureExtParamsMap)["frames_per_second"] + "'");
            mNavi->evaluateJS("$('MaterialVLCParams').value = '" + (*textureExtParamsMap)["vlc_params"] + "'");
            mrl = (*textureExtParamsMap)["remote_mrl"];
            StringHelpers::replaceSubStr(mrl, "\\", "\\\\");
            mNavi->evaluateJS("$('MaterialVLCRemoteMrl').value = '" + mrl + "'");

            string sp3dStr = (*textureExtParamsMap)["sound_params"];
            if (sp3dStr.find("3d")==0)
            {
                mNavi->evaluateJS("$('MaterialVLCSP3d').checked = true");
                std::string spminStr = sp3dStr;
                size_t spacePos = spminStr.find_first_of(' ');
                spminStr.erase(0,spacePos+1);
                std::string spmaxStr=spminStr;
                spacePos = spminStr.find_first_of(' ');
                spminStr.erase(spacePos);
                spmaxStr.erase(0,spacePos+1);
                
                mNavi->evaluateJS("$('MaterialVLCSPMin').value = '" + spminStr + "'");
                mNavi->evaluateJS("$('MaterialVLCSPMax').value = '" + spmaxStr + "'");
            }
        }
        else if (plugin == "swf")
        {
            std::string url = NaviUtilities::encodeURIComponent(StringHelpers::convertStringToWString((*textureExtParamsMap)["url"]));
            mNavi->evaluateJS("$('MaterialSWFUrl').value = '" + url + "'");
            mNavi->evaluateJS("$('MaterialSWFWidth').value = '" + (*textureExtParamsMap)["width"] + "'");
            mNavi->evaluateJS("$('MaterialSWFHeight').value = '" + (*textureExtParamsMap)["height"] + "'");
            mNavi->evaluateJS("$('MaterialSWFFps').value = '" + (*textureExtParamsMap)["frames_per_second"] + "'");
            //navi->evaluateJS("$('MaterialSWFParams').value = '" + (*textureExtParamsMap)["swf_params"] + "'");
        }
       else if (plugin == "vnc")
        {
            String address = (*textureExtParamsMap)["address"];
            String password = (*textureExtParamsMap)["password"];
            std::string host, port;
            CommonTools::StringHelpers::getURLHostPort(address, host, port);
            String::size_type p = password.find_first_of(":");
            if (p != String::npos)
                password = password.substr(p + 1);
            mNavi->evaluateJS("$('MaterialVNCHost').value = '" + address + "'");
            mNavi->evaluateJS("$('MaterialVNCPort').value = '" + port + "'");
            mNavi->evaluateJS("$('MaterialVNCPwd').value = '" + password + "'");
        }
    }

    // Go back to the main directory
    _chdir(modeler->mExecPath.c_str());
}

//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerAddNewDeformation(Object3D::Command pCommand)
{
    Object3D* obj = mNavigator->getModeler()->getSelected();
    size_t nbCommands = mDeformButton.size();

    // create new name :
    // ...

    // create new window :
    // ...

    // get the button label
    std::string deformName;
    switch( pCommand)
    {
    case Object3D::TRANSLATE: deformName = "TRANSLATE"; break;
    case Object3D::ROTATE: deformName = "ROTATE"; break;
    case Object3D::SCALE: deformName = "SCLAE"; break;
    case Object3D::TAPERX:
    case Object3D::TAPERY: deformName = "TAPER"; break;
    case Object3D::TOP_SHEARX:
    case Object3D::TOP_SHEARY: deformName = "TOP_SHEAR"; break;
    case Object3D::TWIST_BEGIN:
    case Object3D::TWIST_END: deformName = "TWIST"; break;
    case Object3D::PATH_CUT_BEGIN:
    case Object3D::PATH_CUT_END: deformName = "PATH_CUT"; break;
    case Object3D::DIMPLE_BEGIN:
    case Object3D::DIMPLE_END: deformName = "DIMPLE"; break;
    case Object3D::HOLE_SIZEX:
    case Object3D::HOLE_SIZEY: 
    case Object3D::HOLLOW_SHAPE: deformName = "HOLE"; break;
    case Object3D::SKEW:
    case Object3D::REVOLUTION:
    case Object3D::RADIUS_DELTA: deformName = "SKEW"; break;
    default: deformName = "ERROR"; break;
    }

    // update property of this window :
    // ...

    // update size :
    mDeformButton.push_back( deformName );
}

/*
//-------------------------------------------------------------------------------------
void GUI_ModelerProperties::modelerProperties(Navi* caller, const Awesomium::JSArguments& args)
{
std::string slider;
slider = naviData["slider"].str();
}
*/

//-------------------------------------------------------------------------------------
