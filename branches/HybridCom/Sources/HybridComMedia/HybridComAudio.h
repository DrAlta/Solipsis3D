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

#ifndef __HybridComMedia_h__
#define __HybridComMedia_h__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <time.h>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
#include <CTBasicThread.h>
#include <CTLog.h>
#include <CTSystem.h>
#include <CTStringHelpers.h>
*/

#include <CTLog.h>
using namespace CommonTools;

#define AUDIO	0
#define VIDEO	1

/** This class represents the HybridComMedia.
*/
class HybridComAudio
{
public:
	HybridComAudio (int type);
	//{
	//	if(type==AUDIO)audio=true;else audio=false;
	//	destPort=0;
	//	destHost.erase();
	//	ftest = fopen("c:\\development\\hybridcom\\testSpeex", "w");
	//	if (ftest==NULL){
	//		LOGHANDLER_LOGF(LogHandler::VL_WARNING, "ftest null", "bla");
	//	}
	//	else {
	//		LOGHANDLER_LOGF(LogHandler::VL_WARNING, "ftest ok", "bla");
	//	}
	//}
 
	void sleep (unsigned int mseconds){
		clock_t goal = mseconds + clock();
		while (goal > clock());
	}
	void sethost(std::string h){mediahost.append(h);}
	void sethlid(std::string id){hlid.append(id);}
	std::string gethlid(){return hlid;}
	std::string gethost(){return mediahost;}
	int winsockInit();
	int createSocket(int type, int *ptr_port, struct sockaddr_in *ptr_address);
	void sendVideoFrame (int socket, const char * buffer, int bufferlen);
	void setDestPort(int dp){destPort=dp;};
	void setRecvPort(int dp){recvPort=dp;};
	void setDestHost(std::string dh){destHost.append(dh);};
	
	int getDestPort(){return destPort;};
	int getRecvPort(){return recvPort;};
	const char * getDestHost(){return destHost.c_str();};
	FILE * ftest;

	char * readFrom(int s);

private:
	std::string hlid;		// hyperlink id
	std::string mediahost;	// dundal red5 media host

//	int port;			// dundal red5 media audio/video port;
//	int protocol;
	
	int destPort;		// La ou on envoie les frames.
	int recvPort;
	std::string destHost;

	bool running;
	bool audio;
	bool isAudio () {if(audio)return true;}
	char *buffer;
	int len;


protected:

};

#endif // #ifndef __HybridComMedia_h__