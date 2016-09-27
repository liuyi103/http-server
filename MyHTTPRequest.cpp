//
// Created by yicheng on 16-9-27.
//

#include "MyHTTPRequest.h"
#include <stdio.h>
#include <cstring>
#include <iostream>

#define STRSIZE 1024

MyHTTPRequest::MyHTTPRequest(char *buf) {
    char request[STRSIZE], filename[STRSIZE], protocol[STRSIZE];
    char *endline;
    int signal = sscanf(buf, "%s %s %s\n", request, filename, protocol);
    endline = strchr(buf, '\n');
    buf = endline + 1;
    request_ = request;
    filename_ = filename;
    protocol_ = protocol;
    ts_ = time(0);
    if (signal < 1) {
        cout << "incorrect request" << endl;
        return;
    }
    while (true) {
        if (strlen(endline) == 1) {
            body_ = "";
            break;
        }
        char keyStr[STRSIZE], valueStr[STRSIZE];
        signal = sscanf(buf, "%s %s\n", keyStr, valueStr);
        if (signal == EOF) {
            cout << "End of file" << endl;
            break;
        }
        if (signal == 0) {
            if (*buf == '\n') {
                body_ = buf + 1;
            } else {
                perror("unusual format");
            }
            break;
        }
        endline = strchr(buf, '\n');
        if (endline == nullptr) {
            attributes_[static_cast<string>(keyStr)] = static_cast<string>(valueStr);
            break;
        }
        if (*(endline + 1) == '\n') {
            body_ = endline + 2;
            break;
        }
        attributes_[static_cast<string>(keyStr)] = static_cast<string>(valueStr);
        buf = endline + 1;
    }
}