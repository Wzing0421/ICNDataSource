#include "VideoTrans.h"

videoTrans::videoTrans(string _GlobalName, unsigned short _port){
    GlobalName = _GlobalName;
    port= _port;
    videoTransSocket.create(port);
}

videoTrans::~videoTrans(){}

void videoTrans::videoTransProc(){
    char recvVideoBuf[1500];
    string srcip_;
    unsigned short sport_;
    // ICN IP
    string dstIP = "162.105.85.235";
    unsigned short dstport = 51005;
    string contentName = "pku/eecs/video/test1.mp3";
    int lenrecv;

    while(true){
        lenrecv = videoTransSocket.recvbuf(recvVideoBuf, 1500, srcip_, sport_);
        if(lenrecv < 0){
            cout << "[Error] udpvideo recv error" << endl;
            break;
        }
        char sendbuf[1500];
        // default name length is 50
        memcpy(sendbuf, contentName.c_str(), contentName.size());
        memcpy(sendbuf + 50, recvVideoBuf, lenrecv);
        int lensend = videoTransSocket.sendbuf(sendbuf, lenrecv + 50, dstIP, dstport);
        cout << lensend << endl;
    }
    videoTransSocket.Close();
}