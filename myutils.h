//
// Created by yicheng on 16-9-28.
//

#ifndef HTTPSERVER_MYUTILS_H
#define HTTPSERVER_MYUTILS_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <MyHTTPRequest.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

const static string http_error_hdr = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
const static string http_html_hdr = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static string http_index_html =
        "<html><head><title>This is Yicheng's mini project</title></head>"
                "<body><h1>For DTL</h1>";
const static string http_form_html = "<form action=\"foo.asp\" method=\"post\">\n"
        "  Password: <input type=\"text\" name=\"password\"><br>\n"
        "  <input type=\"submit\" value=\"Submit\">\n"
        "</form>";
const static string http_index_end =  "</body></html>";
const static string http_next = "<p> Post received and password matched.";


#define exit_if(r, ...) if(r) {printf(__VA_ARGS__); printf("%s:%d error no: %d error msg %s\n", __FILE__, __LINE__, errno, strerror(errno)); exit(1);}

void setNonBlock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    exit_if(flags<0, "fcntl failed");
    int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    exit_if(r<0, "fcntl failed");
}

static void add_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}

static void delete_event(int epollfd,int fd,int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}

static void modify_event(int epollfd,int fd,int state){
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}

static void handle_accept(int epollfd,int listenfd){
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t  cliaddrlen;
    clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
    if (clifd == -1)
        perror("accept error:");
    else {
        printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
        setNonBlock(clifd);
        add_event(epollfd,clifd,EPOLLIN);
    }
}

static string generate_response(MyHTTPRequest& request) {
    if (request.request_ == "GET") {
        if (request.filename_ == "/") {
            return http_html_hdr + http_index_html + http_form_html + http_index_end;
        }
        else {
            cout << "file not found" << endl;
            return http_error_hdr;
        }
    }
    if (request.request_ == "POST") {
        if (request.thebody_ == "password=1") {
            return http_html_hdr + http_index_html + http_next + http_index_end;
        }
        else {
            cout << endl << "lyc:" << request.thebody_ << endl;
            return http_html_hdr + http_index_html + http_form_html + http_index_end;
        }
    }
    return http_error_hdr;
}



#endif //HTTPSERVER_MYUTILS_H
