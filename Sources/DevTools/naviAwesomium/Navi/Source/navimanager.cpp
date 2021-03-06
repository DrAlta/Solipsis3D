/*
	This file is part of NaviLibrary, a library that allows developers to create and 
	interact with web-content as an overlay or material in Ogre3D applications.

	Copyright (C) 2009 Adam J. Simmons
	http://princeofcode.com/navilibrary.php

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "NaviManager.h"
#include "Navi.h"
#include <algorithm>
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <direct.h>
#include <stdlib.h>
#endif

using namespace NaviLibrary;
using namespace NaviLibrary::NaviUtilities;

template<> NaviManager* Singleton<NaviManager>::instance = 0;

#define TIP_SHOW_DELAY 0.7
#define TIP_ENTRY_DELAY 2.0

NaviManager::NaviManager(Ogre::Viewport* defaultViewport, const std::string &baseDirectory)
	: webCore(0), focusedNavi(0), mouseXPos(0), mouseYPos(0), mouseButtonRDown(false), mouseButtonLDown(false), zOrderCounter(5), 
	defaultViewport(defaultViewport), tooltipParent(0), lastTooltip(0), tooltipShowTime(0), isDraggingFocusedNavi(0),
	keyboardFocusedNavi(0)
{
	webCore = new Awesomium::WebCore(Awesomium::LOG_VERBOSE);
	webCore->setBaseDirectory(NaviUtilities::getCurrentWorkingDirectory() + baseDirectory + "\\");
	keyboardHook = new Impl::KeyboardHook(this);

	tooltipNavi = new Navi("__tooltip", 250, 50, NaviPosition(0, 0), false, 70, 199, Front, defaultViewport);
	tooltipNavi->hide();
	tooltipNavi->setTransparent(true);
	tooltipNavi->loadFile("tooltip.html");
	tooltipNavi->bind("resizeTooltip", NaviDelegate(this, &NaviManager::onResizeTooltip));

// BEGIN GREG : adapted by TOF
    minZOrder = 0;
    maxZOrder = 650;
// END GREG

}

NaviManager::~NaviManager()
{
	delete keyboardHook;

	for(iter = activeNavis.begin(); iter != activeNavis.end();)
	{
		Navi* toDelete = iter->second;

// BEGIN GREG : adapted by TOF
        mtlNameNaviNameMap.erase(mtlNameNaviNameMap.find(toDelete->getMaterialName()));
 // END GREG
        iter = activeNavis.erase(iter);

// BEGIN GREG : adapted by TOF
        if(focusedNavi == toDelete) focusedNavi = 0;
// END GREG

		delete toDelete;
	}

	delete tooltipNavi;

	if(webCore)
		delete webCore;
}

NaviManager& NaviManager::Get()
{
	if(!instance)
		OGRE_EXCEPT(Ogre::Exception::ERR_RT_ASSERTION_FAILED, 
			"An attempt was made to retrieve the NaviManager Singleton before it has been instantiated! Did you forget to do 'new NaviManager(renderWin)'?", 
			"NaviManager::Get");

	return *instance;
}

NaviManager* NaviManager::GetPointer()
{
	return instance;
}

void NaviManager::Update()
{
	webCore->update();

	std::map<std::string,Navi*>::iterator end;
	end = activeNavis.end();
	iter = activeNavis.begin();

	while(iter != end)
	{
		if(iter->second->okayToDelete)
		{
			Navi* naviToDelete = iter->second;
// BEGIN GREG : adapted by TOF
            mtlNameNaviNameMap.erase(mtlNameNaviNameMap.find(naviToDelete->getMaterialName()));
// END GREG
            iter = activeNavis.erase(iter);
			if(focusedNavi == naviToDelete)
			{
				focusedNavi = 0;
				isDraggingFocusedNavi = false;
// BEGIN GREG
	            keyboardFocusedNavi = 0;
// END GREG
			}

			delete naviToDelete;
		}
		else
		{
			iter->second->update();
			iter++;
		}
	}

	tooltipNavi->update();

	if(tooltipShowTime)
	{
		if(tooltipShowTime < tooltipTimer.getMilliseconds())
		{
			tooltipNavi->show(true);
			tooltipNavi->setPosition(NaviPosition(mouseXPos, mouseYPos + 15));
			tooltipShowTime = 0;
			lastTooltip = tooltipTimer.getMilliseconds();
		}
	}
}

Navi* NaviManager::createNavi(const std::string &naviName, unsigned short width, unsigned short height, const NaviPosition &naviPosition, 
			bool asyncRender, int maxAsyncRenderRate, Tier tier, Ogre::Viewport* viewport)
{
// BEGIN GREG : adapted by TOF
 /*   if(activeNavis.find(naviName) != activeNavis.end())
		OGRE_EXCEPT(Ogre::Exception::ERR_RT_ASSERTION_FAILED, 
			"An attempt was made to create a Navi named '" + naviName + "' when a Navi by the same name already exists!", 
			"NaviManager::createNavi");*/
    iter = activeNavis.find(naviName);
    if (iter != activeNavis.end())
        if (iter->second->okayToDelete)
        {
            mtlNameNaviNameMap.erase(mtlNameNaviNameMap.find(iter->second->getMaterialName()));
            if(focusedNavi == iter->second) focusedNavi = 0;
            delete iter->second;
        }
        else
            OGRE_EXCEPT(Ogre::Exception::ERR_RT_ASSERTION_FAILED, 
            "An attempt was made to create a Navi named '" + naviName + "' when a Navi by the same name already exists!", 
            "NaviManager::createNavi");


	int highestZOrder = -1;
	int zOrder = 0;

	for(iter = activeNavis.begin(); iter != activeNavis.end(); iter++)
// BEGIN GREG
		if(!iter->second->isMaterialOnly())
// END GREG
		if(iter->second->overlay)
			if(iter->second->overlay->getTier() == tier)
				if(iter->second->overlay->getZOrder() > highestZOrder)
					highestZOrder = iter->second->overlay->getZOrder();

	if(highestZOrder != -1)
		zOrder = highestZOrder + 1;

    Navi* newNavi = new Navi(naviName, width, height, naviPosition, asyncRender, maxAsyncRenderRate, (Ogre::uchar)zOrder, tier, 
        viewport? viewport : defaultViewport);
	activeNavis[naviName] = newNavi;
    mtlNameNaviNameMap[newNavi->getMaterialName()] = naviName;

    return newNavi;
// END GREG
}

Navi* NaviManager::createNaviMaterial(const std::string &naviName, unsigned short width, unsigned short height, 
			bool asyncRender, int maxAsyncRenderRate, Ogre::FilterOptions texFiltering, const std::string &mtlName)
{
// BEGIN GREG : adapted by TOF
    if(activeNavis.find(naviName) != activeNavis.end())
		OGRE_EXCEPT(Ogre::Exception::ERR_RT_ASSERTION_FAILED, 
			"An attempt was made to create a Navi named '" + naviName + "' when a Navi by the same name already exists!", 
			"NaviManager::createNaviMaterial");


    iter = activeNavis.find(naviName);
    if (iter != activeNavis.end())
        if (iter->second->okayToDelete)
        {
            mtlNameNaviNameMap.erase(mtlNameNaviNameMap.find(iter->second->getMaterialName()));
            if(focusedNavi == iter->second) focusedNavi = 0;
            delete iter->second;
        }
        else
            OGRE_EXCEPT(Ogre::Exception::ERR_RT_ASSERTION_FAILED, 
            "An attempt was made to create a Navi named '" + naviName + "' when a Navi by the same name already exists!", 
            "NaviManager::createNaviMaterial");


    Navi* newNavi = new Navi(naviName, width, height, asyncRender, maxAsyncRenderRate, texFiltering, mtlName);
    activeNavis[naviName] = newNavi;
    mtlNameNaviNameMap[newNavi->getMaterialName()] = naviName;
    return newNavi;
// END GREG
}



Navi* NaviManager::getNavi(const std::string &naviName)
{
	iter = activeNavis.find(naviName);
	if(iter != activeNavis.end())
		return iter->second;

	return 0;
}

std::vector<Navi*> NaviManager::getNavis(const std::string& pattern)
{
	std::vector<Navi*> result;

	for(iter = activeNavis.begin(); iter != activeNavis.end(); iter++)
		if(NaviUtilities::wildcardCompare(pattern, iter->first))
			result.push_back(iter->second);

	return result;
}

void NaviManager::destroyNavi(const std::string &naviName)
{
	iter = activeNavis.find(naviName);
	if(iter != activeNavis.end())
		iter->second->okayToDelete = true;
}

void NaviManager::destroyNavi(Navi* naviToDestroy)
{
	if(naviToDestroy)
		naviToDestroy->okayToDelete = true;
}

void NaviManager::resetAllPositions()
{
	for(iter = activeNavis.begin(); iter != activeNavis.end(); iter++)
		if(!iter->second->isMaterialOnly())
			iter->second->resetPosition();
}

bool NaviManager::isAnyNaviFocused()
{
	if(focusedNavi)
		return true;

	return false;
}

Navi* NaviManager::getFocusedNavi()
{
	return focusedNavi;
}

bool NaviManager::hasKeyboardFocus()
{
	return !!keyboardFocusedNavi;
}

bool NaviManager::injectMouseMove(int xPos, int yPos)
{
	bool eventHandled = false;

	if(tooltipNavi->getVisibility())
		tooltipNavi->setPosition(NaviPosition(xPos, yPos + 15));

	if(focusedNavi && isDraggingFocusedNavi || focusedNavi && mouseButtonRDown)
	{
		if(focusedNavi->movable)
			focusedNavi->moveNavi(xPos-mouseXPos, yPos-mouseYPos);

		eventHandled = true;
	}
	else
	{
// BEGIN GREG : adapted by TOF
        //	if(mouseButtonRDown && focusedNavi)
        if(mouseButtonRDown && focusedNavi && !focusedNavi->isMaterialOnly())
// END GREG
        {
			focusedNavi->injectMouseMove(focusedNavi->getRelativeX(xPos), focusedNavi->getRelativeY(yPos));
			mouseXPos = xPos;
			mouseYPos = yPos;
	
			return true;
		}

		Navi* top = getTopNavi(xPos, yPos);

		if(top)
		{
			top->injectMouseMove(top->getRelativeX(xPos), top->getRelativeY(yPos));
			eventHandled = true;

			for(iter = activeNavis.begin(); iter != activeNavis.end(); ++iter)
				if(iter->second->ignoringBounds)
					if(!(iter->second->isPointOverMe(xPos, yPos) && iter->second->overlay->panel->getZOrder() < top->overlay->panel->getZOrder()))
						iter->second->injectMouseMove(iter->second->getRelativeX(xPos), iter->second->getRelativeY(yPos));

			if(tooltipParent)
				if(!tooltipParent->isMaterialOnly())
					if(tooltipParent != top)
						handleTooltip(0, L"");
		}
// BEGIN GREG : adapted by TOF
        else
        {
            for(iter = activeNavis.begin(); iter != activeNavis.end(); ++iter)
                if(iter->second->ignoringBounds)
                    iter->second->injectMouseMove(iter->second->getRelativeX(xPos), iter->second->getRelativeY(yPos));
        }
// END GREG

		if(tooltipParent)
			if(!tooltipParent->isMaterialOnly())
				if(!tooltipParent->isPointOverMe(xPos, yPos))
					handleTooltip(0, L"");
	}

	mouseXPos = xPos;
	mouseYPos = yPos;

	return eventHandled;
}

bool NaviManager::injectMouseWheel(int relScroll)
{
	if(focusedNavi)
	{
		focusedNavi->injectMouseWheel(relScroll / 12);
		return true;
	}

	return false;
}

bool NaviManager::injectMouseDown(int buttonID)
{
	if(buttonID == LeftMouseButton)
	{
		mouseButtonLDown = true;
		if(focusNavi(mouseXPos, mouseYPos))
		{
			int relX = focusedNavi->getRelativeX(mouseXPos);
			int relY = focusedNavi->getRelativeY(mouseYPos);

			focusedNavi->injectMouseDown(relX, relY);
		}
	}
	else if(buttonID == RightMouseButton)
	{
		mouseButtonRDown = true;
		focusNavi(mouseXPos, mouseYPos);
	}

	if(focusedNavi)
		return true;

	return false;
}

bool NaviManager::injectMouseUp(int buttonID)
{
	isDraggingFocusedNavi = false;

// BEGIN GREG : adapted by TOF
    //	if(buttonID == LeftMouseButton && focusedNavi)
    if(buttonID == LeftMouseButton && focusedNavi && !focusedNavi->isMaterialOnly())
// END GREG
	{
		if(focusedNavi)
			focusedNavi->injectMouseUp(focusedNavi->getRelativeX(mouseXPos), focusedNavi->getRelativeY(mouseYPos));

		mouseButtonLDown = false;
	}
	else if(buttonID == RightMouseButton)
	{     
        mouseButtonRDown = false;
	}

// BEGIN GREG : adapted by TOF
    //	if(focusedNavi)
    if(focusedNavi && !focusedNavi->isMaterialOnly())
// END GREG

		return true;

	return false;
}

bool NaviManager::focusNavi(int x, int y, Navi* selection)
{
// BEGIN GREG : adapted by TOF
    if (focusedNavi && focusedNavi->isModal)
        return true;
// END GREG
    
    deFocusAllNavis();
	Navi* naviToFocus = selection? selection : getTopNavi(x, y);

	if(!naviToFocus)
	{
		if(keyboardFocusedNavi)
// BEGIN GREG
//			if(!keyboardFocusedNavi->isMaterialOnly())
// END GREG
				keyboardFocusedNavi = 0;

		return false;
	}

// BEGIN GREG : adapted by TOF
if(!naviToFocus->isMaterialOnly())
{
// END GRED
	std::vector<Navi*> sortedNavis;

	for(iter = activeNavis.begin(); iter != activeNavis.end(); iter++)
		if(iter->second->overlay)
			if(iter->second->overlay->getTier() == naviToFocus->overlay->getTier())
				sortedNavis.push_back(iter->second);

	struct compare { bool operator()(Navi* a, Navi* b){ return(a->overlay->getZOrder() > b->overlay->getZOrder()); }};
	std::sort(sortedNavis.begin(), sortedNavis.end(), compare());

	if(sortedNavis.size())
	{
		if(sortedNavis.at(0) != naviToFocus)
		{
			unsigned int popIdx = 0;
			for(; popIdx < sortedNavis.size(); popIdx++)
				if(sortedNavis.at(popIdx) == naviToFocus)
					break;

			unsigned short highestZ = sortedNavis.at(0)->overlay->getZOrder();
			for(unsigned int i = 0; i < popIdx; i++)
				sortedNavis.at(i)->overlay->setZOrder(sortedNavis.at(i+1)->overlay->getZOrder());
			
			sortedNavis.at(popIdx)->overlay->setZOrder(highestZ);
		}
	}
// BEGIN GREG : adapted by TOF
}
// END GRED

	focusedNavi = naviToFocus;
// BEGIN GREG : adapted by TOF
    focusedNavi->setFocus(true);
// END GREG
    focusedNavi->webView->focus();
	isDraggingFocusedNavi = false;
	keyboardFocusedNavi = focusedNavi->hasInternalKeyboardFocus? focusedNavi : 0;

	return true;
}

Navi* NaviManager::getTopNavi(int x, int y)
{
	Navi* top = 0;

	for(iter = activeNavis.begin(); iter != activeNavis.end(); iter++)
	{
		if(!iter->second->isPointOverMe(x, y))
			continue;

		if(!top)
			top = iter->second;
		else
			top = top->overlay->panel->getZOrder() > iter->second->overlay->panel->getZOrder()? top : iter->second;
	}

	return top;
}

void NaviManager::deFocusAllNavis()
{
	for(iter = activeNavis.begin(); iter != activeNavis.end(); iter++)
		iter->second->webView->unfocus();

	focusedNavi = 0;
	isDraggingFocusedNavi = false;
	keyboardFocusedNavi = 0;
}

void NaviManager::handleKeyMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(keyboardFocusedNavi)
		keyboardFocusedNavi->webView->injectKeyboardEvent(0, msg, wParam, lParam);

	for(iter = activeNavis.begin(); iter != activeNavis.end(); iter++)
		if(iter->second->alwaysReceivesKeyboard && iter->second != keyboardFocusedNavi)
			iter->second->webView->injectKeyboardEvent(0, msg, wParam, lParam);
}

void NaviManager::onResizeTooltip(Navi* Navi, const Awesomium::JSArguments& args)
{
	if(args.size() != 2 || !tooltipParent)
		return;

	tooltipNavi->resize(args[0].toInteger(), args[1].toInteger());
	tooltipNavi->setPosition(NaviPosition(mouseXPos, mouseYPos + 15));

	if(lastTooltip + TIP_ENTRY_DELAY * 1000 > tooltipTimer.getMilliseconds())
	{
		tooltipNavi->show(true);
		lastTooltip = tooltipTimer.getMilliseconds();
	}
	else
	{
		tooltipShowTime = tooltipTimer.getMilliseconds() + TIP_SHOW_DELAY * 1000;
	}
}

void NaviManager::handleTooltip(Navi* tooltipParent, const std::wstring& tipText)
{
	tooltipShowTime = 0;
	tooltipNavi->hide(true);

	if(tipText.length())
	{
		this->tooltipParent = tooltipParent;
		tooltipNavi->evaluateJS("setTooltip(?)", JSArgs(std::string(tipText.begin(), tipText.end())));
	}
	else
	{
		this->tooltipParent = 0;
	}
}

void NaviManager::handleRequestDrag(Navi* caller)
{
	focusNavi(0, 0, caller);
	isDraggingFocusedNavi = true;
}

void NaviManager::handleKeyboardFocusChange(Navi* caller, bool isFocused)
{
	if(isFocused)
	{
		if(!caller->isMaterialOnly())
		{
			if(!caller->getOverlay()->isVisible)
			{
				caller->webView->unfocus();

				if(keyboardFocusedNavi == caller)
					keyboardFocusedNavi = 0;

				return;
			}
		}				

		keyboardFocusedNavi = caller;
		keyboardFocusedNavi->webView->focus();

		for(iter = activeNavis.begin(); iter != activeNavis.end(); iter++)
			if(iter->second != keyboardFocusedNavi)
				iter->second->webView->unfocus();
	}
	else if(caller == keyboardFocusedNavi)
	{
		keyboardFocusedNavi = 0;
	}
}



// BEGIN GREG : adapted by TOF
/**
* Retrieve a pointer to a Navi by its material name.
*
* @param	mtlName	The material name used by the Navi to retrieve.
*
* @return	If the Navi is found, returns a pointer to the Navi, otherwise returns 0.
*/
Navi* NaviManager::getNaviFromMtlName(const std::string &mtlName)
{
    std::map<std::string,std::string>::const_iterator it = mtlNameNaviNameMap.find(mtlName);
    if(it == mtlNameNaviNameMap.end())
        return 0;

    return getNavi(it->second);
}
// END GREG



// BEGIN GREG
void NaviManager::focusNavi(Navi* naviToFocus)
{
    focusNavi(0, 0, naviToFocus);
}

void NaviManager::setZOrderMinMax(unsigned short min, unsigned short max)
{
    minZOrder = min;
    maxZOrder = max;
    recomputeZOrder();
}


void NaviManager::recomputeZOrder()
{
    std::vector<Navi*> sortedNavis;
    for(iter = activeNavis.begin(); iter != activeNavis.end(); iter++)
        if(!iter->second->isMaterialOnly())
            sortedNavis.push_back(iter->second);
    struct compare { bool operator()(Navi* a, Navi* b){ return(a->overlay->getZOrder() > b->overlay->getZOrder()); }};
    std::sort(sortedNavis.begin(), sortedNavis.end(), compare());
    unsigned int nbNavis = (unsigned int)sortedNavis.size();
    zOrderCounter = (minZOrder < maxZOrder - nbNavis) ? minZOrder : maxZOrder - nbNavis;
    for (unsigned int i = nbNavis; i > 0; i--, zOrderCounter++)
        sortedNavis.at(i - 1)->overlay->setZOrder((zOrderCounter < minZOrder) ? minZOrder : zOrderCounter);
}
// END GREG
