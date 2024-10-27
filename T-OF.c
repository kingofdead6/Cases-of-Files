#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 3 
#define FILE_NAME "tnotof_file.dat"  

typedef struct {
    int key;     char data[100];  
} Record;

typedef struct {
    Record records[BLOCK_SIZE];
    int recordCount; 
} Block;

int readBlock(FILE *file, int blockNumber, Block *block) {
    fseek(file, blockNumber * sizeof(Block), SEEK_SET);
    return fread(block, sizeof(Block), 1, file);
}

void writeBlock(FILE *file, int blockNumber, Block *block) {
    fseek(file, blockNumber * sizeof(Block), SEEK_SET);
    fwrite(block, sizeof(Block), 1, file);
}

int search(FILE *file, int key) {
    Block block;
    int blockNumber = 0;
    rewind(file);  
    
    while (readBlock(file, blockNumber, &block)) {
        for (int i = 0; i < block.recordCount; i++) {
            if (block.records[i].key == key) {
                printf("Record found: Key = %d, Data = %s\n", key, block.records[i].data);
                return 1;
            }
        }
        blockNumber++;
    }
    printf("Record with key %d not found.\n", key);
    return 0;
}

void insert(FILE *file, int key, const char *data) {
    Block block;
    int blockNumber = 0;
    
    while (readBlock(file, blockNumber, &block)) {
        if (block.recordCount < BLOCK_SIZE) {
            // Add the new record to the current block
            block.records[block.recordCount].key = key;
            strcpy(block.records[block.recordCount].data, data);
            block.recordCount++;
            writeBlock(file, blockNumber, &block);
            printf("Record inserted: Key = %d, Data = %s\n", key, data);
            return;
        }
        blockNumber++;
    }
    
    // If no block has space, create a new block
    block.recordCount = 0;
    block.records[block.recordCount].key = key;
    strcpy(block.records[block.recordCount].data, data);
    block.recordCount++;
    writeBlock(file, blockNumber, &block);
    printf("Record inserted in a new block: Key = %d, Data = %s\n", key, data);
}

void delete(FILE *file, int key) {
    Block block, lastBlock;
    int blockNumber = 0;
    int lastBlockNumber = 0;
    
    rewind(file); 
    
    // Find the last block
    while (readBlock(file, lastBlockNumber, &lastBlock)) {
        lastBlockNumber++;
    }
    lastBlockNumber--; 
    
    while (readBlock(file, blockNumber, &block)) {
        for (int i = 0; i < block.recordCount; i++) {
            if (block.records[i].key == key) {
                // Replace with the last record
                if (lastBlockNumber == blockNumber && lastBlock.recordCount == 1) {
                    block.recordCount--;
                } else {
                    block.records[i] = lastBlock.records[lastBlock.recordCount - 1];
                    lastBlock.recordCount--;
                    writeBlock(file, lastBlockNumber, &lastBlock);
                }
                writeBlock(file, blockNumber, &block);
                printf("Record with key %d deleted.\n", key);
                return;
            }
        }
        blockNumber++;
    }
    printf("Record with key %d not found.\n", key);
}

void findInRange(FILE *file, int keyMin, int keyMax) {
    Block block;
    int blockNumber = 0;
    
    rewind(file);  
    printf("Records with keys in the range [%d, %d]:\n", keyMin, keyMax);
    
    while (readBlock(file, blockNumber, &block)) {
        for (int i = 0; i < block.recordCount; i++) {
            if (block.records[i].key >= keyMin && block.records[i].key <= keyMax) {
                printf("Key = %d, Data = %s\n", block.records[i].key, block.records[i].data);
            }
        }
        blockNumber++;
    }
}

int main() {
    FILE *file = fopen(FILE_NAME, "rb+");
    
    if (!file) {
        file = fopen(FILE_NAME, "wb+");
        printf("Created new file: %s\n", FILE_NAME);
    }
    
    insert(file, 10, "Record A");
    insert(file, 20, "Record B");
    insert(file, 30, "Record C");
    insert(file, 40, "Record D");
    insert(file, 50, "Record E");
    
    search(file, 20);
    
    delete(file, 30);
    
    findInRange(file, 10, 40);
    
    fclose(file);
    return 0;
}
