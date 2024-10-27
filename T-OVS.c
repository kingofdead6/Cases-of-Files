#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 256 
#define FILE_NAME "TnotOVS_data.dat"

typedef struct {
    int key;
    int data_size;
    char *data;
} Record;

typedef struct {
    int used_size; 
    char data[BLOCK_SIZE];
} Block;

void writeBlock(FILE *file, int blockNumber, Block *block) {
    fseek(file, blockNumber * sizeof(Block), SEEK_SET);
    fwrite(block, sizeof(Block), 1, file);
}

int readBlock(FILE *file, int blockNumber, Block *block) {
    fseek(file, blockNumber * sizeof(Block), SEEK_SET);
    return fread(block, sizeof(Block), 1, file);
}

void insertRecord(FILE *file, int key, const char *data) {
    int data_size = strlen(data) + 1;
    Block block;
    int blockNumber = 0;
    int inserted = 0;

    // Prepare the record with metadata
    Record new_record = { key, data_size, strdup(data) };

    while (readBlock(file, blockNumber, &block) == 1) {
        // Check if the block has enough space for the record
        if (block.used_size + sizeof(int) * 2 + data_size <= BLOCK_SIZE) {
            // Copy the record's metadata and data into the block
            memcpy(&block.data[block.used_size], &new_record.key, sizeof(int));
            memcpy(&block.data[block.used_size + sizeof(int)], &new_record.data_size, sizeof(int));
            memcpy(&block.data[block.used_size + sizeof(int) * 2], new_record.data, data_size);
            block.used_size += sizeof(int) * 2 + data_size;
            writeBlock(file, blockNumber, &block);
            inserted = 1;
            break;
        }
        blockNumber++;
    }

    // If no block has space, create a new one
    if (!inserted) {
        Block new_block = { .used_size = sizeof(int) * 2 + data_size };
        memcpy(new_block.data, &new_record.key, sizeof(int));
        memcpy(new_block.data + sizeof(int), &new_record.data_size, sizeof(int));
        memcpy(new_block.data + sizeof(int) * 2, new_record.data, data_size);
        writeBlock(file, blockNumber, &new_block);`6
    }
    free(new_record.data); // Free the allocated memory for data
}

void searchRecord(FILE *file, int key) {
    Block block;
    int blockNumber = 0;
    int found = 0;

    while (readBlock(file, blockNumber, &block) == 1) {
        int offset = 0;

        while (offset < block.used_size) {
            int record_key;
            int data_size;

            // Read the record's key and data size
            memcpy(&record_key, &block.data[offset], sizeof(int));
            memcpy(&data_size, &block.data[offset + sizeof(int)], sizeof(int));

            // Check if the current record matches the key
            if (record_key == key) {
                char *data = malloc(data_size);
                memcpy(data, &block.data[offset + sizeof(int) * 2], data_size);
                printf("Record found - Key: %d, Data: %s\n", record_key, data);
                free(data);
                found = 1;
                break;
            }

            offset += sizeof(int) * 2 + data_size; // Move to the next record
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
        int offset = 0;

        while (offset < block.used_size) {
            int record_key;
            int data_size;

            // Read the record's key and data size
            memcpy(&record_key, &block.data[offset], sizeof(int));
            memcpy(&data_size, &block.data[offset + sizeof(int)], sizeof(int));

            if (record_key == key) {
                int remaining_size = block.used_size - (offset + sizeof(int) * 2 + data_size);
                memmove(&block.data[offset], &block.data[offset + sizeof(int) * 2 + data_size], remaining_size);
                block.used_size -= sizeof(int) * 2 + data_size;
                writeBlock(file, blockNumber, &block);
                printf("Record with key %d deleted.\n", key);
                found = 1;
                break;
            }

            offset += sizeof(int) * 2 + data_size; // Move to the next record
        }
        if (found) break;
        blockNumber++;
    }

    if (!found) {
        printf("Record with key %d not found for deletion.\n", key);
    }
}

int main() {
    FILE *file = fopen(FILE_NAME, "rb+");
    if (!file) {
        file = fopen(FILE_NAME, "wb+");
        printf("Created a new file: %s\n", FILE_NAME);
    }

    insertRecord(file, 10, "Data for key 10");
    insertRecord(file, 20, "Some longer data for key 20 that might straddle");
    insertRecord(file, 15, "Data for key 15");

    searchRecord(file, 20);

    deleteRecord(file, 15);

    searchRecord(file, 15);

    fclose(file);
    return 0;
}
