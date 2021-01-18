#include "MsgTrans.h"

MsgTrans::MsgTrans(string _GlobalName, unsigned short _port, string _dstIP){
    GlobalName = _GlobalName;
    port= _port;
    MsgTransSocket.create(port);
    dstIP = _dstIP;
}

MsgTrans::~MsgTrans(){}

void MsgTrans::MsgTransProc(){
    char recvMsgBuf[1500];
    string srcip_;
    unsigned short sport_;
    // ICN IP
    //unsigned short dstport = 31903;
    unsigned short dstport = 51002;
    //string contentName = "pku/eecs/video/test1.mp4";
    int lenrecv;
    unsigned long long count = 0;
    while(true){
        
        count++;
        // this is only data message that needs contentName
        lenrecv = MsgTransSocket.recvbuf(recvMsgBuf, 1500, srcip_, sport_);
        if(lenrecv < 0){
            cout << "[Error] udp short message recv error" << endl;
            break;
        }
        // add content Name
        DataPackage datapack;
        string contentName = GlobalName + "/segment" + to_string(count);
        memcpy(datapack.contentName, contentName.c_str(), contentName.size());
        memcpy(datapack.data, recvMsgBuf, lenrecv);
        datapack.end = 0;

        char sendbuf[1500];
        memcpy(sendbuf, &datapack, sizeof(DataPackage));
        int lensend = MsgTransSocket.sendbuf(sendbuf, sizeof(sendbuf), dstIP, dstport);
    }
    MsgTransSocket.Close();
}