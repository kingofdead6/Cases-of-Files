#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 3
#define RECORD_SIZE 100
#define FILE_NAME "TOFS_data.dat"

// Define the record structure
typedef struct {
    int key;
    char data[RECORD_SIZE];
} Record;

// Define the block structure to hold records
typedef struct {
    Record records[BLOCK_SIZE];
    int record_count;
} Block;

// Function to write a block to the file at a specified block number
void writeBlock(FILE *file, int blockNumber, Block *block) {
    fseek(file, blockNumber * sizeof(Block), SEEK_SET);
    fwrite(block, sizeof(Block), 1, file);
}

// Function to read a block from the file at a specified block number
int readBlock(FILE *file, int blockNumber, Block *block) {
    fseek(file, blockNumber * sizeof(Block), SEEK_SET);
    return fread(block, sizeof(Block), 1, file);
}

// Function to insert a record into the file in an ordered manner
void insertRecord(FILE *file, int key, const char *data) {
    Block block;
    int blockNumber = 0;
    int inserted = 0;

    while (readBlock(file, blockNumber, &block) == 1) {
        if (block.record_count < BLOCK_SIZE) {
            int i ; 
            for ( i = 0; i < block.record_count; i++) {
                if (block.records[i].key > key) break;
            }
            for (int j = block.record_count; j > i; j--) {
                block.records[j] = block.records[j - 1];
            }

            // Insert the new record
            block.records[i].key = key;
            strncpy(block.records[i].data, data, RECORD_SIZE - 1);
            block.records[i].data[RECORD_SIZE - 1] = '\0';
            block.record_count++;
            writeBlock(file, blockNumber, &block);
            inserted = 1;
            break;
        }
        blockNumber++;
    }

    // If no existing block has space, add a new block
    if (!inserted) {
        Block newBlock = { .record_count = 1 };
        newBlock.records[0].key = key;
        strncpy(newBlock.records[0].data, data, RECORD_SIZE - 1);
        newBlock.records[0].data[RECORD_SIZE - 1] = '\0';
        writeBlock(file, blockNumber, &newBlock);
    }
}

void searchRecord(FILE *file, int key) {
    Block block;
    int blockNumber = 0;
    int found = 0;

    while (readBlock(file, blockNumber, &block) == 1) {
        for (int i = 0; i < block.record_count; i++) {
            if (block.records[i].key == key) {
                printf("Record found - Key: %d, Data: %s\n", key, block.records[i].data);
                found = 1;
                break;
            }
        }
        if (found) break;
        blockNumber++;
    }
    if (!found) {
        printf("Record with key %d not found.\n", key);
    }
}

void deleteRecord(FILE *file, int key) {
    Block block;
    int blockNumber = 0;
    int found = 0;

    while (readBlock(file, blockNumber, &block) == 1) {
        for (int i = 0; i < block.record_count; i++) {
            if (block.records[i].key == key) {
                // Shift remaining records
                for (int j = i; j < block.record_count - 1; j++) {
                    block.records[j] = block.records[j + 1];
                }
                block.record_count--;
                writeBlock(file, blockNumber, &block);
                printf("Record with key %d deleted.\n", key);
                found = 1;
                break;
            }
        }
        if (found) break;
        blockNumber++;
    }
    if (!found) {
        printf("Record with key %d not found for deletion.\n", key);
    }
}

void searchRange(FILE *file, int start_key, int end_key) {
    Block block;
    int blockNumber = 0;
    int found = 0;

    printf("Records with keys in range [%d, %d]:\n", start_key, end_key);
    while (readBlock(file, blockNumber, &block) == 1) {
        for (int i = 0; i < block.record_count; i++) {
            if (block.records[i].key >= start_key && block.records[i].key <= end_key) {
                printf("Key: %d, Data: %s\n", block.records[i].key, block.records[i].data);
                found = 1;
            }
        }
        blockNumber++;
    }
    if (!found) {
        printf("No records found in the specified range.\n");
    }
}

int main() {
    FILE *file = fopen(FILE_NAME, "rb+");
    if (!file) {
        file = fopen(FILE_NAME, "wb+");
        printf("Created a new file: %s\n", FILE_NAME);
    }

    insertRecord(file, 10, "Data for key 10");
    insertRecord(file, 20, "Data for key 20");
    insertRecord(file, 15, "Data for key 15");
    insertRecord(file, 25, "Data for key 25");

    searchRecord(file, 15);

    searchRange(file, 10, 20);

    deleteRecord(file, 15);

    searchRange(file, 10, 20);

    fclose(file);
    return 0;
}
