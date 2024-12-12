#include <iostream>
#include "cache.cpp"

using namespace std;

#define MATRIX_SZ 32

int main() {

    for(int i = 0; i < MATRIX_SZ; ++i) {
        for(int j = 0; j < MATRIX_SZ; ++j) {
            cin >> matrix[i][j];
        }
    }

    cache_init();
    cycles = 0;
    hits = 0;
    misses = 0;

    int addr;
    for(int i = 0; i < 4000000; ++i) {
        cin >> addr;
        cache_access(addr);
    }

    mem_2_cache(0, MATRIX_SZ * MATRIX_SZ, MATRIX_SZ * MATRIX_SZ, 2 * MATRIX_SZ * MATRIX_SZ);
    matrix_multiplication(MATRIX_SZ, MATRIX_SZ, MATRIX_SZ , MATRIX_SZ);

    while(cin >> addr) {
        cache_access(addr);
    }

    cout << cycles << " cycles" << endl;
    cout << hits << " hits" << endl;
    cout << misses << " misses" << endl;
}