/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef nsIMenu_h__
#define nsIMenu_h__

#include "nsISupports.h"
#include "nsStringFwd.h"

class nsIDocShell;
class nsIMenuBar;
class nsIMenu;
class nsIMenuItem;
class nsIMenuListener;
class nsIChangeManager;
class nsIContent;
class nsIMenuCommandDispatcher;


// {ab6cea83-00ff-11d5-bb6f-f432a43ead7c}
#define NS_IMENU_IID      \
{ 0xab6cea83, 0x00ff, 0x11d5, \
  { 0xbb, 0x6f, 0xf4, 0x32, 0xa4, 0x3e, 0xad, 0x7c } };


/**
 * Menu widget
 */
class nsIMenu : public nsISupports {

  public:
    NS_DEFINE_STATIC_IID_ACCESSOR(NS_IMENU_IID)

  /**
    * Creates the Menu
    *
    */
    NS_IMETHOD Create ( nsISupports * aParent, const nsAString &aLabel, const nsAString &aAccessKey, 
                          nsIChangeManager* aManager, nsIDocShell* aShell, nsIContent* aNode ) = 0;

   /**
    * Get the Menu's Parent
    *
    */
    NS_IMETHOD GetParent(nsISupports *&aParent) = 0;

   /**
    * Get the Menu label
    *
    */
    NS_IMETHOD GetLabel(nsString &aText) = 0;

   /**
    * Set the Menu label
    *
    */
    NS_IMETHOD SetLabel(const nsAString &aText) = 0;

	/**
    * Get the Menu Access Key
    *
    */
	NS_IMETHOD GetAccessKey(nsString &aText) = 0;
   
	/**
    * Set the Menu Access Key
    *
    */
	NS_IMETHOD SetAccessKey(const nsAString &aText) = 0;

	/**
    * Set the Menu enabled state
    *
    */
	NS_IMETHOD SetEnabled(PRBool aIsEnabled) = 0;

	/**
    * Get the Menu enabled state
    *
    */
	NS_IMETHOD GetEnabled(PRBool* aIsEnabled) = 0;
	
	/**
    * Query if this is the help menu. Mostly for MacOS voodoo.
    *
    */
	NS_IMETHOD IsHelpMenu(PRBool* aIsHelpMenu) = 0;
	
	/**
    * Adds a Menu Item
    *
    */
    NS_IMETHOD AddItem(nsISupports* aItem) = 0;

   /**
    * Adds a separator
    *
    */
    NS_IMETHOD AddSeparator() = 0;

   /**
    * Returns the number of menu items
    * This does count separators as items
    */
    NS_IMETHOD GetItemCount(PRUint32 &aCount) = 0;

   /**
    * Returns a Menu or Menu Item at a specified Index
    *
    */
    NS_IMETHOD GetItemAt(const PRUint32 aPos, nsISupports *& aMenuItem) = 0;

   /**
    * Inserts a Menu Item at a specified Index
    *
    */
    NS_IMETHOD InsertItemAt(const PRUint32 aPos, nsISupports * aMenuItem) = 0;

   /**
    * Removes an Menu Item from a specified Index
    *
    */
    NS_IMETHOD RemoveItem(const PRUint32 aPos) = 0;

   /**
    * Removes all the Menu Items
    *
    */
    NS_IMETHOD RemoveAll() = 0;

   /**
    * Gets Native MenuHandle
    *
    */
    NS_IMETHOD  GetNativeData(void** aData) = 0;

   /**
    * Sets Native MenuHandle
    *
    */
    NS_IMETHOD  SetNativeData(void* aData) = 0;
    
   /**
    * Adds menu listener for dynamic construction
    *
    */
    NS_IMETHOD AddMenuListener(nsIMenuListener * aMenuListener) = 0;

   /**
    * Removes menu listener for dynamic construction
    *
    */
    NS_IMETHOD RemoveMenuListener(nsIMenuListener * aMenuListener) = 0;

   /**
    * Get GetMenuContent
    *
    */
    NS_IMETHOD GetMenuContent(nsIContent ** aMenuContent) = 0;
    
};

// {FF2CE8E5-9755-4DFA-86A3-FBB558093C04}
#define NS_IMENU_MOZILLA_1_8_BRANCH_IID \
{ 0xFF2CE8E5, 0x9755, 0x4DFA, \
  { 0x86, 0xA3, 0xFB, 0xB5, 0x58, 0x09, 0xC3, 0x04 } };

class nsIMenu_MOZILLA_1_8_BRANCH : public nsIMenu
{
  public:
    NS_DEFINE_STATIC_IID_ACCESSOR(NS_IMENU_MOZILLA_1_8_BRANCH_IID)

   /**
    * Enable/disable native widget for a particular nsIMenuItem
    *
    */
    NS_IMETHOD ChangeNativeEnabledStatusForMenuItem(nsIMenuItem* aMenuItem,
                                                    PRBool aEnabled) = 0;

   /**
    * Retrieve the native menu and the index of the item within that menu.
    *
    */
    NS_IMETHOD GetMenuRefAndItemIndexForMenuItem(nsISupports* aMenuItem,
                                                 void**       aMenuRef,
                                                 PRUint16*    aMenuItemIndex) = 0;

   /**
    * Sets an appropriate icon for the menu.
    */
    NS_IMETHOD SetupIcon() = 0;
};

#endif
