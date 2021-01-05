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

void sendBinFile(){
    string dstip = "162.105.85.63";
    unsigned short dstport = 51002;
    UDPSocket udpsocket;
    udpsocket.create(20022);
    string file1;
    cout << "Please input file name: " << endl;
    cin >> file1;

    ifstream in(file1, ios::binary);
    if(!in){
        cout << "File open error" << endl;
        return;
    }  

    int count = 1;
    while (!in.eof())  
    {
        DataPackage datapackage;
        string contentName = "pku/eecs/file/" + file1 + "/segment" + to_string(count);
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

void sendTextFile(){
    string dstip = "162.105.85.247";
    unsigned short dstport = 51002;
    UDPSocket udpsocket;
    udpsocket.create(20021);
    string file1;
    cout << "Please input file name: " << endl;
    cin >> file1;

    ifstream in(file1);  
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
        string contentName = "pku/eecs/file/" + file1 + "/segment" + to_string(count);
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


int main(){

    sendTextFile();
    //sendBinFile();
    return 0;
}