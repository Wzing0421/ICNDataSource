#include "VideoTrans.h"

videoTrans::videoTrans(string _GlobalName, unsigned short _port, string _dstIP){
    GlobalName = _GlobalName;
    port= _port;
    videoTransSocket.create(port);
    dstIP = _dstIP;
}

videoTrans::~videoTrans(){}

void videoTrans::videoTransProc(){
    char recvVideoBuf[1500];
    string srcip_;
    unsigned short sport_;
    // ICN IP
    unsigned short dstport = 51005;
    //string contentName = "pku/eecs/video/test1.mp4";
    int lenrecv;

    while(true){
        lenrecv = videoTransSocket.recvbuf(recvVideoBuf, 1500, srcip_, sport_);
        if(lenrecv < 0){
            cout << "[Error] udpvideo recv error" << endl;
            break;
        }
        char sendbuf[1500];
        // default name length is 50
        memcpy(sendbuf, GlobalName.c_str(), GlobalName.size());
        memcpy(sendbuf + 50, recvVideoBuf, lenrecv);
        int lensend = videoTransSocket.sendbuf(sendbuf, lenrecv + 50, dstIP, dstport);
    }
    videoTransSocket.Close();
}