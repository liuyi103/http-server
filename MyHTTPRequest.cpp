//
// Created by yicheng on 16-9-27.
//

#include "MyHTTPRequest.h"
#include <stdio.h>
#include <cstring>
#include <iostream>

MyHTTPRequest::MyHTTPRequest(char *buf) {
    cout << "a good call" << endl;
    char* current_line = buf;
    char* endline = strchr(buf, '\n');
    buf = endline + 1;
    *endline = 0;
    sscanf(current_line, "%s %s %s", request_, filename_, protocol_);
    ts_ = time(0);
    while (true) {
        string key, value;
        int signal = sscanf(buf, "%s: %s\n", key, value);
        if (signal == EOF) {
            cout << "End of file" << endl;
            break;
        }
        if (signal != 0) {
            if (*buf == '\n') {
                body_ = buf + 1;
            } else {
                perror("unusual format");
            }
            break;
        }
        attributes_[key] = value;
        endline = strchr(buf, '\n');
        buf = endline + 1;
    }
}