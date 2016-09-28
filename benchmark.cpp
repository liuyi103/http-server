//
// Created by yicheng on 16-9-28.
//

#include <iostream>
#include <assert.h>
using namespace std;

int main() {
    time_t start = time(0), end;
    int correct_check = 0;
    for (int i = 0; i < 1000000; i++) {
        assert(system("curl -s -N 127.0.0.1:8080") == 0);
    }
    end = time(0);
    cout << end - start << endl;
    return 0;
}
