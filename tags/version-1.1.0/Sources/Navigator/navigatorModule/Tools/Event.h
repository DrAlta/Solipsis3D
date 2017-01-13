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

#ifndef __Event_h__
#define __Event_h__

#include <time.h>

namespace Solipsis {

enum MouseButton {
    MBNone = 0,
    MBLeft = 1,
    MBRight = MBLeft<<1,
    MBMiddle = MBRight<<1
};

/** This class contains state of the mouse.
 */
class MouseState {
public:
    int mX, mY, mZ;
    float mXreal, mYreal, mZreal;
    int mXrel, mYrel, mZrel;
    MouseButton mButtons;
};

//! Keyboard scan codes
enum KeyCode
{
    KC_UNASSIGNED  = 0x00,
    KC_ESCAPE      = 0x01,
    KC_1           = 0x02,
    KC_2           = 0x03,
    KC_3           = 0x04,
    KC_4           = 0x05,
    KC_5           = 0x06,
    KC_6           = 0x07,
    KC_7           = 0x08,
    KC_8           = 0x09,
    KC_9           = 0x0A,
    KC_0           = 0x0B,
    KC_MINUS       = 0x0C,    // - on main keyboard
    KC_EQUALS      = 0x0D,
    KC_BACK        = 0x0E,    // backspace
    KC_TAB         = 0x0F,
    KC_Q           = 0x10,
    KC_W           = 0x11,
    KC_E           = 0x12,
    KC_R           = 0x13,
    KC_T           = 0x14,
    KC_Y           = 0x15,
    KC_U           = 0x16,
    KC_I           = 0x17,
    KC_O           = 0x18,
    KC_P           = 0x19,
    KC_LBRACKET    = 0x1A,
    KC_RBRACKET    = 0x1B,
    KC_RETURN      = 0x1C,    // Enter on main keyboard
    KC_LCONTROL    = 0x1D,
    KC_A           = 0x1E,
    KC_S           = 0x1F,
    KC_D           = 0x20,
    KC_F           = 0x21,
    KC_G           = 0x22,
    KC_H           = 0x23,
    KC_J           = 0x24,
    KC_K           = 0x25,
    KC_L           = 0x26,
    KC_SEMICOLON   = 0x27,
    KC_APOSTROPHE  = 0x28,
    KC_GRAVE       = 0x29,    // accent
    KC_LSHIFT      = 0x2A,
    KC_BACKSLASH   = 0x2B,
    KC_Z           = 0x2C,
    KC_X           = 0x2D,
    KC_C           = 0x2E,
    KC_V           = 0x2F,
    KC_B           = 0x30,
    KC_N           = 0x31,
    KC_M           = 0x32,
    KC_COMMA       = 0x33,
    KC_PERIOD      = 0x34,    // . on main keyboard
    KC_SLASH       = 0x35,    // / on main keyboard
    KC_RSHIFT      = 0x36,
    KC_MULTIPLY    = 0x37,    // * on numeric keypad
    KC_LMENU       = 0x38,    // left Alt
    KC_SPACE       = 0x39,
    KC_CAPITAL     = 0x3A,
    KC_F1          = 0x3B,
    KC_F2          = 0x3C,
    KC_F3          = 0x3D,
    KC_F4          = 0x3E,
    KC_F5          = 0x3F,
    KC_F6          = 0x40,
    KC_F7          = 0x41,
    KC_F8          = 0x42,
    KC_F9          = 0x43,
    KC_F10         = 0x44,
    KC_NUMLOCK     = 0x45,
    KC_SCROLL      = 0x46,    // Scroll Lock
    KC_NUMPAD7     = 0x47,
    KC_NUMPAD8     = 0x48,
    KC_NUMPAD9     = 0x49,
    KC_SUBTRACT    = 0x4A,    // - on numeric keypad
    KC_NUMPAD4     = 0x4B,
    KC_NUMPAD5     = 0x4C,
    KC_NUMPAD6     = 0x4D,
    KC_ADD         = 0x4E,    // + on numeric keypad
    KC_NUMPAD1     = 0x4F,
    KC_NUMPAD2     = 0x50,
    KC_NUMPAD3     = 0x51,
    KC_NUMPAD0     = 0x52,
    KC_DECIMAL     = 0x53,    // . on numeric keypad
    KC_OEM_102     = 0x56,    // < > | on UK/Germany keyboards
    KC_F11         = 0x57,
    KC_F12         = 0x58,
    KC_F13         = 0x64,    //                     (NEC PC98)
    KC_F14         = 0x65,    //                     (NEC PC98)
    KC_F15         = 0x66,    //                     (NEC PC98)
    KC_KANA        = 0x70,    // (Japanese keyboard)
    KC_ABNT_C1     = 0x73,    // / ? on Portugese (Brazilian) keyboards
    KC_CONVERT     = 0x79,    // (Japanese keyboard)
    KC_NOCONVERT   = 0x7B,    // (Japanese keyboard)
    KC_YEN         = 0x7D,    // (Japanese keyboard)
    KC_ABNT_C2     = 0x7E,    // Numpad . on Portugese (Brazilian) keyboards
    KC_NUMPADEQUALS= 0x8D,    // = on numeric keypad (NEC PC98)
    KC_PREVTRACK   = 0x90,    // Previous Track (KC_CIRCUMFLEX on Japanese keyboard)
    KC_AT          = 0x91,    //                     (NEC PC98)
    KC_COLON       = 0x92,    //                     (NEC PC98)
    KC_UNDERLINE   = 0x93,    //                     (NEC PC98)
    KC_KANJI       = 0x94,    // (Japanese keyboard)
    KC_STOP        = 0x95,    //                     (NEC PC98)
    KC_AX          = 0x96,    //                     (Japan AX)
    KC_UNLABELED   = 0x97,    //                        (J3100)
    KC_NEXTTRACK   = 0x99,    // Next Track
    KC_NUMPADENTER = 0x9C,    // Enter on numeric keypad
    KC_RCONTROL    = 0x9D,
    KC_MUTE        = 0xA0,    // Mute
    KC_CALCULATOR  = 0xA1,    // Calculator
    KC_PLAYPAUSE   = 0xA2,    // Play / Pause
    KC_MEDIASTOP   = 0xA4,    // Media Stop
    KC_VOLUMEDOWN  = 0xAE,    // Volume -
    KC_VOLUMEUP    = 0xB0,    // Volume +
    KC_WEBHOME     = 0xB2,    // Web home
    KC_NUMPADCOMMA = 0xB3,    // , on numeric keypad (NEC PC98)
    KC_DIVIDE      = 0xB5,    // / on numeric keypad
    KC_SYSRQ       = 0xB7,
    KC_RMENU       = 0xB8,    // right Alt
    KC_PAUSE       = 0xC5,    // Pause
    KC_HOME        = 0xC7,    // Home on arrow keypad
    KC_UP          = 0xC8,    // UpArrow on arrow keypad
    KC_PGUP        = 0xC9,    // PgUp on arrow keypad
    KC_LEFT        = 0xCB,    // LeftArrow on arrow keypad
    KC_RIGHT       = 0xCD,    // RightArrow on arrow keypad
    KC_END         = 0xCF,    // End on arrow keypad
    KC_DOWN        = 0xD0,    // DownArrow on arrow keypad
    KC_PGDOWN      = 0xD1,    // PgDn on arrow keypad
    KC_INSERT      = 0xD2,    // Insert on arrow keypad
    KC_DELETE      = 0xD3,    // Delete on arrow keypad
    KC_LWIN        = 0xDB,    // Left Windows key
    KC_RWIN        = 0xDC,    // Right Windows key
    KC_APPS        = 0xDD,    // AppMenu key
    KC_POWER       = 0xDE,    // System Power
    KC_SLEEP       = 0xDF,    // System Sleep
    KC_WAKE        = 0xE3,    // System Wake
    KC_WEBSEARCH   = 0xE5,    // Web Search
    KC_WEBFAVORITES= 0xE6,    // Web Favorites
    KC_WEBREFRESH  = 0xE7,    // Web Refresh
    KC_WEBSTOP     = 0xE8,    // Web Stop
    KC_WEBFORWARD  = 0xE9,    // Web Forward
    KC_WEBBACK     = 0xEA,    // Web Back
    KC_MYCOMPUTER  = 0xEB,    // My Computer
    KC_MAIL        = 0xEC,    // Mail
    KC_MEDIASELECT = 0xED     // Media Select
};

enum EvtType {
    ETNone = 0,
    ETKeyPressed,
    ETKeyReleased,
    ETMousePressed,
    ETMouseReleased,
    ETMouseMoved,
    ETTick,
    ETTermRequested,
    ETTermCompleted,
    ETUser
};

typedef struct KeyboardEvt {
    EvtType mType;
    KeyCode mKey;
} KeyboardEvt;

typedef struct MouseEvt {
    EvtType mType;
    MouseState mState;
} MouseEvt;

typedef struct UserEvt {
    EvtType mType;
    int mCode;
    void* mData1;
    void* mData2;
} UserEvt;

typedef union {
    EvtType mType;
    KeyboardEvt mKeyboard;
    MouseEvt mMouse;
    UserEvt mUser;
} Evt;

/** This class represents a keyboard listener.
 */
class KeyboardEventListener
{
public:
    virtual bool keyPressed(const KeyboardEvt& evt) = 0;
    virtual bool keyReleased(const KeyboardEvt& evt) = 0;
};

/** This class represents a mouse listener.
 */
class MouseEventListener
{
public:
    virtual bool mouseMoved(const MouseEvt& evt) = 0;
    virtual bool mousePressed(const MouseEvt& evt) = 0;
    virtual bool mouseReleased(const MouseEvt& evt) = 0;
};

/** This class represents an event occured into an application.
 */
class Event
{
protected:
    time_t mTimestamp;
    Evt mEvt;

public:
    Event() { setEvent(-1, 0); }
    Event(time_t timestamp, const Evt* evt) { setEvent(timestamp, evt); }
    virtual ~Event() {}

    void setEvent(time_t timestamp, const Evt* evt)
    {
        if (evt == 0)
            mEvt.mType = ETNone;
        else
            memcpy(&mEvt, evt, sizeof(mEvt));
        mTimestamp = timestamp;
    }
    time_t getTimestamp() const { return mTimestamp; }
    const Evt& getEvt() const { return mEvt; }
};

} // namespace Solipsis

#endif // #ifndef __Event_h__