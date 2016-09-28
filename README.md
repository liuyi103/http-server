# http-server
This repository implements HTTP server 1.1 with pure Linux apis.

This project referred to 

[1] http://blog.csdn.net/ruizeng88/article/details/6682028

[2] https://segmentfault.com/a/1190000003063859

##Supported features

requests:
- GET
- POST

features:
- always alive
- concurrent connections via epoll

##Test plan

build the project with CMake;
Start the server;
Open a browser, visit 127.0.0.1:8080 (test GET, can open multiple tabs concurrently);
Input "1" as password (test POST).



