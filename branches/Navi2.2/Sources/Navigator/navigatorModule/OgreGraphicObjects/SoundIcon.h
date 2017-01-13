/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Christophe Floutier

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

#ifndef __SoundIcon_h__
#define __SoundIcon_h__

using namespace Ogre;

namespace Solipsis {

class ColorInterpolator
{
public:
	ColorInterpolator(const ColourValue & from, const ColourValue & to)
	{
		mFrom = from; 
		mTo = to;
	}
	const ColourValue & getResult(Real fRatio)
	{
		Real invert = (1-fRatio);
		mResult.r = mTo.r *fRatio + mFrom.r*invert;
		mResult.g = mTo.g *fRatio + mFrom.g*invert;
		mResult.b = mTo.b *fRatio + mFrom.b*invert;
		mResult.a = mTo.a *fRatio + mFrom.a*invert;

		return mResult;
	}

private:
	ColourValue mFrom;
	ColourValue mTo;
	ColourValue mResult;
};

/** This class represents an The sound icon shown above the avatar.
*/
class SoundIcon 
{
public:
    /** Enumeration denoting the status of the icon **/
    enum SoundIcon_Status {
        Invisible,
        Showed,
		Animated
    };

	SoundIcon(SceneManager* pMgr, SceneNode* pNode, const String& name, Real yPos = 0, Real xSize = 0.5, Real ySize = 0.3);
	~SoundIcon();

	void setStatus(SoundIcon_Status status);
	SoundIcon_Status getStatus()
	{
		return m_status;
	}

	void animate(Real timeSinceLastFrame);

protected:
	// the billboard set
	BillboardSet* m_SoundIcon; 
	Billboard *m_pBoard;

	SceneNode* m_pParentNode;
	SceneManager* m_pMgr;

	SoundIcon_Status m_status;

	ColorInterpolator mCInterpolator;
	Real m_animationTime;
};

} // namespace Solipsis

#endif // #ifndef __SoundIcon_h__