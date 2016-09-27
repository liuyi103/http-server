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

#define BUF_LEN 1028
#define PORT 8080
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

static void add_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}

//删除事件
static void delete_event(int epollfd,int fd,int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}

//修改事件
static void modify_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
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

//处理接收到的连接
static void handle_accept(int epollfd,int listenfd){
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t  cliaddrlen;
    clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
    if (clifd == -1)
        perror("accept error:");
    else {
        printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);                       //添加一个客户描述符和事件
        add_event(epollfd,clifd,EPOLLIN);
    }
}



//事件处理函数
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
    //建立TCP套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed!\n");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    //这里要注意，端口号一定要使用htons先转化为网络字节序，否则绑定的实际端口
    //可能和你需要的不同
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in))) {
        perror("socket binding failed!\n");
        return -2;
    }

    // I am not sure whether the following part is needed.
//    if (!listen(sockfd, 128)) {
//        perror("prepare listening failed\n");
//        return -3;
//    };
    listen(sockfd, 128);
    struct epoll_event events[EPOLLEVENTS];

    epollfd = epoll_create(FDSIZE);

    add_event(epollfd, sockfd, EPOLLIN);

//循环等待
    for ( ; ; ){
        //该函数返回已经准备好的描述符事件数目
        int ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);
        //处理接收到的连接
        char buf[BUF_LEN];
        handle_events(epollfd,events,ret,sockfd,buf);
    }
}