#include "UDPSocket.h"
#include "Package.h"

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

void sendBinFile(string file, string dstip){
    
    unsigned short dstport = 51002;
    UDPSocket udpsocket;
    udpsocket.create(20022);
    
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
        udpsocket.sendbuf(sendbuffer, sizeof(sendbuffer), dstip, dstport);       
        if(datapackage.end == 1){
            break;
        }
        usleep(10000);
        count++;
    }
    udpsocket.Close();
}

void sendTextFile(string file, string dstip){
    
    unsigned short dstport = 51002;
    UDPSocket udpsocket;
    udpsocket.create(20021);

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
        udpsocket.sendbuf(sendbuffer, sizeof(sendbuffer), dstip, dstport);       
        usleep(10000);
        count++;
    }
    udpsocket.Close();
}

void sendfile(){
    string dstip;
    string fileName;

    while(true){
        cout << "请输入文件名称：" << endl;
        cin >> fileName;
        cout << "请输入目的ICN地址" << endl;
        cin >> dstip;

        if(judgeBinOrText(fileName)){
            //text
            sendTextFile(fileName, dstip);
        }
        else{
            //binary
            sendBinFile(fileName, dstip);
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
    sendfile();
    //sendTextFile();
    //sendBinFile();
    return 0;
}