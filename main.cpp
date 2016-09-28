#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <MyHTTPRequest.h>
#include <unordered_map>
#include <myutils.h>

#define PORT 8080
#define IPADDRESS   "127.0.0.1"
#define MAXSIZE     1024
#define LISTENQ     5
#define FDSIZE      15000
#define EPOLLEVENTS 100

unordered_map<int, MyHTTPRequest> latestRequest;

static void do_read(int epollfd,int fd,char *buf){
    int nread;
    nread = read(fd,buf,MAXSIZE);
    cout << "\nThis is nread " << nread << endl;
    if (nread == -1)     {
        perror("read error:");
        close(fd); //记住close fd
//        latestRequest.erase(fd);
        delete_event(epollfd,fd,EPOLLIN); //删除监听
    }
    else if (nread == 0)     {
        fprintf(stderr,"client close.\n");
        close(fd); //记住close fd
        delete_event(epollfd,fd,EPOLLIN); //删除监听
        return;
    }
    else {
        printf("read message is : %s",buf);
        latestRequest[fd] = MyHTTPRequest(buf);
        modify_event(epollfd,fd,EPOLLOUT);
    }
}

static void do_write(int epollfd,int fd,char *buf) {
    int nwrite;
    nwrite = write(fd,buf,strlen(buf));
    if (nwrite == -1){
        perror("write error:");
        close(fd);   //记住close fd
        delete_event(epollfd,fd,EPOLLOUT);  //删除监听
    }else{
        close(fd);   //记住close fd
        delete_event(epollfd,fd,EPOLLOUT);  //删除监听
    }
    memset(buf,0,MAXSIZE);
}

// This function can't contain any output.
static void handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char *buf)
{
    int i;
    int fd;
    for (i = 0;i < num;i++)
    {
        fd = events[i].data.fd;
        if ((fd == listenfd) &&(events[i].events & EPOLLIN))
            handle_accept(epollfd,listenfd);
        else if (events[i].events & EPOLLIN)
            do_read(epollfd,fd,buf);
        else if (events[i].events & EPOLLOUT) {
            strcpy(buf, generate_response(latestRequest[fd]).c_str());
            do_write(epollfd,fd,buf);
        }
    }
}

int main() {
    int sockfd, err, newfd, epollfd;
    struct sockaddr_in addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed!\n");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in))) {
        perror("socket binding failed!\n");
        return -2;
    }

    listen(sockfd, 20);
    struct epoll_event events[EPOLLEVENTS];

    epollfd = epoll_create(FDSIZE);

    add_event(epollfd, sockfd, EPOLLIN);

    setNonBlock(sockfd);
    for ( ; ; ){
        int ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);
        char buf[MAXSIZE];
        handle_events(epollfd,events,ret,sockfd,buf);
    }
}