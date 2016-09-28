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

const static string http_error_hdr = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
const static string http_html_hdr = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static string http_index_html =
        "<html><head><title>This is Yicheng's mini project</title></head>"
                "<body><h1>For DTL</h1>"
                "<p>HTTP 1.1 SERVER</body></html>";

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
        add_event(epollfd,clifd,EPOLLIN);
    }
}

static string generate_response(MyHTTPRequest& request) {
    if (request.request_ == "GET") {
        if (request.filename_ == "/") {
            return http_html_hdr + http_index_html;
        }
        else {
            cout << "file not found" << endl;
            return http_error_hdr;
        }
    }
    if (request.request_ == "POST") {
        if (request.filename_ == "/") {
            return http_html_hdr + http_index_html;
        }
        else {
            cout << "file not found" << endl;
            return http_error_hdr;
        }
    }
    return http_error_hdr;
}
#endif //HTTPSERVER_MYUTILS_H
