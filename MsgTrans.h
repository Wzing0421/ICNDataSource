#ifndef __MSGTRANS_H__
#define __MSGTRANS_H__

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

class MsgTrans{
private:
    /**
     * name for short message stream
     */
    string GlobalName;

    /**
     * bind port for video socket
     */
    unsigned short port;

    UDPSocket MsgTransSocket;

    /**
     * dst IP for ICN Node: e.g: 162.105.85.235
     */
    string dstIP;

public:
    
    MsgTrans(string _GlobalName, unsigned short _port, string _dstIP);
    ~MsgTrans();
    void MsgTransProc();

};
#endif