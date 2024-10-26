#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RECORD_SIZE 50 // Fixed record data size for simplicity

// Define Record structure with a fixed key and data size
typedef struct {
    int key;
    char data[RECORD_SIZE];
} Record;

// Define Block structure containing records and a pointer to the next block
typedef struct Block {
    Record *records;
    int record_count;
    struct Block *next;
} Block;

// Define TV structure that contains the head block and block size
typedef struct {
    Block *head;
    int block_size;
} TV;

// Function to initialize the TV structure
TV* initializeTV(int block_size) {
    TV *tv = (TV*)malloc(sizeof(TV));
    tv->head = NULL;
    tv->block_size = block_size;
    return tv;
}

// Function to save the current state of blocks to a file for visualization
void saveBlocksToFile(TV *tv, const char *filename) {
    FILE *file = fopen(filename, "w");
    Block *current = tv->head;
    int block_num = 1;

    while (current != NULL) {
        fprintf(file, "Block %d (Record count: %d):\n", block_num, current->record_count);
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key != -1) {  // Ignore deleted records
                fprintf(file, "  Record %d - Key: %d, Data: %s\n", i + 1, current->records[i].key, current->records[i].data);
            }
        }
        fprintf(file, "\n");
        current = current->next;
        block_num++;
    }

    fclose(file);
    printf("Current blocks saved to %s\n", filename);
}

// Function to insert a record into TV
void insertRecord(TV *tv, int key, const char *data) {
    Block *current = tv->head, *previous = NULL;
    Record new_record = {key};
    strncpy(new_record.data, data, RECORD_SIZE - 1);
    new_record.data[RECORD_SIZE - 1] = '\0'; // Ensure null termination

    // Find the last block or create the first block if necessary
    while (current != NULL && current->record_count >= tv->block_size) {
        previous = current;
        current = current->next;
    }

    // If no suitable block is found, create a new one
    if (current == NULL) {
        current = (Block*)malloc(sizeof(Block));
        current->records = (Record*)malloc(tv->block_size * sizeof(Record));
        current->record_count = 0;
        current->next = NULL;

        if (previous) previous->next = current;
        else tv->head = current;
    }

    // Add the new record to the current block
    current->records[current->record_count] = new_record;
    current->record_count++;

    // Print result in terminal
    printf("Inserted record: Key: %d, Data: %s\n", key, data);

    // Save blocks to file
    saveBlocksToFile(tv, "blocks_state.txt");
}

// Function to delete a record from TV by key
void deleteRecord(TV *tv, int key) {
    Block *current = tv->head;

    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key == key) {
                current->records[i].key = -1; // Mark key as deleted
                current->records[i].data[0] = '\0';
                printf("Deleted record with key %d\n", key);
                saveBlocksToFile(tv, "blocks_state.txt");
                return;
            }
        }
        current = current->next;
    }
    printf("Record with key %d not found for deletion.\n", key);
}

// Function to search for a single record by key in TV
Record* searchRecord(TV *tv, int key) {
    Block *current = tv->head;

    // Traverse blocks sequentially to find the key
    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key == key) {
                printf("Found record: Key: %d, Data: %s\n", current->records[i].key, current->records[i].data);
                return &current->records[i];
            }
        }
        current = current->next;
    }
    printf("Record with key %d not found.\n", key);
    return NULL;
}

// Function to search for all records within a specific key range in TV
void searchRange(TV *tv, int start_key, int end_key) {
    Block *current = tv->head;
    int found = 0;

    printf("Records in range %d to %d:\n", start_key, end_key);
    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key >= start_key && current->records[i].key <= end_key) {
                printf("  Key: %d, Data: %s\n", current->records[i].key, current->records[i].data);
                found = 1;
            }
        }
        current = current->next;
    }
    if (!found) {
        printf("No records found in the specified range.\n");
    }
}

// Main function to demonstrate the TV operations
int main() {
    TV *tv = initializeTV(3);

    // Insert records
    insertRecord(tv, 10, "Data for key 10");
    insertRecord(tv, 20, "Data for key 20");
    insertRecord(tv, 15, "Data for key 15");
    insertRecord(tv, 25, "Data for key 25");

    // Search for a specific record
    searchRecord(tv, 15);

    // Range search
    searchRange(tv, 10, 20);

    // Delete a record
    deleteRecord(tv, 15);

    // Verify deletion by searching again
    searchRecord(tv, 15);

    // Free memory (not fully implemented for simplicity)
    free(tv);

    return 0;
}
