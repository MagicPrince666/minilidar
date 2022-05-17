#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "udp_server.h"
#include "xepoll.h"

UdpServer::UdpServer() {
    listenfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if(listenfd_ < 0) {
        perror("socket");
        assert(listenfd_ > 0);
    }
    /* 将套接字和IP、端口绑定 */
    struct sockaddr_in addr_serv;
    memset(&addr_serv, 0, sizeof(struct sockaddr_in));  //每个字节都用0填充
    addr_serv.sin_family = AF_INET;                     //使用IPV4地址
    addr_serv.sin_port = htons(port_);                  //端口
    /* INADDR_ANY表示不管是哪个网卡接收到数据，只要目的端口是SERV_PORT，就会被该应用程序接收到 */
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);      //自动获取IP地址

    /* 绑定socket */
    if(bind(listenfd_, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0) {
        perror("bind error:");
        assert(listenfd_ > 0);
    }

    std::cout << "fd " << listenfd_ << " listening at " << port_ << std::endl;

    MY_EPOLL->EpollAdd(listenfd_, std::bind(&UdpServer::HandleSocket, this));
}

UdpServer::~UdpServer() {
    if(listenfd_ > 0) {
        MY_EPOLL->EpollDel(listenfd_);
        close(listenfd_);
    }
}

bool UdpServer::HandleSocket() {
    int send_num;
    char send_buf[20] = "i am server!";
    char recv_buf[20];
    struct sockaddr_in addr_client;
    int len = sizeof(addr_client);

    int recv_num = recvfrom(listenfd_, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);

    if(recv_num < 0) {  
        perror("recvfrom error:");  
        return false;
    }
    recv_buf[recv_num] = '\0';  
    printf("server receive %d bytes: %s\n", recv_num, recv_buf);  

    send_num = sendto(listenfd_, send_buf, recv_num, 0, (struct sockaddr *)&addr_client, len);  

    if(send_num < 0) {  
      perror("sendto error:");  
      return false;
    } 
    return true;
}