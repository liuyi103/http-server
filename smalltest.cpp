//
// Created by yicheng on 16-9-27.
//

#include <MyHTTPRequest.h>
#include <cstring>
#include <sstream>
#include <myutils.h>

int main() {
    char requestData[] = "GET / HTTP/1.1\n"
            "Host: 127.0.0.1:8081\n"
            "Connection: keep-alive\n"
            "Upgrade-Insecure-Requests: 1\n"
            "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\n"
            "Accept-Encoding: gzip, deflate, sdch\n"
            "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\n\nthis is body";
    MyHTTPRequest request(requestData);
    cout << generate_response(request) << endl;
    return 0;
}