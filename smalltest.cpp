//
// Created by yicheng on 16-9-27.
//

#include <MyHTTPRequest.h>
#include <cstring>
#include <sstream>
#include <myutils.h>

int main() {
    char requestData[] = "POST /foo.asp HTTP/1.1\n"
            "Host: 127.0.0.1:8080\n"
            "Connection: keep-alive\n"
            "Content-Length: 10\n"
            "Cache-Control: max-age=0\n"
            "Origin: http://127.0.0.1:8080\n"
            "Upgrade-Insecure-Requests: 1\n"
            "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36\n"
            "Content-Type: application/x-www-form-urlencoded\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\n"
            "Referer: http://127.0.0.1:8080/\n"
            "Accept-Encoding: gzip, deflate\n"
            "Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\n"
            "\n"
            "password=1";
    MyHTTPRequest request(requestData);
    cout << endl<<request.thebody_ << endl;
    return 0;
}