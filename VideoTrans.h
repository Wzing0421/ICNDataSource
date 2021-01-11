#ifndef __VIDEOTRANS_H__
#define __VIDEOTRANS_H__

#include <unordered_map>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string>

#include "UDPSocket.h"
#include "Package.h"

using namespace std;

class videoTrans{
private:
    /**
     * name for video stream
     */
    string GlobalName;

    /**
     * bind port for video socket
     */
    unsigned short port;

    UDPSocket videoTransSocket;

public:
    
    videoTrans(string _GlobalName, unsigned short _port);
    ~videoTrans();
    void videoTransProc();

};
#endif