//
// Created by yicheng on 16-9-27.
//

#ifndef HTTPSERVER_MYHTTPREQUEST_H
#define HTTPSERVER_MYHTTPREQUEST_H

#include <map>
#include <string>
#include <iostream>

using namespace std;

class MyHTTPRequest {
public:
    map<string, string> attributes_;
    string request_;
    string body_;
    string filename_;
    string protocol_;
    time_t ts_;
    MyHTTPRequest() {};
    MyHTTPRequest(char *buf);
};


#endif //HTTPSERVER_MYHTTPREQUEST_H
