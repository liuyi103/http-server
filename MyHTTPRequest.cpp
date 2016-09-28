//
// Created by yicheng on 16-9-27.
//

#include "MyHTTPRequest.h"
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <sstream>

#define STRSIZE 1024

MyHTTPRequest::MyHTTPRequest(char *buf) {
    char request[STRSIZE] = "", filename[STRSIZE] = "", protocol[STRSIZE] = "";
    char firstLine[STRSIZE];
    char key[STRSIZE], value[STRSIZE];
    istringstream sin(buf);
    sin.getline(firstLine, STRSIZE);
    // use sscanf to avoid no filename or protocol.
    int signal = sscanf(firstLine, "%s %s %s\n", request, filename, protocol);
    request_ = request;
    filename_ = filename;
    protocol_ = protocol;
    ts_ = time(0);
    if (signal < 1) {
        cout << "incorrect request" << endl;
        return;
    }
    while (sin.getline(key, STRSIZE, ':')) {
        if(key[0] == '\n' || key[1] == '\n') { // \r\n or \n
            thebody_ = (key + 2);
            cout << "body is "<< thebody_<<endl;
            return;
        }
        sin.getline(value, STRSIZE);
        attributes_[key] = value;
    }

}