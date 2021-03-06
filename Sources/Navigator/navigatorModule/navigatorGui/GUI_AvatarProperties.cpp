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

#include "GUI_AvatarProperties.h"
#include "GUI_MessageBox.h"
#include "GUI_Avatar.h"

#include <CTStringHelpers.h>
#include <CTSystem.h>
#include <Navi.h>
#include <AvatarEditor.h>
#include <CharacterManager.h>
#include <Character.h>
#include <CharacterInstance.h>

using namespace Solipsis;
using namespace CommonTools;

GUI_AvatarProperties * GUI_AvatarProperties::stGUI_AvatarProperties = NULL;

//-------------------------------------------------------------------------------------
GUI_AvatarProperties::GUI_AvatarProperties() : GUI_Panel("uiavatarprop"),
mLockAmbientDiffuse(false)
{
    mNavigator = Navigator::getSingletonPtr();
	m_curState = NSNotCreated;
}

GUI_AvatarProperties::~GUI_AvatarProperties()
{
	stGUI_AvatarProperties = NULL;
}

//-------------------------------------------------------------------------------------
bool GUI_AvatarProperties::createAndShowPanel()
{
    if (!stGUI_AvatarProperties)
    {
      stGUI_AvatarProperties =  new GUI_AvatarProperties();
    }
    return stGUI_AvatarProperties->show();
}

//-------------------------------------------------------------------------------------
/*static*/ void GUI_AvatarProperties::unload()
{
    if (!stGUI_AvatarProperties)
    {
        return;
    } 

     stGUI_AvatarProperties->destroy();
    return;
}  

//-------------------------------------------------------------------------------------
/*static*/ void GUI_AvatarProperties::hidePanel()
{
    if (!stGUI_AvatarProperties)
    {
        return;
    } 

    stGUI_AvatarProperties->hide();
    return;
}  

//-------------------------------------------------------------------------------------
/*static*/ bool GUI_AvatarProperties::isPanelVisible()
{
    if (!stGUI_AvatarProperties)
    {
        return false;
    } 

    return stGUI_AvatarProperties->isVisible();
}  

//-------------------------------------------------------------------------------------
bool GUI_AvatarProperties::show()
{
    if (m_curState == NSNotCreated)
    {
        // Create Navi UI modeler
        createNavi(NaviPosition(TopRight), 512, 512);
        mNavi->bind("pageLoaded", NaviDelegate(this, &GUI_AvatarProperties::avatarPropPageLoaded));
        mNavi->loadFile("uiavatarprop.html");
        mNavi->setMovable(true);
        mNavi->hide();
        mNavi->setMask("uiavatarprop.png");//Eliminate the black shadow at the margin of the menu
        mNavi->setOpacity(0.75f);

        // page loaded
        mNavi->bind("pageClosed", NaviDelegate(this, &GUI_AvatarProperties::avatarPropPageClosed));

        // detect a changement on the properties tabber
        mNavi->bind("ClickOnTabber", NaviDelegate(this, &GUI_AvatarProperties::avatarTabberChange));

        // animation
        mNavi->bind("AnimPlayPause", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAnimPlayPause));
        mNavi->bind("AnimStop", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAnimStop));
        mNavi->bind("AnimNext", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAnimNext));
        mNavi->bind("AnimPrev", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAnimPrev));
        // properties
        mNavi->bind("AvatarHeight", NaviDelegate(this, &GUI_AvatarProperties::avatarPropHeight));
        mNavi->bind("BonePrev", NaviDelegate(this, &GUI_AvatarProperties::avatarPropBonePrev));
        mNavi->bind("BoneNext", NaviDelegate(this, &GUI_AvatarProperties::avatarPropBoneNext));
        mNavi->bind("BodyPartPrev", NaviDelegate(this, &GUI_AvatarProperties::avatarPropBPPrev));
        mNavi->bind("BodyPartNext", NaviDelegate(this, &GUI_AvatarProperties::avatarPropBPNext));
        mNavi->bind("BodyPartModelPrev", NaviDelegate(this, &GUI_AvatarProperties::avatarPropBPMPrev));
        mNavi->bind("BodyPartModelNext", NaviDelegate(this, &GUI_AvatarProperties::avatarPropBPMNext));
        mNavi->bind("BodyPartModelEdit", NaviDelegate(this, &GUI_AvatarProperties::avatarPropBPMEdit));
        mNavi->bind("BodyPartModelRemove", NaviDelegate(this, &GUI_AvatarProperties::avatarPropBPMRemove));
        mNavi->bind("AttachPrev", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAttPrev));
        mNavi->bind("AttachNext", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAttNext));
        mNavi->bind("AttachModelPrev", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAttMPrev));
        mNavi->bind("AttachModelNext", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAttMNext));
        mNavi->bind("AttachModelEdit", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAttMEdit));
        mNavi->bind("AttachModelRemove", NaviDelegate(this, &GUI_AvatarProperties::avatarPropAttMRemove));
        mNavi->bind("AvatarPropSliders", NaviDelegate(this, &GUI_AvatarProperties::avatarPropSliders));
        mNavi->bind("SelectionReset", NaviDelegate(this, &GUI_AvatarProperties::avatarPropReset));
        // material
        mNavi->bind("Ambient", NaviDelegate(this, &GUI_AvatarProperties::avatarColorAmbient));
        mNavi->bind("Diffuse", NaviDelegate(this, &GUI_AvatarProperties::avatarColorDiffuse));
        mNavi->bind("Specular", NaviDelegate(this, &GUI_AvatarProperties::avatarColorSpecular));
        mNavi->bind("AvatarLockAmbientDiffuse", NaviDelegate(this, &GUI_AvatarProperties::avatarColorLockAmbientDiffuse));
        mNavi->bind("AvatarDoubleSide", NaviDelegate(this, &GUI_AvatarProperties::avatarDoubleSide));
        mNavi->bind("AvatarShininess", NaviDelegate(this, &GUI_AvatarProperties::avatarPropShininess));
        mNavi->bind("AvatarTransparency", NaviDelegate(this, &GUI_AvatarProperties::avatarPropTransparency));
        mNavi->bind("AvatarScrollU", NaviDelegate(this, &GUI_AvatarProperties::avatarPropScrollU));
        mNavi->bind("AvatarScrollV", NaviDelegate(this, &GUI_AvatarProperties::avatarPropScrollV));
        mNavi->bind("AvatarScaleU", NaviDelegate(this, &GUI_AvatarProperties::avatarPropScaleU));
        mNavi->bind("AvatarScaleV", NaviDelegate(this, &GUI_AvatarProperties::avatarPropScaleV));
        mNavi->bind("AvatarRotateU", NaviDelegate(this, &GUI_AvatarProperties::avatarPropRotateU));
        mNavi->bind("AvatarAddTexture", NaviDelegate(this, &GUI_AvatarProperties::avatarPropTextureAdd));
        mNavi->bind("AvatarRemoveTexture", NaviDelegate(this, &GUI_AvatarProperties::avatarPropTextureRemove));
        mNavi->bind("AvatarPrevTexture", NaviDelegate(this, &GUI_AvatarProperties::avatarPropTexturePrev));
        mNavi->bind("AvatarNextTexture", NaviDelegate(this, &GUI_AvatarProperties::avatarPropTextureNext));
        mNavi->bind("AvatarResetColour", NaviDelegate(this, &GUI_AvatarProperties::avatarPropResetColour));
        mNavi->bind("AvatarPropSound", NaviDelegate(this, &GUI_AvatarProperties::avatarPropSound));

        m_curState = NSCreated;
    }
    else //if(!isAvatarMainVisible())
    {
        // Update the properties panel from the selected object datas
        avatarTabberLoad(1);
        mNavi->evaluateJS("$('avatarTabbers').tabber.tabShow(1)");
        GUI_Panel::show();
    }

    return true;
}

//------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarTabberChange(Navi* caller, const Awesomium::JSArguments& args)
{
    unsigned tab;
    tab = args.at(0).toInteger(); 
    avatarTabberLoad (tab);
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarTabberLoad(unsigned pTab)
{
    // get the current avatar
    CharacterInstance* avatar = mNavigator->getAvatarEditor()->getManager()->getCurrentInstance();
    if( avatar != 0 )
    {
        ColourValue col;
        Ogre::Vector2 UV;
        std::string text;

        switch( pTab )
        {
        case 0:	// animations tab
            {
                mNavi->evaluateJS("$('avatarTabbers').tabber.tabShow(0)");

                // Setup the animation list
                vector<std::string> list;
                for(int i=0; i<avatar->getCharacter()->getNumAnimations(); i++)
                    list.push_back( avatar->getEntity()->getSkeleton()->getAnimation( i )->getName() );
                std::string text = avatar->getEntity()->getSkeleton()->getAnimation( avatar->getCurrentAnimation() )->getName();
                mNavi->evaluateJS("$('animationSelectTitre').innerHTML = '" + text + "'");
                text = "";
                vector<std::string>::iterator iter = list.begin();
                int id = 0;
                while(iter!=list.end())
                {
                    text += "<div class='itemOut' onmouseout=this.className='itemOut' onmouseover=this.className='itemOver'><a href='#' class='lienMenu' onclick=select('";
                    text += (*iter).data();
                    text += "')>";
                    text += (*iter).data();
                    text += "</a></div>";
                    iter++;
                }
                mNavi->evaluateJS("$('animationSelectItem').innerHTML = \"" + text + "\"");

                size_t nbItem = list.size();
                if( nbItem < 7 )
                    mNavi->evaluateJS("$('animationSelectItem').style.height = '" + StringConverter::toString(nbItem*16) + "px'");
                list.clear();
            }
            break;
        case 1:	//  properties tab
            {
                mNavi->evaluateJS("$('avatarTabbers').tabber.tabShow(1)");

                // height
				mNavi->evaluateJS("height.onchange = function() {}");
#if 0 // GILLES
                Vector3 size = avatar->getEntity()->getBoundingBox().getSize();
				navi->evaluateJS("height.setValue(" + StringConverter::toString(int((size.y-0.5)*100)) + ")");
				navi->evaluateJS("height.onchange = function() {elementClicked('AvatarHeight')}");

				navi->evaluateJS("$('HeightValue').value=height.getValue()/100.+0.5+'m'");
#else
                float height = avatar->getEntity()->getBoundingBox().getSize().y * avatar->getSceneNode()->getScale().y - 0.5;
				mNavi->evaluateJS("height.setValue(" + StringConverter::toString(int(height*100.)) + ")");
				mNavi->evaluateJS("height.onchange = function() {elementClicked('AvatarHeight')}");
                char heightValue[8];
                sprintf( heightValue, "'%1.2fm'", height+.5 );
                mNavi->evaluateJS("$('HeightValue').value=" + std::string(heightValue));
#endif
                // bones
                Bone* bone = avatar->getCurrentBone();
                std::string name(bone->getName());
                std::string temp("");
                for(int i=0; i<(int)name.length(); i++)
                {
                    char c = name[i];
                    if (! ((c < 48 && c != 32) || c == 255 ||
                        c == 208 || c == 209 ||	c == 215 || c == 216 ||
                        (c < 192 && c > 122) || (c < 65 && c > 57)) ) 
                    {
                        temp += c;
                    }
                }
                mNavi->evaluateJS("$('BoneName').innerHTML = '" + temp + "'");

                // body parts
                name = "None";
                if(avatar->getCharacter()->getNumBodyParts() > 0)
                    name = avatar->getCurrentBodyPart()->getBodyPart()->getName();
                mNavi->evaluateJS("$('BodyPartName').innerHTML = '" + name + "'");

                name = "None";
                if(avatar->getCharacter()->getNumBodyParts() > 0)
                    name = avatar->getCurrentBodyPart()->getCurrentBodyPartModelName();
                mNavi->evaluateJS("$('BodyPartModelName').innerHTML = '" + name + "'");
                if( name == "None" )
                    mNavi->evaluateJS("$('BodyPartEdit').disabled = 1");

                // attachements / goodies
                name = "None";
                if(avatar->getCharacter()->getNumGoodies() > 0)
                    name = avatar->getCurrentGoody()->getGoody()->getName();
                mNavi->evaluateJS("$('AttachName').innerHTML = '" + name + "'");

                name = "None";
                if(avatar->getCharacter()->getNumGoodies() > 0)
                    name = avatar->getCurrentGoody()->getCurrentGoodyModelName();
                mNavi->evaluateJS("$('AttachModelName').innerHTML = '" + name + "'");
                if( name == "None" )
                    mNavi->evaluateJS("$('AttachEdit').disabled = 1");

                AvatarEditor::getSingletonPtr()->selectType = 0;
                Quaternion q = bone->getOrientation();
                Vector3 orientation(q.getYaw().valueRadians(), q.getPitch().valueRadians(), q.getRoll().valueRadians());
                avatarUpdateSliders( bone->getPosition()+0.5, orientation/Math::TWO_PI+0.5, bone->getScale()/2 );
            }
            break;
        case 2: // material
            {
                mNavi->evaluateJS("$('avatarTabbers').tabber.tabShow(2)");

                int type = AvatarEditor::getSingletonPtr()->selectType;
                ModifiableMaterialObject* object;

                if( type == 2 ) // Goody
                    object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
                else //if( type <= 1 ) // BodyPart
                    object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

                ModifiedMaterial* material = object->getModifiedMaterial();

                char color[32];

                sprintf(color, "%.2X%.2X%.2X", material->getAmbient().r * 255, material->getAmbient().g * 255, material->getAmbient().b * 255);
                
                mNavi->evaluateJS("$S('pAmbient').background='#" + String(color) + "'");

                sprintf(color, "%.2X%.2X%.2X", material->getDiffuse().r * 255, material->getDiffuse().g * 255, material->getDiffuse().b * 255);
                mNavi->evaluateJS("$S('pDiffuse').background='#" + String(color) + "'");
                sprintf(color, "%.2X%.2X%.2X", material->getSpecular().r * 255, material->getSpecular().g * 255, material->getSpecular().b * 255);
                mNavi->evaluateJS("$S('pSpecular').background='#" +  String(color) + "'");

                mNavi->evaluateJS("shininess.onchange = function() {}");
                mNavi->evaluateJS("transparency.onchange = function() {}");
                mNavi->evaluateJS("scrollU.onchange = function() {}");
                mNavi->evaluateJS("scrollV.onchange = function() {}");
                mNavi->evaluateJS("scaleU.onchange = function() {}");
                mNavi->evaluateJS("scaleV.onchange = function() {}");
                mNavi->evaluateJS("rotateU.onchange = function() {}");

                mNavi->evaluateJS("shininess.setValue(" + StringConverter::toString(Real(material->getShininess()/128.)*100) + ")");
                mNavi->evaluateJS("transparency.setValue("+ StringConverter::toString(material->getAlpha()*100) + ")");
                UV = material->getTextureScroll() ;
                mNavi->evaluateJS("scrollU.setValue(" + StringConverter::toString(UV.x*100+50) + ")");
                mNavi->evaluateJS("scrollV.setValue(" + StringConverter::toString(UV.y*100+50) + ")");
                UV = material->getTextureScale() ;
                mNavi->evaluateJS("scaleU.setValue(" + StringConverter::toString(UV.x*100-50) + ")");
                mNavi->evaluateJS("scaleU.setValue(" + StringConverter::toString(UV.y*100-50) + ")");
                mNavi->evaluateJS("rotateU.setValue(" + StringConverter::toString(material->getTextureRotate()/Math::TWO_PI*100) + ")");

                mNavi->evaluateJS("shininess.onchange = function() {elementClicked('AvatarShininess')}");
                mNavi->evaluateJS("transparency.onchange = function() {elementClicked('AvatarTransparency')}");
                mNavi->evaluateJS("scrollU.onchange = function() {elementClicked('AvatarScrollU')}");
                mNavi->evaluateJS("scrollV.onchange = function() {elementClicked('AvatarScrollV')}");
                mNavi->evaluateJS("scaleU.onchange = function() {elementClicked('AvatarScaleU')}");
                mNavi->evaluateJS("scaleV.onchange = function() {elementClicked('AvatarScaleV')}");
                mNavi->evaluateJS("rotateU.onchange = function() {elementClicked('AvatarRotateU')}");

                MaterialPtr mat = object->getModifiedMaterial()->getOwner();
                CullingMode mode = mat->getTechnique(0)->getPass(0)->getCullingMode();
                mNavi->evaluateJS("document.getElementById('doubleSide').checked = " + (mode == CULL_NONE)?"true":"false" );

                avatarUpdateTextures( object );
            }
            break;
        case 3:	//  sound tab
            {
                mNavi->evaluateJS("$('avatarTabbers').tabber.tabShow(3)");

                float minDist, maxDist;
                Avatar* user = mNavigator->getUserAvatar();
                user->getVoiceDistances(minDist, maxDist);
                // sound3DMinDist
                mNavi->evaluateJS("sound3DMinDist.onchange = function() {}");
                mNavi->evaluateJS("sound3DMinDist.setValue(" + StringConverter::toString(minDist) + ")");
                mNavi->evaluateJS("sound3DMinDist.onchange = function() {soundDistances()}");
                mNavi->evaluateJS("$('Sound3DMinDistValue').value=sound3DMinDist.getValue()");
                // sound3DMaxDist
                mNavi->evaluateJS("sound3DMaxDist.onchange = function() {}");
                mNavi->evaluateJS("sound3DMaxDist.setValue(" + StringConverter::toString(maxDist) + ")");
                mNavi->evaluateJS("sound3DMaxDist.onchange = function() {soundDistances()}");
                mNavi->evaluateJS("$('Sound3DMaxDistValue').value=sound3DMaxDist.getValue()");
            }
            break;
        }
        //mNaviMgr->Update();
    }
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarTabberSave()
{}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropPageLoaded(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropPageLoaded()");

    // Update the properties panel from the selected object datas
    avatarTabberLoad( 1 );

    // Show Navi UI
    if (m_curState == NSCreated)
        mNavi->show(true);
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropPageClosed(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropPageClosed()");

    if (!GUI_Avatar::isPanelVisible())
    {
        hide();
        GUI_Avatar::createAndShowPanel();
    }
    else 
        GUI_Avatar::unload();
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAnimPlayPause(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAnimPlayPause()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    Avatar* user = mNavigator->getUserAvatar();
    

    user->stopAnimation();

    std::string state(mNavi->evaluateJSWithResult("$('AnimPlayPause').value").get().toString());
    if(state == "Pause")
    {
        mNavi->evaluateJS(std::string("$('AnimPlayPause').value = 'Play'"));
        mNavi->evaluateJS(std::string("$('AnimTime').style = 'display: none'"));
        user->stopAnimation();
    }
    else //Play
    {
        mNavi->evaluateJS(std::string("$('AnimPlayPause').value = 'Pause'"));
        mNavi->evaluateJS(std::string("$('AnimTime').style = 'display: block'"));
        user->setState(AnimationState(avatar->getCurrentAnimation()));
        user->startAnimation(user->getEntity()->getSkeleton()->getAnimation(avatar->getCurrentAnimation())->getName());
    }

    // ...
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAnimStop(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAnimStop()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    Avatar* user = mNavigator->getUserAvatar();
    

    mNavi->evaluateJS(std::string("$('AnimPlayPause').value = 'Play'"));
    mNavi->evaluateJS(std::string("$('AnimTime').style = 'display: none'"));
    user->setState(ASAvatarNone);
    user->stopAnimation();
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAnimNext(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAnimNext()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    Avatar* user = mNavigator->getUserAvatar();
    

    unsigned int numAnim = avatar->getCharacter()->getNumAnimations();
    unsigned int current = avatar->getCurrentAnimation();
    if(++current >= numAnim) current = 0;
    avatar->setCurrentAnimation(current);

    std::string text( user->getEntity()->getSkeleton()->getAnimation(current)->getName() );
    user->stopAnimation();
    user->setState(AnimationState(current+1));
    user->startAnimation( text );

    mNavi->evaluateJS("$('animationSelectTitre').innerHTML = '" + text + "'");
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAnimPrev(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAnimPrev()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    Avatar* user = mNavigator->getUserAvatar();
    

    unsigned int numAnim = avatar->getCharacter()->getNumAnimations();
    int current = avatar->getCurrentAnimation();
    if(--current < 0) current = numAnim-1;
    avatar->setCurrentAnimation(current);

    std::string text( user->getEntity()->getSkeleton()->getAnimation(current)->getName() );
    user->stopAnimation();
    user->setState(AnimationState(current+1));
    user->startAnimation( text );

    mNavi->evaluateJS("$('animationSelectTitre').innerHTML = '" + text + "'");
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropHeight(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropHeight()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    float height = mNavi->evaluateJSWithResult("height.getValue()").get().toInteger() / 100. + 0.5;
    float scale = height / avatar->getEntity()->getBoundingBox().getSize().y;
#if 1 // GILLES
	SceneNode* node = mNavigator->getUserAvatar()->getSceneNode();
	node->setScale( scale, scale, scale );
    //mNavigator->getUserAvatar()->onSceneNodeChanged();

    char heightValue[8];
    sprintf( heightValue, "'%1.2fm'", height );
    mNavi->evaluateJS("$('HeightValue').value=" + std::string(heightValue));
#else
    SceneNode* node = avatar->getSceneNode();

    //	static Node* child = node->removeChild( (unsigned short) 2 );
    node->setScale( scale, scale, scale );
    //node->addChild( child );

	navi->evaluateJS("$('HeightValue').value=height.getValue()/100.+0.5+'m'");
#endif
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropBonePrev(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropBonePrev()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    Bone* bone = avatar->setPreviousBoneAsCurrent();
    std::string str = bone->getName();
    std::string temp;
    char c;
    for(int i=0; i<(int)str.length(); i++)
    {
        c = str[i];
        if (! ((c < 48 && c != 32) || c == 255 ||
            c == 208 || c == 209 ||
            c == 215 || c == 216 ||
            (c < 192 && c > 122) ||
            (c < 65 && c > 57))) 
        {
            temp += c;
        }
    }
    str = "$('BoneName').innerHTML = '" + temp;
    mNavi->evaluateJS(str + "'");

    AvatarEditor::getSingletonPtr()->selectType = 0;
    Quaternion q = bone->getOrientation();
    Vector3 orientation(q.getYaw().valueRadians(), q.getPitch().valueRadians(), q.getRoll().valueRadians());
    avatarUpdateSliders( bone->getPosition()+0.5, orientation/Math::TWO_PI+0.5, bone->getScale()/2 );

    //mNavi->evaluateJS("$('oriX').style.display='none'");
    //mNavi->evaluateJS("$('oriY').style.display='none'");
    //mNavi->evaluateJS("$('oriZ').style.display='none'");
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropBoneNext(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropBoneNext()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    Bone* bone = avatar->setNextBoneAsCurrent();
    std::string str = bone->getName();
    std::string temp;
    char c;
    for(int i=0; i<(int)str.length(); i++)
    {
        c = str[i];
        if (! ((c < 48 && c != 32) || c == 255 ||
            c == 208 || c == 209 ||
            c == 215 || c == 216 ||
            (c < 192 && c > 122) ||
            (c < 65 && c > 57)	)) 
        {
            temp += c;
        }
    }
    str = "$('BoneName').innerHTML = '" + temp;
    mNavi->evaluateJS(str + "'");

    AvatarEditor::getSingletonPtr()->selectType = 0;
    Quaternion q = bone->getOrientation();
    Vector3 orientation(q.getYaw().valueRadians(), q.getPitch().valueRadians(), q.getRoll().valueRadians());
    avatarUpdateSliders( bone->getPosition()+0.5, orientation/Math::TWO_PI+0.5, bone->getScale()/2 );

    //mNavi->evaluateJS("$('oriX').style.display='none'");
    //mNavi->evaluateJS("$('oriY').style.display='none'");
    //mNavi->evaluateJS("$('oriZ').style.display='none'");
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropBPPrev(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropBPPrev()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    BodyPartInstance* bp = avatar->setPreviousBodyPartAsCurrent();
    BodyPartModel* bpm = bp->getCurrentBodyPartModel();
    std::string bpName = bp->getBodyPart()->getName();

    std::string str = "$('BodyPartName').innerHTML = '";
    str += bpName;
    mNavi->evaluateJS(str + "'");

    str = "$('BodyPartModelName').innerHTML = '";
    if (bpm != NULL)
    {
        str += bpm->getName();
        mNavi->evaluateJS(str + "'");
        mNavi->evaluateJS("$('BodyPartEdit').disabled = 0");

        // Update sliders ...
        AvatarEditor::getSingletonPtr()->selectType = 1;
        //Vector3 pos( bpm->getPosition() );
        //Vector3 ori( bpm->getOrientatio() );
        //Vector3 scale( bpm->getScale() );
        avatarUpdateSliders( Vector3::ZERO, Vector3::ZERO, Vector3::ZERO );
    }
    else 
    {
        str += "None";
        mNavi->evaluateJS(str + "'");
        mNavi->evaluateJS("$('BodyPartEdit').disabled = 1");
    }
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropBPNext(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropBPNext()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    BodyPartInstance* bp = avatar->setNextBodyPartAsCurrent();
    BodyPartModel* bpm = bp->getCurrentBodyPartModel();
    std::string bpName = bp->getBodyPart()->getName();

    std::string str = "$('BodyPartName').innerHTML = '";
    str += bpName;
    mNavi->evaluateJS(str + "'");

    str = "$('BodyPartModelName').innerHTML = '";
    if (bpm != NULL)
    {
        str += bpm->getName();
        mNavi->evaluateJS(str + "'");
        mNavi->evaluateJS("$('BodyPartEdit').disabled = 0");

        // Update sliders ...
        AvatarEditor::getSingletonPtr()->selectType = 1;
        //Vector3 pos( bpm->getPosition() );
        //Vector3 ori( bpm->getOrientatio() );
        //Vector3 scale( bpm->getScale() );
        avatarUpdateSliders( Vector3::ZERO, Vector3::ZERO, Vector3::ZERO );
    }
    else 
    {
        str += "None";
        mNavi->evaluateJS(str + "'");
        mNavi->evaluateJS("$('BodyPartEdit').disabled = 1");
    }
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropBPMPrev(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropBPMPrev()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    BodyPartInstance* bp = avatar->getCurrentBodyPart();
    bp->setPreviousBodyPartModelAsCurrent();
    BodyPartModel* bpm = bp->getCurrentBodyPartModel();

    std::string str = "$('BodyPartModelName').innerHTML = '";
    if (bpm != NULL)
    {
        str += bpm->getName();
        mNavi->evaluateJS(str + "'");
        mNavi->evaluateJS("$('BodyPartEdit').disabled = 0");

        // Update sliders ...
        AvatarEditor::getSingletonPtr()->selectType = 1;
        //Vector3 pos( bpm->getPosition() );
        //Vector3 ori( bpm->getOrientatio() );
        //Vector3 scale( bpm->getScale() );
        //avatarUpdateSliders( Vector3::ZERO, Vector3::ZERO, Vector3::ZERO );
        mNavi->evaluateJS("$('sliders').style = 'display:none'");
    }
    else 
    {
        str += "None";
        mNavi->evaluateJS(str + "'");
        mNavi->evaluateJS("$('BodyPartEdit').disabled = 1");
    }
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropBPMNext(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropBPMNext()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    BodyPartInstance* bp = avatar->getCurrentBodyPart();
    bp->setNextBodyPartModelAsCurrent();
    BodyPartModel* bpm = bp->getCurrentBodyPartModel();

    std::string str = "$('BodyPartModelName').innerHTML = '";
    if (bpm != NULL)
    {
        str += bpm->getName();
        mNavi->evaluateJS(str + "'");
        mNavi->evaluateJS("$('BodyPartEdit').disabled = 0");

        // Update sliders ...
        AvatarEditor::getSingletonPtr()->selectType = 1;
        //Vector3 pos( bpm->getPosition() );
        //Vector3 ori( bpm->getOrientatio() );
        //Vector3 scale( bpm->getScale() );
        //avatarUpdateSliders( Vector3::ZERO, Vector3::ZERO, Vector3::ZERO );
        mNavi->evaluateJS("$('sliders').style = 'display:none'");
    }
    else 
    {
        str += "None";
        mNavi->evaluateJS(str + "'");
        mNavi->evaluateJS("$('BodyPartEdit').disabled = 1");
    }
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropBPMEdit(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropBPMEdit()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    AvatarEditor::getSingletonPtr()->selectType = 1;
    avatarTabberLoad(2);
    mNavi->evaluateJS("$('avatarTabbers').tabber.tabShow(2)");
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropBPMRemove(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropBPMRemove()");
    //CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    //
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAttPrev(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAttPrev()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    if(avatar->getCharacter()->getNumGoodies() > 0)
    {
        GoodyInstance* g = avatar->setPreviousGoodyAsCurrent();
        GoodyModel* gm = g->getCurrentGoodyModel();
        std::string gName = g->getGoody()->getName();

        std::string str = "$('AttachName').innerHTML = '";
        str += gName;
        mNavi->evaluateJS(str + "'");

        str = "$('AttachModelName').innerHTML = '";
        if (gm != NULL)
        {
            str += gm->getName();
            mNavi->evaluateJS(str + "'");
            mNavi->evaluateJS("$('AttachEdit').disabled = 0");

            // Update sliders ...
            AvatarEditor::getSingletonPtr()->selectType = 2;
            avatarUpdateSliders( g->getCurrentPosition()/2+0.5, g->getCurrentRotationsAngles()/180 + 0.5, Vector3::ZERO );
            mNavi->evaluateJS("$('sliders').style = 'display:block'");
        }
        else 
        {
            str += "None";
            mNavi->evaluateJS(str + "'");
            mNavi->evaluateJS("$('AttachEdit').disabled = 1");
        }
    }
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAttNext(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAttNext()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    if(avatar->getCharacter()->getNumGoodies() > 0)
    {
        GoodyInstance* g = avatar->setNextGoodyAsCurrent();
        GoodyModel* gm = g->getCurrentGoodyModel();
        std::string gName = g->getGoody()->getName();

        std::string str = "$('AttachName').innerHTML = '";
        str += gName;
        mNavi->evaluateJS(str + "'");

        str = "$('AttachModelName').innerHTML = '";
        if (gm != NULL)
        {
            str += gm->getName();
            mNavi->evaluateJS(str + "'");
            mNavi->evaluateJS("$('AttachEdit').disabled = 0");

            // Update sliders ...
            AvatarEditor::getSingletonPtr()->selectType = 2;
            avatarUpdateSliders( g->getCurrentPosition()/2+0.5, g->getCurrentRotationsAngles()/180 + 0.5, Vector3::ZERO );
            mNavi->evaluateJS("$('sliders').style = 'display:block'");
        }
        else 
        {
            str += "None";
            mNavi->evaluateJS(str + "'");
            mNavi->evaluateJS("$('AttachEdit').disabled = 1");
        }
    }
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAttMPrev(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAttMPrev()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    if(avatar->getCharacter()->getNumGoodies() > 0)
    {
        GoodyInstance* g = avatar->getCurrentGoody();
        g->setPreviousGoodyModelAsCurrent();
        GoodyModel* gm = g->getCurrentGoodyModel();

        std::string str = "$('AttachModelName').innerHTML = '";
        if (gm != NULL)
        {
            str += gm->getName();
            mNavi->evaluateJS(str + "'");
            mNavi->evaluateJS("$('AttachEdit').disabled = 0");

            // Update sliders ...
            //AvatarEditor::getSingletonPtr()->selectType = 2;
            //avatarUpdateSliders( g->getCurrentPosition(), g->getCurrentRotationsAngles(), Vector3::ZERO );
        }
        else 
        {
            str += "None";
            mNavi->evaluateJS(str + "'");
            mNavi->evaluateJS("$('AttachEdit').disabled = 1");
        }
    }
}

//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAttMNext(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAttMNext()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    if(avatar->getCharacter()->getNumGoodies() > 0)
    {
        GoodyInstance* g = avatar->getCurrentGoody();
        g->setNextGoodyModelAsCurrent();
        GoodyModel* gm = g->getCurrentGoodyModel();

        std::string str = "$('AttachModelName').innerHTML = '";
        if (gm != NULL)
        {
            str += gm->getName();
            mNavi->evaluateJS(str + "'");
            mNavi->evaluateJS("$('AttachEdit').disabled = 0");

            // Update sliders ...
            //AvatarEditor::getSingletonPtr()->selectType = 2;
            //avatarUpdateSliders( g->getCurrentPosition(), g->getCurrentRotationsAngles(), Vector3::ZERO );
        }
        else 
        {
            str += "None";
            mNavi->evaluateJS(str + "'");
            mNavi->evaluateJS("$('AttachEdit').disabled = 1");
        }
    }
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAttMEdit(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAttMEdit()");
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    

    AvatarEditor::getSingletonPtr()->selectType = 2;
    avatarTabberLoad(2);
    mNavi->evaluateJS("$('avatarTabbers').tabber.tabShow(2)");
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropAttMRemove(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropAttMRemove()");
    //CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    //
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropSliders(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropSliders()");
    
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    Avatar* user = mNavigator->getUserAvatar();

    std::string slider = args.at(0).toString();
    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type < 0) return;

    // get the value from the slidebar position
    std::string temp = slider + ".getValue()";
    float value = mNavi->evaluateJSWithResult(temp).get().toDouble() / 100.;

    Bone* bone = avatar->getCurrentBone();
    BodyPartInstance* body = NULL;
    GoodyInstance* goody = NULL;

    // get the selected object ( BodyPart / Goody )
    if( avatar->getCharacter()->getNumBodyParts() > 0 )
        body = avatar->getCurrentBodyPart();
    if( avatar->getCharacter()->getNumGoodies() > 0 )
        goody = avatar->getCurrentGoody();

    // enable modification on the bone properties
    if( !bone->isManuallyControlled() ) 
    {
        bone->setManuallyControlled( true );
        for(int i=0; i<user->getEntity()->getSkeleton()->getNumAnimations(); i++ )
            user->getEntity()->getSkeleton()->getAnimation( i )->destroyNodeTrack( bone->getHandle() );
    }

    Vector3 vec;
    Quaternion q;
    Radian angle;

    // apply the modification on the selection ( bone / bodyPart / goody )
    if( slider == "posX")
    {
        switch( type )
        {
        case 0: // bone
            bone->translate( value - 0.5 - bone->getPosition().x, 0, 0 );
            break;
        case 1: // bodyPart
            break;
        case 2: // goody
            goody->setCurrentXScrollPosition( value );
            break;
        }
    }
    else if( slider == "posY")
    {
        switch( type )
        {
        case 0: // bone
            bone->translate( 0, value - 0.5 - bone->getPosition().y, 0 );
            break;
        case 1: // bodyPart
            break;
        case 2: // goody
            goody->setCurrentYScrollPosition( value );
            break;
        }
    }
    else if( slider == "posZ")
    {
        switch( type )
        {
        case 0: // bone
            bone->translate( 0, 0, value - 0.5 - bone->getPosition().z );
            break;
        case 1: // bodyPart
            break;
        case 2: // goody
            goody->setCurrentZScrollPosition( value );
            break;
        }
    }
    else if( slider == "oriX")
    {
        switch( type )
        {
        case 0: // bone
            q = bone->getOrientation();
            angle = Radian( (value - 0.5)*Math::TWO_PI );
            bone->yaw( angle - q.getYaw() );
            break;
        case 1: // bodyPart
            break;
        case 2: // goody
            goody->setCurrentYawAngleScrollPosition( value );
            break;
        }
    }
    else if( slider == "oriY")
    {
        switch( type )
        {
        case 0: // bone
            q = bone->getOrientation();
            angle = Radian( (value - 0.5)*Math::TWO_PI );
            bone->pitch( angle - q.getPitch() );
            break;
        case 1: // bodyPart
            break;
        case 2: // goody
            goody->setCurrentPitchAngleScrollPosition( value );
            break;
        }
    }
    else if( slider == "oriZ")
    {
        switch( type )
        {
        case 0: // bone
            q = bone->getOrientation();
            angle = Radian( (value - 0.5)*Math::TWO_PI );
            bone->roll( angle - q.getRoll() );
            break;
        case 1: // bodyPart
            break;
        case 2: // goody
            goody->setCurrentRollAngleScrollPosition( value );
            break;
        }
    }
    else if( slider == "scaleX")
    {
        switch( type )
        {
        case 0: // bone
            vec = bone->getScale();
            vec.x = value * 2;
            bone->setScale( vec );
            break;
        case 1: // bodyPart
            break;
        case 2: // goody
            break;
        }
    }
    else if( slider == "scaleY")
    {
        switch( type )
        {
        case 0: // bone
            vec = bone->getScale();
            vec.y = value * 2;
            bone->setScale( vec );
            break;
        case 1: // bodyPart
            break;
        case 2: // goody
            break;
        }
    }
    else if( slider == "scaleZ")
    {
        switch( type )
        {
        case 0: // bone
            vec = bone->getScale();
            vec.z = value * 2;
            bone->setScale( vec );
            break;
        case 1: // bodyPart
            break;
        case 2: // goody
            break;
        }
    }
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropReset(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropSliders()");
    
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();

    std::string slider( args.at(0).toString() );
    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type < 0) return;
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarColorAmbient(Navi* caller, const Awesomium::JSArguments& args)
{
    unsigned idRGB = 0;
    std::string str, color;
    int rgb[3]; 
    str = args.at(0).toString();

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

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    ModifiedMaterial* material = object->getModifiedMaterial();
    if( material != 0 )
    {
        material->useAddedColour(true);
        material->setAmbient( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
        //material->setAddedColour( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );

        if(mLockAmbientDiffuse)
            material->setDiffuse( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
    }
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarColorDiffuse(Navi* caller, const Awesomium::JSArguments& args)
{
    unsigned idRGB = 0;
    std::string str, color;
    int rgb[3]; 
    str = args.at(0).toString();

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

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

#if 0 // GILLES
    ModifiedMaterial* material = object->getModifiedMaterial();
    if( material != 0 )
    {
        material->setDiffuse( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
        if(mLockAmbientDiffuse)
        {
            material->useAddedColour(true);
            material->setAmbient( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
            //material->setAddedColour( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
        }
    }
#else
    if (object != 0) 
    {
        ModifiedMaterial* material = object->getModifiedMaterial();
        if (material != 0) 
        {
            // apply the nex color
            material->useAddedColour(true);

            ColourValue addedColor( rgb[0]/255., rgb[1]/255., rgb[2]/255., 1. );
            material->setAddedColour( addedColor );
            /*
            if(mLockAmbientDiffuse) 
            {
                material->setAmbient( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
                material->setDiffuse( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
            }
            */
        }
    }
#endif
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarColorSpecular(Navi* caller, const Awesomium::JSArguments& args)
{
    unsigned idRGB = 0;
    std::string str, color;
    int rgb[3]; 
    str = args.at(0).toString();

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

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    ModifiedMaterial* material = object->getModifiedMaterial();
    if( material != 0 )
        material->setSpecular( ColourValue(rgb[0]/255., rgb[1]/255., rgb[2]/255.) );
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarColorLockAmbientDiffuse(Navi* caller, const Awesomium::JSArguments& args)
{
    std::string value = mNavi->evaluateJSWithResult("$('lockAmbientdiffuse').checked").get().toString();
    mLockAmbientDiffuse = (value == "true")?true:false;
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarDoubleSide(Navi* caller, const Awesomium::JSArguments& args)
{
    
    std::string value = mNavi->evaluateJSWithResult("$('doubleSide').checked").get().toString();

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    MaterialPtr mat = object->getModifiedMaterial()->getOwner();
    mat->getTechnique(0)->getPass(0)->setCullingMode( (value == "true")?CULL_NONE:CULL_CLOCKWISE );
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropShininess(Navi* caller, const Awesomium::JSArguments& args)
{
    
    std::string value = mNavi->evaluateJSWithResult("shininess.getValue()").get().toString();

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    ModifiedMaterial* material = object->getModifiedMaterial();
    if( material != 0 )
        material->setShininess( atoi(value.c_str())*1.28 );
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropTransparency(Navi* caller, const Awesomium::JSArguments& args)
{
    
    std::string value = mNavi->evaluateJSWithResult("transparency.getValue()").get().toString();

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    ModifiedMaterial* material = object->getModifiedMaterial();
    if( material != 0 )
    {
        //        material->setSceneBlendType(SBT_TRANSPARENT_ALPHA);
        material->setAlpha( atoi(value.c_str())/100. );
        //		material->getOwner()->getTechnique(0)->getPass(0)->setDepthWriteEnabled( false );
    }
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropResetColour(Navi* caller, const Awesomium::JSArguments& args)
{
    
    std::string value = mNavi->evaluateJSWithResult("transparency.getValue()").get().toString();

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    object->resetColour();

    // update colour sliders
    avatarTabberLoad( 2 );
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropScrollU(Navi* caller, const Awesomium::JSArguments& args)
{
    
    std::string value = mNavi->evaluateJSWithResult("scrollU.getValue()").get().toString();

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    ModifiedMaterial* material = object->getModifiedMaterial();
    if( material != 0 )
    {
        Ogre::Vector2 UV = material->getTextureScroll();
        material->setTextureScroll(atoi(value.c_str())/100. - .5, UV.y);
    }
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropScrollV(Navi* caller, const Awesomium::JSArguments& args)
{
    
    std::string value = mNavi->evaluateJSWithResult("scrollV.getValue()").get().toString();

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    ModifiedMaterial* material = object->getModifiedMaterial();
    if( material != 0 )
    {
        Ogre::Vector2 UV = material->getTextureScroll();
        material->setTextureScroll(UV.x, atoi(value.c_str())/100. - .5);
    }
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropScaleU(Navi* caller, const Awesomium::JSArguments& args)
{
    
    std::string value = mNavi->evaluateJSWithResult("scaleU.getValue()").get().toString();

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    ModifiedMaterial* material = object->getModifiedMaterial();
    if( material != 0 )
    {
        Ogre::Vector2 UV = material->getTextureScale();
        material->setTextureScale( atoi(value.c_str())/100. + .5, UV.y );
    }
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropScaleV(Navi* caller, const Awesomium::JSArguments& args)
{
    
    std::string value = mNavi->evaluateJSWithResult("scaleV.getValue()").get().toString();

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    ModifiedMaterial* material = object->getModifiedMaterial();
    if( material != 0 )
    {
        Ogre::Vector2 UV = material->getTextureScale();
        material->setTextureScale( UV.x, atoi(value.c_str())/100. + .5 );
    }
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropRotateU(Navi* caller, const Awesomium::JSArguments& args)
{
    
    std::string value = mNavi->evaluateJSWithResult("rotateU.getValue()").get().toString();

    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    ModifiedMaterial* material = object->getModifiedMaterial();

    if( material != 0 )
        material->setTextureRotate( Ogre::Radian(atoi(value.c_str())/100.*Math::TWO_PI) );
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropTextureAdd(Navi* caller, const Awesomium::JSArguments& args)
{
    char * PathTexture = FileBrowser::displayWindowForLoading( 
        "Image Files (*.png;*.bmp;*.jpg)\0*.png;*.bmp;*.jpg\0", string("") ); 

    if (PathTexture != NULL )
    {
        CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
        ModifiableMaterialObject* object;

        int type = AvatarEditor::getSingletonPtr()->selectType;
        if( type == 2 ) // Goody
            object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
        else //if( type <= 1 ) // BodyPart
            object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

        String TextureFilePath (PathTexture);

        //Create the new OGRE texture with the file selected :
        TexturePtr PtrTexture = TextureManager::getSingleton().load( TextureFilePath, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

        //Add texture for the object (with obj->mModifiedMaterialManager)
        object->addTexture(PtrTexture);

        avatarUpdateTextures( object );
    }
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropTextureRemove(Navi* caller, const Awesomium::JSArguments& args)
{
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    //get selected texture :
    if( object->getModifiableMaterialObjectBase()->isTextureModifiable() )
    {
        // remove the old texture and set the previous texture as current
        object->removeTexture( object->getCurrentTexture() );
    }

    avatarUpdateTextures( object );
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropTexturePrev(Navi* caller, const Awesomium::JSArguments& args)
{
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    object->setNextTextureAsCurrent();
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropTextureNext(Navi* caller, const Awesomium::JSArguments& args)
{
    CharacterInstance* avatar = AvatarEditor::getSingletonPtr()->getManager()->getCurrentInstance();
    ModifiableMaterialObject* object;

    int type = AvatarEditor::getSingletonPtr()->selectType;
    if( type == 2 ) // Goody
        object = (ModifiableMaterialObject*)avatar->getCurrentGoody()->getCurrentGoodyModelInstance();
    else //if( type <= 1 ) // BodyPart
        object = (ModifiableMaterialObject*)avatar->getCurrentBodyPart()->getCurrentBodyPartModelInstance();

    object->setPreviousTextureAsCurrent();
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarUpdateTextures(ModifiableMaterialObject* pObject)
{
    

    std::string texturePath, text;
    String str;
    TexturePtr texture; 
    Ogre::Image image;

    // Go back to the main directory
    _chdir(mNavigator->getAvatarEditor()->mExecPath.c_str());

    // create a temporary forlder for the thumbnail textures
#ifdef WIN32
    CreateDirectory( "NaviLocal\\NaviTmpTexture", NULL );
#else
    system( "md NaviLocal\\NaviTmpTexture" );
#endif

    // update Navi interface
    text = "textTabTextures = \"";
    //if( pObject->isTextureModifiable() )
    {
        TextureVectorIterator tvIter = pObject->getModifiableMaterialObjectBase()->getTextureIterator();
        while( tvIter.hasMoreElements() )
        {
            texture = tvIter.getNext();
            texturePath = texture->getName();
            Path path(texturePath);
            size_t begin = path.getFormatedPath().find_last_of( '\\' );
            size_t end = path.getFormatedPath().find_last_of( '.' );
            std::string fileName( path.getFormatedPath(), begin+1, end-begin-1 );
            fileName += ".jpg";

            str = ResourceGroupManager::getSingleton().findGroupContainingResource(texturePath);

            //text += "<img src='file:///d:\\test.jpg";
            text += "<img src='./NaviTmpTexture/";
            text += fileName;
            text +=	"' width=128 height=128/>	";

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
        }
    }

    text += "\"";
    mNavi->evaluateJS(text);

    // Go back to the main directory
    _chdir(mNavigator->getAvatarEditor()->mExecPath.c_str());
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarPropSound(Navi* caller, const Awesomium::JSArguments& args)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "GUI_AvatarProperties::avatarPropSound()");
    
    Avatar* user = mNavigator->getUserAvatar();

    float minDist = args.at(0).toDouble();
    float maxDist = args.at(1).toDouble();
    if (minDist > maxDist) minDist = maxDist;
    mNavi->evaluateJS("$('Sound3DMinDistValue').value=sound3DMinDist.getValue()");
    mNavi->evaluateJS("$('Sound3DMaxDistValue').value=sound3DMaxDist.getValue()");
    user->setVoiceDistances(minDist, maxDist);
}
//-------------------------------------------------------------------------------------
void GUI_AvatarProperties::avatarUpdateSliders(Vector3 pos, Vector3 ori, Vector3 scale)
{
    

    mNavi->evaluateJS("posX.onchange = function() {}");
    mNavi->evaluateJS("posY.onchange = function() {}");
    mNavi->evaluateJS("posZ.onchange = function() {}");
    mNavi->evaluateJS("oriX.onchange = function() {}");
    mNavi->evaluateJS("oriY.onchange = function() {}");
    mNavi->evaluateJS("oriZ.onchange = function() {}");
    mNavi->evaluateJS("scaleX.onchange = function() {}");
    mNavi->evaluateJS("scaleY.onchange = function() {}");
    mNavi->evaluateJS("scaleZ.onchange = function() {}");

    // position
    pos = pos * 100;
    mNavi->evaluateJS("posX.setValue(" + StringConverter::toString(int(pos.x)) + ")");
    mNavi->evaluateJS("posY.setValue(" + StringConverter::toString(int(pos.y)) + ")");
    mNavi->evaluateJS("posZ.setValue(" + StringConverter::toString(int(pos.z)) + ")");

    // orientation
    ori = ori * 100;
    mNavi->evaluateJS("oriX.setValue(" + StringConverter::toString(int(ori.x)) + ")");
    mNavi->evaluateJS("oriY.setValue(" + StringConverter::toString(int(ori.y)) + ")");
    mNavi->evaluateJS("oriZ.setValue(" + StringConverter::toString(int(ori.z)) + ")");

    // scale
    scale = scale * 100;
    mNavi->evaluateJS("scaleX.setValue(" + StringConverter::toString(int(scale.x)) + ")");
    mNavi->evaluateJS("scaleY.setValue(" + StringConverter::toString(int(scale.y)) + ")");
    mNavi->evaluateJS("scaleZ.setValue(" + StringConverter::toString(int(scale.z)) + ")");

    mNavi->evaluateJS("posX.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'posX'}).send()}");
    mNavi->evaluateJS("posY.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'posY'}).send()}");
    mNavi->evaluateJS("posZ.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'posZ'}).send()}");
    mNavi->evaluateJS("oriX.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'oriX'}).send()}");
    mNavi->evaluateJS("oriY.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'oriY'}).send()}");
    mNavi->evaluateJS("oriZ.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'oriZ'}).send()}");
    mNavi->evaluateJS("scaleX.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'scaleX'}).send()}");
    mNavi->evaluateJS("scaleY.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'scaleY'}).send()}");
    mNavi->evaluateJS("scaleZ.onchange = function() {new NaviData('AvatarPropSliders').add({slider:'scaleZ'}).send()}");

    // display the orientation sliders
    //mNavi->evaluateJS("$('oriX').style.display='block'");
    //mNavi->evaluateJS("$('oriY').style.display='block'");
    //mNavi->evaluateJS("$('oriZ').style.display='block'");
}

//-------------------------------------------------------------------------------------
