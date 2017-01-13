// HybridComMedia.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "HybridComAudio.h"
//#include <speex/speex.h>
//#include <speex/speex_callbacks.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

HybridComAudio::HybridComAudio (int type):
	ftest(NULL)
	{
		if(type==AUDIO)audio=true;else audio=false;
		destPort=0;
		destHost.erase();
		//ftest = fopen("c:\\development\\hybridcom\\testSpeex", "w");
		if (ftest==NULL){
			LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ftest null bla");
		}
		else {
			LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "ftest ok");
		}
		 buffer = (char *) malloc (1024);
		 len=0;
	}

int HybridComAudio::winsockInit(){
	WSADATA wsaData;
	WORD version;
	int error;
	version = MAKEWORD( 2, 0 );

	error = WSAStartup( version, &wsaData );

	/* check for error */
	if ( error != 0 )
	{
	    /* error occured */
	   fprintf (stdout, "winsock startup failed\n");
	   return -1;
	}

	/* check for correct version */
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
	     HIBYTE( wsaData.wVersion ) != 0 )
	{
	    /* incorrect WinSock version */
	    WSACleanup();
	    fprintf (stdout, "winsock init failed\n");
		return -1;
	}

	/* WinSock has been initialized */
	fprintf (stdout, "winsock init Okok\n");
	return 0;
}

int HybridComAudio::createSocket(int type, int *ptr_port, struct sockaddr_in *ptr_address) {
	int desc, lg = sizeof(struct sockaddr_in);
	struct sockaddr_in address;

	if ((desc=socket(AF_INET, type, 0))== -1){
		fprintf (stdout, "socket sys call failed\n");
		return -1;
	} 

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl (INADDR_ANY);
	address.sin_port = htons (*ptr_port);

	if (bind(desc, (struct sockaddr*) &address, lg) == -1) {
		fprintf (stdout, "bind sys call failed\n");
		closesocket(desc);
		return -1;
	}

	if (ptr_address != NULL)
		getsockname(desc, (struct sockaddr *) ptr_address, &lg);

	return desc;
}

char * HybridComAudio::readFrom (int socket) {
	struct sockaddr_in address;
	int lg = sizeof(address);
/*
	address.sin_family = AF_INET; 
	address.sin_port = htons(getRecvPort());
	address.sin_addr.s_addr = inet_addr(getDestHost());		
	
	address.sin_port = htons(3001);
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
*/
	len=recvfrom(socket, buffer, 1024, 0,(struct sockaddr *)&address, &lg);
	if (len==0) {
		return NULL;
	}
	buffer[len] = '\0';
	return buffer;
}

void HybridComAudio::sendVideoFrame (int socket, const char * buffer, int bufferlen) {
	struct sockaddr_in address;
	int lg = sizeof(address);

	address.sin_family = AF_INET; 
	address.sin_port = htons(getDestPort());
	address.sin_addr.s_addr = inet_addr(getDestHost());		
	
//	address.sin_port = htons(3000);
//	address.sin_addr.s_addr = inet_addr("127.0.0.1");		

	sendto(socket, buffer, bufferlen,
					0,(struct sockaddr *)&address, lg);
}

