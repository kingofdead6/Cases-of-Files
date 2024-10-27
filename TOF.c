#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FILE_NAME "TOF-try.dat"
#define BLOCK_SIZE 3

typedef struct {
    int key ;
    char data[100];
} Record ;

typedef struct {
   Record record[BLOCK_SIZE] ;
   int RecordCount ;
} Block ;

int ReadBlock(FILE *file , int BlockNumber , Block *block){
    fseek(file,BlockNumber *sizeof(Block) , SEEK_SET) ;
    return fread(block , sizeof(Block) ,1,file);
}

void WriteBlock (FILE *file , int BlockNumber , Block *block){
     fseek(file,BlockNumber *sizeof(Block) , SEEK_SET) ;
     fwrite(block , sizeof(Block) ,1,file);
}

int Search (FILE *file , int key){
    Block block ;
    int BlockNumber = 0 ;
    rewind(file);

    while (ReadBlock(file , BlockNumber , &block)){
        for (int i  = 0 ; i < block.RecordCount ; i++){
            if(block.record[i].key == key){
                printf("The Record with the key number :  %d was successfully found and it contained these data  : %s \n" , key , block.record[i].data);
                return 1 ;
            }  
        }
        BlockNumber++ ;
    }
    printf("The Record with the key : %d was not found \n" , key);
    return 0 ;
}

void insert (FILE *file , int key , const char *data) {
    Block block ;
    int BLockNumber = 0 ;
    while (ReadBlock(file , BLockNumber ,&block)){
        if(block.RecordCount < BLOCK_SIZE) {
            block.record[block.RecordCount].key = key ;
            strcpy(block.record[block.RecordCount].data ,data);
            block.RecordCount++ ;
            WriteBlock(file,BLockNumber , &block);
            printf("Record inserted Successfully : Key = %d , Data : %s \n" , key , data) ;
            return ;
        }
        BLockNumber++ ;
     
    }
    block.RecordCount = 0 ;
    block.record[block.RecordCount].key = key ;
    strcpy(block.record[block.RecordCount].data,data);
    block.RecordCount++ ;
    WriteBlock(file,BLockNumber , &block);
    printf("Record inserted Successfully : Key = %d , Data : %s \n" , key , data) ;
}

void delete (FILE *file , int key) {
    Block block , lastBlock ;
    int BlockNumber =0 ;
    int LastBlockNumber = 0 ;
    rewind(file);

    while(ReadBlock(file , LastBlockNumber , &lastBlock)){
        LastBlockNumber++ ;
    }
    LastBlockNumber-- ;
    while(ReadBlock(file,BlockNumber ,&block)){
        for(int i = 0 ; i < block.RecordCount ; i++){
            if(LastBlockNumber == BlockNumber && lastBlock.RecordCount == 1){
                block.RecordCount-- ;
            }
            else {
                block.record[i] = lastBlock.record[lastBlock.RecordCount -1];
                lastBlock.RecordCount-- ;
                WriteBlock(file,BlockNumber ,&block);
                printf("Record with key = %d is deleted \n" , key);
                return ;
            }
        }
         BlockNumber++ ;
    }
    printf("record with key = %d was not found \n" ,key);
}

void SearchinRange(FILE *file , int Min_key , int Max_key){
    Block block ;
    int BlockNumber = 0 ;
    rewind(file) ;
    printf("Records with keys in the range [%d, %d]:\n", Min_key , Max_key);
    while (ReadBlock(file , BlockNumber , &block)){
        for(int i = 0 ; i < block.RecordCount ; i++){
            if(block.record[i].key > Min_key && block.record[i].key < Max_key){
                printf("key = %d , Data : %s \n" ,block.record[i].key , block.record[i].data);
            }
        }
        BlockNumber++ ;
    }
}

int main(){
    FILE *file = fopen(FILE_NAME , "rb+");
    if (!file){
        file = fopen(FILE_NAME , "wb+");
        printf("Created a new file Named : %s \n" , FILE_NAME);
    }
    insert(file ,10 , "Record A") ;
    insert(file ,20 , "Record B") ;
    insert(file ,30 , "Record C") ;
    insert(file ,40 , "Record D") ;

    Search(file,20);
    delete(file,30);

    SearchinRange(file , 10 , 40) ;

    fclose(file) ;
    return 0 ;
}