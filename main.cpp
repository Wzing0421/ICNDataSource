#include "UDPSocket.h"
#include "TCPSocket.h"
#include "Package.h"
#include "VideoTrans.h"
#include "MsgTrans.h"

#include <unordered_map>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace std;

void SplitString(string& s, vector<string>& v, const string& c);
string getFileName(string GlobalName);
bool judgeBinOrText(string GlobalName);

unordered_map<string, unsigned short> ContentName2VideoPort;
unordered_map<unsigned short, string> VideoPort2ContentName;

void fileCopy(char *file1, char *file2)  
{  
    // 最好对file1和file2进行判断  
      
    ifstream in(file1);  
    ofstream out(file2);  
    string filename;  
    string line;  
  
    while (getline (in, line))  
    {   
        char buf[1400];
        int i;
        for(i = 0; i < line.size(); i++){
            buf[i] = line[i];
        }
        buf[i] = '\0';
        out << buf << endl;
          
    }
}  

void binfileCopy(char* file1, char* file2){
    ifstream fin(file1, ios::binary);
    ofstream fout(file2, ios::binary);

    if(!fin){
        cout << "File open error" << endl;
        return;
    }
    char buf[1400];
    while (!fin.eof())
    {
        /* code */
        fin.read(buf, 1400);
        fout.write(buf, fin.gcount());
    }
    fin.close();
    fout.close();
}

void PublishBinFile(string file, string dstip){
    
    unsigned short dstport = 31900;
    TCPClient tcpclient;
    if(0 != tcpclient.tcpconnect(dstip, dstport)){
        cout << "tcp client connect error" << endl;
        return;
    }
    ifstream in(file, ios::binary);
    if(!in){
        cout << "File open error" << endl;
        return;
    }  

    int count = 1;
    while (!in.eof())  
    {
        DataPackage datapackage;
        string contentName = "pku/eecs/file/" + file + "/segment" + to_string(count);
        strcpy(datapackage.contentName, contentName.c_str());
        
        in.read(datapackage.data, 1400);
        datapackage.datasize = in.gcount();
        cout << datapackage.contentName << endl;
        datapackage.segmentNum = count;
        if(datapackage.datasize != 1400){
            datapackage.end = 1;
        }
        else{
            datapackage.end = 0;
        }
        char sendbuffer[1500];
        memcpy(sendbuffer, &datapackage, sizeof(sendbuffer));
        int len = tcpclient.sendbuf(sendbuffer, sizeof(sendbuffer));
        if(len <= 0){
            cout << "tcp send error" << endl;
            break;
        }
        if(datapackage.end == 1){
            break;
        }
        usleep(10000);
        count++;
    }
    tcpclient.tcpclose();
}

void PublishTextFile(string file, string dstip){

    unsigned short dstport = 31900;
    TCPClient tcpclient;
    if(0 != tcpclient.tcpconnect(dstip, dstport)){
        cout << "tcp client connect error" << endl;
        return;
    }

    ifstream in(file);  
    string filename;  
    string line;  
  
    int count = 1;
    while (getline (in, line))  
    {
        DataPackage datapackage;
        int i;
        for(i = 0; i < line.size(); i++){
            datapackage.data[i] = line[i];
        }
        datapackage.data[i] = '\0';
        string contentName = "pku/eecs/file/" + file + "/segment" + to_string(count);
        cout << contentName << endl;
        strcpy(datapackage.contentName, contentName.c_str());
        datapackage.datasize = i + 2;
        datapackage.segmentNum = count;
        if(in.eof()){
            datapackage.end = 1;
        }
        else{
            datapackage.end = 0;
        }
        char sendbuffer[1500];
        memcpy(sendbuffer, &datapackage, sizeof(sendbuffer));
        int len = tcpclient.sendbuf(sendbuffer, sizeof(sendbuffer));
        if(len <= 0){
            cout << "tcp send error" << endl;
            break;
        }
        //udpsocket.sendbuf(sendbuffer, sizeof(sendbuffer), dstip, dstport);       
        usleep(10000);
        count++;
    }
    tcpclient.tcpclose();
}

bool judgeVideo(string name){
    return name.find("video") != name.npos;
}

bool judgeFile(string name){
    return name.find("file") != name.npos;
}

bool judgeMsg(string name){
    return name.find("msg") != name.npos;
}

void *thread_startVideoReceiver(void *arg){
    ARGS *p = (ARGS*) arg;
    cout << "Create Receiving Video Thread! Port is: " << p->port << " Name is : " << p->GlobalName << endl;
    videoTrans videoreceiver(p->GlobalName, p->port, p->dstIP);
    videoreceiver.videoTransProc();
}

void *thread_startMsgReceiver(void *arg){
    MSGARGS *p = (MSGARGS*) arg;
    string globalname = p->GlobalName;
    string dstIP = p->dstIP;
    cout << "Create Receiving Msg Thread! Port is: " << p->port << " Name is : " << globalname << " IP is: " << dstIP << endl;
    
    MsgTrans msgreceiver(globalname, p->port, dstIP);
    msgreceiver.MsgTransProc();
}

void Publish(){
    string dstip;
    string name;

    while(true){
        cout << "请输入发布业务名称：" << endl;
        cin >> name;
        if(ContentName2VideoPort.find(name) != ContentName2VideoPort.end()){
            cout << "[Warning] The task: " << name << " is running!" << endl;
            continue;
        }
        cout << "请输入目的ICN地址" << endl;
        cin >> dstip;

        // first judge its a video stream or a file
        if(judgeVideo(name)){
            // start a new thread transfering video stream to ICN node
            pthread_t thid;
            unsigned short port;
            while (true)
            {
                port = rand() % 10000 + 10000;// get a random port : [10000, 20000)
                if(VideoPort2ContentName.find(port) != VideoPort2ContentName.end()) continue;
                VideoPort2ContentName.insert(make_pair(port, name));
                ContentName2VideoPort.insert(make_pair(name, port));
                break;
            }
            ARGS arg(name, port, dstip);
            
            if(pthread_create(&thid, NULL, thread_startVideoReceiver, (void*)&arg) != 0){
                cout << "Thread " << thid << "create error" << endl;
                continue;
            }
        }
        else if(judgeMsg(name)){
            // short message
            // start a new thread transfering message stream to ICN node
            pthread_t thid;
            unsigned short port;
            while (true)
            {
                port = rand() % 10000 + 10000;// get a random port : [10000, 20000)
                if(VideoPort2ContentName.find(port) != VideoPort2ContentName.end()) continue;
                VideoPort2ContentName.insert(make_pair(port, name));
                ContentName2VideoPort.insert(make_pair(name, port));
                break;
            }
            
            MSGARGS arg(name, port, dstip);
            if(pthread_create(&thid, NULL, thread_startMsgReceiver, (void*)&arg) != 0){
                cout << "Msg Thread " << thid << "create error" << endl;
                continue;
            }
        }
        // file 
        else{
            string fileName = name;
            if(judgeBinOrText(fileName)){
            //text
                PublishTextFile(fileName, dstip);
            }
            else{
                //binary
                PublishBinFile(fileName, dstip);
            }
        }
        
    }
}

void SplitString(string& s, vector<string>& v, const string& c){
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
 
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

string getFileName(string GlobalName){
    string FileName = "";
    vector<string> v;
    SplitString(GlobalName, v, "/");
    if(v.size() < 1){
        cout << "[Error]: Invalid GlobalName! GlobalName is: " << GlobalName << endl; 
        return FileName;
    }
    // 默认倒数第1个是文件名字 eg: pku/eecs/file/test1.txt
    return v[v.size() - 1];
}

bool judgeBinOrText(string GlobalName){
    string FileName = getFileName(GlobalName);
    if(FileName.find("txt") !=  FileName.npos) return true;
    return false;
}

int main(){
    Publish();
    return 0;
}