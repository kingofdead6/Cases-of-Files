#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int key;
    char *data;
} Record;

typedef struct Block {
    Record *records;
    int record_count;
    struct Block *next;
} Block;

typedef struct {
    Block *head;
    int block_size;
} TOVS;

TOVS* initializeTOVS(int block_size) {
    TOVS *tovs = (TOVS*)malloc(sizeof(TOVS));
    tovs->head = NULL;
    tovs->block_size = block_size;
    return tovs;
}

void insertRecord(TOVS *tovs, int key, char *data) {
    Block *current = tovs->head, *previous = NULL;
    Record new_record = {key, strdup(data)};
    int inserted = 0;

    while (current != NULL && !inserted) {
        for (int i = 0; i < current->record_count; i++) {
            if (key < current->records[i].key) {
                memmove(&current->records[i + 1], &current->records[i], (current->record_count - i) * sizeof(Record));
                current->records[i] = new_record;
                current->record_count++;
                inserted = 1;
                break;
            }
        }
        if (!inserted) {
            previous = current;
            current = current->next;
        }
    }

    if (!inserted) {
        Block *new_block = (Block*)malloc(sizeof(Block));
        new_block->records = (Record*)malloc(tovs->block_size * sizeof(Record));
        new_block->records[0] = new_record;
        new_block->record_count = 1;
        new_block->next = NULL;
        if (previous) previous->next = new_block;
        else tovs->head = new_block;
    }
}

void deleteRecord(TOVS *tovs, int key) {
    Block *current = tovs->head;

    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key == key) {
                free(current->records[i].data);
                memmove(&current->records[i], &current->records[i + 1], (current->record_count - i - 1) * sizeof(Record));
                current->record_count--;
                return;
            }
        }
        current = current->next;
    }
}

Record* searchRecord(TOVS *tovs, int key) {
    Block *current = tovs->head;

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

void searchRange(TOVS *tovs, int start_key, int end_key) {
    Block *current = tovs->head;

    while (current != NULL) {
        for (int i = 0; i < current->record_count; i++) {
            if (current->records[i].key >= start_key && current->records[i].key <= end_key) {
                printf("Key: %d, Data: %s\n", current->records[i].key, current->records[i].data);
            }
        }
        current = current->next;
    }
}

int main() {
    TOVS *tovs = initializeTOVS(5);

    insertRecord(tovs, 10, "Data for key 10");
    insertRecord(tovs, 20, "Data for key 20");
    insertRecord(tovs, 15, "Data for key 15");
    insertRecord(tovs, 25, "Data for key 25");

    Record *found = searchRecord(tovs, 15);
    if (found) {
        printf("Found record: Key: %d, Data: %s\n", found->key, found->data);
    } else {
        printf("Record not found.\n");
    }

    printf("Records in range 10 to 20:\n");
    searchRange(tovs, 10, 20);

    deleteRecord(tovs, 15);
    printf("After deletion, records in range 10 to 20:\n");
    searchRange(tovs, 10, 20);

    free(tovs);

    return 0;
}
