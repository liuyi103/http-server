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

#define BUF_LEN 1028
#define PORT 8081
#define IPADDRESS   "127.0.0.1"
#define MAXSIZE     1024
#define LISTENQ     5
#define FDSIZE      15000
#define EPOLLEVENTS 100

const static char http_error_hdr[] = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char http_index_html[] =
        "<html><head><title>This is Yicheng's mini project</title></head>"
                "<body><h1>For DTL</h1>"
                "<p>HTTP 1.1 SERVER</body></html>";

unordered_map<int, MyHTTPRequest> latestRequest;

int http_send_file(char *filename, int sockfd)
{
    if(!strcmp(filename, "/")){
        //通过write函数发送http响应报文；报文包括HTTP响应头和响应内容--HTML文件
        write(sockfd, http_html_hdr, strlen(http_html_hdr));
        write(sockfd, http_index_html, strlen(http_index_html));
    }
    else{
        // 文件未找到情况下发送404error响应
        printf("%s:file not find!\n",filename);
        write(sockfd, http_error_hdr, strlen(http_error_hdr));
    }
    return 0;
}

//HTTP请求解析
void serve(char* buf, int sockfd){
    if(!strncmp(buf, "GET", 3)){
        char *file = buf + 4;
        char *space = strchr(file, ' ');
        *space = '\0';
        http_send_file(file, sockfd);
        return;
    }
    else{
        //其他HTTP请求处理，如POST，HEAD等 。这里我们只处理GET
        printf("unsupported request!\n");
        return;
    }
}

static void do_read(int epollfd,int fd,char *buf){
    int nread;
    nread = read(fd,buf,MAXSIZE);
    if (nread == -1)     {
        perror("read error:");
        close(fd); //记住close fd
        delete_event(epollfd,fd,EPOLLIN); //删除监听
    }
    else if (nread == 0)     {
        fprintf(stderr,"client close.\n");
        close(fd); //记住close fd
        delete_event(epollfd,fd,EPOLLIN); //删除监听
    }
    else {
        printf("read message is : %s",buf);
        //修改描述符对应的事件，由读改为写
        serve(buf, fd);
        latestRequest[fd] = MyHTTPRequest(buf);
        close(fd);
        delete_event(epollfd,fd,EPOLLIN);
//        modify_event(epollfd,fd,EPOLLOUT);
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
        modify_event(epollfd,fd,EPOLLIN);
    }
    memset(buf,0,MAXSIZE);
}

static void handle_events(int epollfd,struct epoll_event *events,int num,int listenfd,char *buf)
{
    int i;
    int fd;
    //进行遍历;这里只要遍历已经准备好的io事件。num并不是当初epoll_create时的FDSIZE。
    for (i = 0;i < num;i++)
    {
        fd = events[i].data.fd;
        //根据描述符的类型和事件类型进行处理
        if ((fd == listenfd) &&(events[i].events & EPOLLIN))
            handle_accept(epollfd,listenfd);
        else if (events[i].events & EPOLLIN)
            do_read(epollfd,fd,buf);
        else if (events[i].events & EPOLLOUT)
            do_write(epollfd,fd,buf);
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

    listen(sockfd, 128);
    struct epoll_event events[EPOLLEVENTS];

    epollfd = epoll_create(FDSIZE);

    add_event(epollfd, sockfd, EPOLLIN);

    for ( ; ; ){
        int ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);
        char buf[BUF_LEN];
        handle_events(epollfd,events,ret,sockfd,buf);
    }
}