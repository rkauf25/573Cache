#include <iostream>
#include <cmath>

using namespace std;

#define MAX_CACHE_SIZE 1024
#define MAX_BLOCK_SIZE 256
#define BLOCK_SIZE 32
#define NUM_SETS 32
#define BLOCKS_PER_SET 4
#define NUM_BANKS 4
#define MATRIX_SZ 32

typedef struct blockStruct
{
    float data[MAX_BLOCK_SIZE];
    int valid;
    int lruLabel;
    int set;
    int tag;
} blockStruct;

typedef struct cacheStruct
{
    blockStruct blocks[NUM_BANKS][MAX_CACHE_SIZE];
    int blockSize;
    int numSets;
    int blocksPerSet;
} cacheStruct;


/* Global Cache variable */
cacheStruct cache;
float matrix[MATRIX_SZ][MATRIX_SZ];
long cycles;
long hits;
long misses;

float mem_access(int addr) {
    cycles += 50;
    ++misses;

    if(addr >= 2 * MATRIX_SZ * MATRIX_SZ) {
        return 0;
    } else if(addr >= MATRIX_SZ * MATRIX_SZ) {
        addr -= MATRIX_SZ * MATRIX_SZ;
        return matrix[addr / MATRIX_SZ][addr % MATRIX_SZ];
    } else {
        return matrix[addr % MATRIX_SZ][addr / MATRIX_SZ];
    }

}

void cache_init(){
    cache.blockSize = BLOCK_SIZE;
    cache.numSets = NUM_SETS;
    cache.blocksPerSet = BLOCKS_PER_SET;
    for(int i = 0; i < NUM_SETS; ++i){ 
        for(int j = 0; j < BLOCKS_PER_SET; ++j){
            cache.blocks[0][i * BLOCKS_PER_SET + j].valid = 0;
            cache.blocks[0][i * BLOCKS_PER_SET + j].lruLabel = 0;
            cache.blocks[0][i * BLOCKS_PER_SET + j].set = i;

            cache.blocks[1][i * BLOCKS_PER_SET + j].valid = 0;
            cache.blocks[1][i * BLOCKS_PER_SET + j].lruLabel = 0;
            cache.blocks[1][i * BLOCKS_PER_SET + j].set = i;
        }
    }
}

float cache_access(int addr) {
    int setBits = floor(log2(cache.numSets));

    if(cache.numSets == 1){
        setBits = 0;
    }

    int blockOffsetBits = floor(log2(cache.blockSize));
    int bankIndexBits = floor(log2(NUM_BANKS));

    int tag = addr >> (setBits + blockOffsetBits + bankIndexBits);

    int bank_index = 0;
    for(int i = 0; i < bankIndexBits; ++i){ //separating out bank index
        bank_index = bank_index << 1;
        bank_index += (addr >> (bankIndexBits + setBits + blockOffsetBits - i - 1)) & 1;
    }

    int set = 0;
    for(int i = 0; i < setBits; ++i){ //separating out set index
        set = set << 1;
        set += (addr >> (setBits + blockOffsetBits - i - 1)) & 1;
    }

    int blockOffset = 0;
    for(int i = 0; i < blockOffsetBits; ++i){
        blockOffset = blockOffset << 1;
        blockOffset += (addr >> (blockOffsetBits - i - 1)) & 1;
    }

    for(int i = 0; i < cache.blocksPerSet; ++i){
        if(cache.blocks[bank_index][i + set * cache.blocksPerSet].tag == tag && cache.blocks[bank_index][i + set * cache.blocksPerSet].valid){ //hit

            for(int j = 0; j < cache.blocksPerSet; ++j){
                cache.blocks[bank_index][j + set * cache.blocksPerSet].lruLabel++;
            }
            cache.blocks[bank_index][i + set * cache.blocksPerSet].lruLabel = 0;
            cycles++;
            ++hits;
            return cache.blocks[bank_index][i + set * cache.blocksPerSet].data[blockOffset];
        }
    } //else miss


    int lru = 0;
    int lruIndex = 0;
    for(int i = 0; i < cache.blocksPerSet; ++i){
        if(!cache.blocks[bank_index][i + set * cache.blocksPerSet].valid){ //empty spot in set
            for(int j = 0; j < cache.blockSize; ++j){
                cache.blocks[bank_index][i + set * cache.blocksPerSet].data[j] = mem_access(addr);
            }
            cache.blocks[bank_index][i + set * cache.blocksPerSet].valid = 1;
            cache.blocks[bank_index][i + set * cache.blocksPerSet].tag = tag;
        

            
            for(int j = 0; j < cache.blocksPerSet; ++j){
                cache.blocks[bank_index][j + set * cache.blocksPerSet].lruLabel++;
            }
            cache.blocks[bank_index][i + set * cache.blocksPerSet].lruLabel = 0;
            return cache.blocks[bank_index][i + set * cache.blocksPerSet].data[blockOffset];
        }

        if(lru < cache.blocks[bank_index][i + set * cache.blocksPerSet].lruLabel){ //finds least recently used in the case of no empty space
            lru = cache.blocks[bank_index][i + set * cache.blocksPerSet].lruLabel;
            lruIndex = i;
        }
    } //cache is full

    for(int i = 0; i < cache.blockSize; ++i){
        cache.blocks[bank_index][lruIndex + set * cache.blocksPerSet].data[i] = mem_access(addr);
    }
    cache.blocks[bank_index][lruIndex + set * cache.blocksPerSet].tag = tag;

    for(int i = 0; i < cache.blocksPerSet; ++i){
        cache.blocks[bank_index][i + set * cache.blocksPerSet].lruLabel++;
    }
    cache.blocks[bank_index][lruIndex + set * cache.blocksPerSet].lruLabel = 0;

    return cache.blocks[bank_index][lruIndex + set * cache.blocksPerSet].data[blockOffset];
}

void mem_2_cache(uint32_t start_a_addr, uint32_t end_a_addr, uint32_t start_b_addr, uint32_t end_b_addr) {
    
    int i = start_a_addr;
    int block = 0;
    while(i < end_a_addr) {
        // cout << i << endl;
        int blockOffset = 0;
        while(blockOffset < BLOCK_SIZE) {
            cache.blocks[0][block].data[blockOffset] = mem_access(i);
            ++blockOffset;
            ++i;
        }
        ++block;
    }

    i = start_b_addr;
    block = 0;
    while(i < end_b_addr) {
        int blockOffset = 0;
        while(blockOffset < BLOCK_SIZE) {
            cache.blocks[1][block].data[blockOffset] = mem_access(i);
            ++blockOffset;
            ++i;
        }
        ++block;
    }
}

void matrix_multiplication(int rows_a, int rows_b, int cols_a, int cols_b) {

    // int i = 0;
    // int block = 0;
    // while(i < rows_a * cols_a) {
    //     int blockOffset = 0;
    //     while(blockOffset < BLOCK_SIZE) {
    //         cout << cache.blocks[0][block].data[blockOffset] << " " << cache.blocks[1][block].data[blockOffset] << endl;
    //         cache.blocks[0][block].data[blockOffset] *= cache.blocks[1][block].data[blockOffset];
    //         cout << cache.blocks[0][block].data[blockOffset] << endl;
    //         ++blockOffset;
    //         ++i;
    //     }
    //     ++block;
    // }

    for(int i = 0; i < rows_a; ++i) {
        for(int j = 0; j < cols_a; ++j) {
            for(int k = 0; k < cols_b; ++k) {
                if(k == 0) {
                    cache.blocks[3][i].data[j] = cache.blocks[0][i].data[k] * cache.blocks[1][i].data[k];
                } else {
                    cache.blocks[3][i].data[j] += cache.blocks[0][i].data[k] * cache.blocks[1][i].data[k];
                }
            }
        }
    }

    for(int i = 0; i < rows_a; ++i) {
        for(int j = 0; j < cols_a; ++j) {
            cout << cache.blocks[3][i].data[j] << " ";
        }
        cout << endl;
    }
}