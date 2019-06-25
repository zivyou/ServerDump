#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>


using namespace std;

class Server{
private:
    int epollObj;
    int listenPort;
    int listenFd;
public:
    void init(int port){
        listenPort = port;
        epollObj = epoll_create(1024);
        
        listenFd = createListenFd();
        if (listenFd < 0){
            close(listenFd);
            close(epollObj);
            exit(-1);
        }
        
        if (joinEpollList(epollObj, listenFd) < 0){
            close(listenFd);
            close(epollObj);
            exit(-1);
        };
    }
    
    int joinEpollList(int epollFd, int fd){
        struct epoll_event serverEvent;
        serverEvent.data.fd = fd;
        serverEvent.events = EPOLLIN;
        if (epoll_ctl(epollObj, EPOLL_CTL_ADD, listenFd, &serverEvent) != 0){
            std::cerr<<"set epoll event failed!"<<endl;
            return -1;
        }
        
        return 0;
    }
    
    int createListenFd(){
        int fd;
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0){
            std::cerr<<"create listen socket failed!"<<endl;
            return -1;
        }
        
        struct sockaddr_in serverAddr;
        memset(serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = listenPort;
        bind(fd, (sockaddr*)&serverAddr, sizeof(serverAddr));
        
        return fd;
    }
    
    void start(){
        // work loop
        struct epoll_event events;
        while (true){
            int fdAmount = epoll_wait(epollObj, events, 1024, -1);
            for (int i=0; i<fdAmount; i++){
                if (events[i].data.fd == listenFd){
                    if (events[i].events & EPOLLIN){
                        struct sockaddr_in client;
                        int serviceFd = accept(listenFd, &client, &sizeof(client));
                        
                    }
                }
            }
        }
    }
};

int main(){
    Server server;
    server.init(7002);
    server.start();
    return 0;
}