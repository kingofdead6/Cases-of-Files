#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define Record structure with key and data of variable size
typedef struct {
    int key;
    char *data;
} Record;

// Define Block structure that contains records and a pointer to the next block
typedef struct Block {
    Record *records;
    int record_count;
    int space_remaining;
    struct Block *next;
} Block;

// Define TOV structure that contains the head block and block size
typedef struct {
    Block *head;
    int block_size;
} TOV;

// Function to initialize the TOV structure
TOV* initializeTOV(int block_size) {
    TOV *tov = (TOV*)malloc(sizeof(TOV));
    tov->head = NULL;
    tov->block_size = block_size;
    return tov;
}

// Function to insert a record into TOV
void insertRecord(TOV *tov, int key, char *data) {
    int data_size = strlen(data) + 1; // Including null terminator
    Block *current = tov->head, *previous = NULL;
    Record new_record = {key, strdup(data)};
    int inserted = 0;

    // Traverse blocks to find a place with enough space
    while (current != NULL && !inserted) {
        if (current->space_remaining >= data_size) {
            for (int i = 0; i < current->record_count; i++) {
                if (key < current->records[i].key) {
                    memmove(&current->records[i + 1], &current->records[i], (current->record_count - i) * sizeof(Record));
                    current->records[i] = new_record;
                    current->record_count++;
                    current->space_remaining -= data_size;
                    inserted = 1;
                    break;
                }
            }
        }
        if (!inserted) {
            previous = current;
            current = current->next;
        }
    }

    // If no suitable block found, create a new block
    if (!inserted) {
        Block *new_block = (Block*)malloc(sizeof(Block));
        new_block->records = (Record*)malloc(tov->block_size * sizeof(Record));
        new_block->records[0] = new_record;
        new_block->record_count = 1;
        new_block->space_remaining = tov->block_size - data_size;
        new_block->next = NULL;
        if (previous) previous->next = new_block;
        else tov->head = new_block;
    }
}

// Function to delete a record from TOV
void deleteRecord(TOV *tov, int key) {
    Block *current = tov->head;

    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key == key) {
                current->space_remaining += strlen(current->records[i].data) + 1;
                free(current->records[i].data);
                memmove(&current->records[i], &current->records[i + 1], (current->record_count - i - 1) * sizeof(Record));
                current->record_count--;
                return;
            }
        }
        current = current->next;
    }
}

// Function to search for a single record by key in TOV
Record* searchRecord(TOV *tov, int key) {
    Block *current = tov->head;

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

// Function to search for all records within a specific key range in TOV
void searchRange(TOV *tov, int start_key, int end_key) {
    Block *current = tov->head;

    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key >= start_key && current->records[i].key <= end_key) {
                printf("Key: %d, Data: %s\n", current->records[i].key, current->records[i].data);
            }
        }
        current = current->next;
    }
}

// Main function to demonstrate the TOV operations
int main() {
    TOV *tov = initializeTOV(5);

    // Inserting records
    insertRecord(tov, 10, "Data for key 10");
    insertRecord(tov, 20, "Data for key 20");
    insertRecord(tov, 15, "Data for key 15");
    insertRecord(tov, 25, "Data for key 25");

    // Searching for a specific record
    Record *found = searchRecord(tov, 15);
    if (found) {
        printf("Found record: Key: %d, Data: %s\n", found->key, found->data);
    } else {
        printf("Record not found.\n");
    }

    // Range search
    printf("Records in range 10 to 20:\n");
    searchRange(tov, 10, 20);

    // Deleting a record
    deleteRecord(tov, 15);
    printf("After deletion, records in range 10 to 20:\n");
    searchRange(tov, 10, 20);

    // Free memory (not fully implemented for simplicity)
    free(tov);

    return 0;
}
