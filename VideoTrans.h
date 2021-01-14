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

    /**
     * dst IP for ICN Node: e.g: 162.105.85.235
     */
    string dstIP;

public:
    
    videoTrans(string _GlobalName, unsigned short _port, string _dstIP);
    ~videoTrans();
    void videoTransProc();

};
#endif