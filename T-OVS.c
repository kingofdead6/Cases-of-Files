#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define Record structure with a key and data of variable size
typedef struct {
    int key;
    char *data;
} Record;

// Define Block structure that contains records, a pointer to the next block, and remaining space
typedef struct Block {
    Record *records;
    int record_count;
    int space_remaining;
    struct Block *next;
} Block;

// Define TVS structure that contains the head block and block size
typedef struct {
    Block *head;
    int block_size;
} TVS;

// Function to initialize the TVS structure
TVS* initializeTVS(int block_size) {
    TVS *tvs = (TVS*)malloc(sizeof(TVS));
    tvs->head = NULL;
    tvs->block_size = block_size;
    return tvs;
}

// Function to insert a record into TVS
void insertRecord(TVS *tvs, int key, char *data) {
    int data_size = strlen(data) + 1; // Including null terminator
    Block *current = tvs->head, *previous = NULL;
    Record new_record = {key, strdup(data)};
    
    // Find the last block or create the first block if necessary
    while (current != NULL && current->space_remaining < data_size) {
        previous = current;
        current = current->next;
    }

    // If no suitable block is found, create a new one
    if (current == NULL) {
        current = (Block*)malloc(sizeof(Block));
        current->records = (Record*)malloc(tvs->block_size * sizeof(Record));
        current->record_count = 0;
        current->space_remaining = tvs->block_size;
        current->next = NULL;

        if (previous) previous->next = current;
        else tvs->head = current;
    }

    // Add the new record to the current block and adjust space
    current->records[current->record_count] = new_record;
    current->record_count++;
    current->space_remaining -= data_size;
}

// Function to delete a record from TVS
void deleteRecord(TVS *tvs, int key) {
    Block *current = tvs->head;

    // Traverse blocks to find the record to delete
    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key == key) {
                // Free the data of the deleted record
                current->space_remaining += strlen(current->records[i].data) + 1;
                free(current->records[i].data);
                
                // Shift remaining records to fill the gap
                memmove(&current->records[i], &current->records[i + 1], (current->record_count - i - 1) * sizeof(Record));
                current->record_count--;
                return;
            }
        }
        current = current->next;
    }
}

// Function to search for a single record by key in TVS
Record* searchRecord(TVS *tvs, int key) {
    Block *current = tvs->head;

    // Sequentially search each block and record
    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key == key) {
                return &current->records[i];
            }
        }
        current = current->next;
    }
    return NULL;
}

// Function to search for all records within a specific key range in TVS
void searchRange(TVS *tvs, int start_key, int end_key) {
    Block *current = tvs->head;

    // Sequentially check each record and print if in range
    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key >= start_key && current->records[i].key <= end_key) {
                printf("Key: %d, Data: %s\n", current->records[i].key, current->records[i].data);
            }
        }
        current = current->next;
    }
}

// Main function to demonstrate the TVS operations
int main() {
    TVS *tvs = initializeTVS(5);

    // Inserting records
    insertRecord(tvs, 10, "Data for key 10");
    insertRecord(tvs, 20, "Data for key 20");
    insertRecord(tvs, 15, "Data for key 15");
    insertRecord(tvs, 25, "Data for key 25");

    // Searching for a specific record
    Record *found = searchRecord(tvs, 15);
    if (found) {
        printf("Found record: Key: %d, Data: %s\n", found->key, found->data);
    } else {
        printf("Record not found.\n");
    }

    // Range search
    printf("Records in range 10 to 20:\n");
    searchRange(tvs, 10, 20);

    // Deleting a record
    deleteRecord(tvs, 15);
    printf("After deletion, records in range 10 to 20:\n");
    searchRange(tvs, 10, 20);

    // Free memory (not fully implemented for simplicity)
    free(tvs);

    return 0;
}
