#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;


int main(){
    int client = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7002);
    
    cout<<"client begin..."<<endl;
    
    if (connect(client, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0){
        std::cout<<"connect to server failed!"<<endl;
        exit(-1);
    }else{
        cout<<"connected to server..."<<endl;
    }
   
    while (true){
        string s;
        if (getline(cin, s)){
            int n = send(client, s.c_str(), s.length(), 0);
            cout<<"Data send: "<<n<<s<<endl;
        }else
            break;
    }
    close(client);
    return 0;
}