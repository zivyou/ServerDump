#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>


using namespace std;

class Server{
private:
    int epollObj;
    int listenPort;
    int listenFd;
    
    int session(int fd){
        char buffer[1024] = {0, };
        int nread = read(fd, buffer, 1024);
        if (nread > 0){
            cout<<"Data Received: "<<buffer<<endl;
            return nread;
        }else if (nread == 0){
            close(fd);
            cout<<"connection has been closed."<<endl;
            return 0;
        }else{
            std::cerr<<"receive data failed!"<<endl;
            return -1;
        }
    }
    
    void setNonBlocking(int fd){
        int opts = fcntl(fd, F_GETFL);
        if (opts < 0){
            std::cerr<<"set non-blocking to "<<fd<<" failed!"<<endl;
            exit(-1);
        }
        
        opts |= O_NONBLOCK;
        if (fcntl(fd, F_SETFL, opts) < 0){
            std::cerr<<"set non-blocking to "<<fd<<" failed!"<<endl;
            exit(-1);
        }
        
    }
public:
    void init(int port){
        listenPort = port;
        epollObj = epoll_create(1024);
        
        listenFd = createListenFd();
        if (listenFd < 0){
            std::cerr<<"create listen fd failed!"<<endl;
            close(listenFd);
            close(epollObj);
            exit(-1);
        }
        
        if (joinEpollList(epollObj, listenFd) < 0){
            std::cerr<<"listen fd join epoll failed!"<<endl;
            close(listenFd);
            close(epollObj);
            exit(-1);
        };
    }
    
    int joinEpollList(int epollFd, int fd){
        struct epoll_event serverEvent;
        serverEvent.data.fd = fd;
        serverEvent.events = EPOLLIN | EPOLLET;
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
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverAddr.sin_port = htons(listenPort);
        bind(fd, (sockaddr*)&serverAddr, sizeof(serverAddr));
        listen(fd, 1024);
        
        return fd;
    }
    
    void start(){
        // work loop
        struct epoll_event events[1024];
        while (true){
            int fdAmount = epoll_wait(epollObj, events, 1024, -1);
            for (int i=0; i<fdAmount; i++){
                if (events[i].data.fd == listenFd){
                    struct sockaddr client;
                    socklen_t sockSize = sizeof(client);
                    int serviceFd = accept(listenFd, &client, &sockSize);
                    if (serviceFd > 0){
                        cout<<"accepted connection..."<<endl;
                        setNonBlocking(serviceFd);
                        struct epoll_event ev;
                        ev.data.fd = serviceFd;
                        ev.events = EPOLLIN | EPOLLET;
                        if (epoll_ctl(epollObj, EPOLL_CTL_ADD, serviceFd, &ev)){
                            std::cerr<<"set epoll event failed!"<<endl;
                            exit(-1);
                        }
                    }
                }else{
                    // service connection
                    int serviceFd = events[i].data.fd;
                    if (events[i].events & EPOLLIN){
                        if (session(serviceFd) < 0){
                            epoll_ctl(epollObj, EPOLL_CTL_DEL, serviceFd, NULL);
                        }
                    }
                }
            }
        }
    }
};

int main(){
    Server server;
    server.init(7002);
    cout<<"init ok"<<endl;
    server.start();
    return 0;
}