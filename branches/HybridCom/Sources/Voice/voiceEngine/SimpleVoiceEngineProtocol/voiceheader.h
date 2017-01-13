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

#ifndef VOICEHEADER_H
#define VOICEHEADER_H

// ---------------------------------------------------------------------------
// Packet types

enum VoicePacketType
{
    VP_LOGIN = 0,
    VP_AUDIO_TO_SERVER = 1,
    VP_AUDIO_TO_CLIENT = 2,
    VP_ENABLE_VOIP = 3
};
// GREG BEGIN
static char* VoicePacketTypeName[4] = {
    "VP_LOGIN",
    "VP_AUDIO_TO_SERVER",
    "VP_AUDIO_TO_CLIENT",
    "VP_ENABLE_VOIP"
};
// GREG END

// ---------------------------------------------------------------------------
// Packet flags

typedef unsigned char VoicePacketFlags;
static const VoicePacketFlags VPF_NONE = 0;
static const VoicePacketFlags VPF_POSDIRVEL = 1;
static const VoicePacketFlags VPF_DISTANCES = VPF_POSDIRVEL<<1;

// ---------------------------------------------------------------------------
// Audio data headers

struct VoicePacketHeader
{
    int format;			///< codec id
    unsigned short audioSize;	///< size of the encoded audio data in bytes
    unsigned short decodedAudioSize; ///< size of the decoded audio data in bytes
    unsigned short frames;		///< number of frames
    unsigned char flags;		///< flags
};  //  struct VoicePacketHeader

struct VoiceFrameHeader
{
    unsigned short frameSize;	///< size of the encoded frame in bytes
};  //  struct VoiceFrameHeader

#endif  //  VOICEHEADER_H
