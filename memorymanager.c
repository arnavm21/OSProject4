#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

struct node {
   int address;
   int free;
   struct node *next;
};

struct node *head = NULL;
struct node *current = NULL;

#define SIZE 64
#define PAGE_SIZE 16
int process_id;
int virtual_address;
int value;
char input[20];
int page_start;
uint8_t checkVPN;
uint8_t checkBITS;

unsigned char memory[SIZE];



void insertFirst(int address, int free) {
   struct node *link = (struct node*) malloc(sizeof(struct node));
	
   link->address = address;
   link->free = free;
	
   //point it to old first node
   link->next = head;
	
   //point first to new first node
   head = link;
}

struct node* findFreePage() {

   //start from the first link
   struct node* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //navigate through list
   while(current->free != 0) {
	
      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
      }
   }      
	
   //if data found, return the current Link
   return current;
}

int main(int argc, char **argv)
{
    for(int i = 0; i < SIZE; i++){
        memory[i] = 255;
    }
    for(int i = (SIZE / PAGE_SIZE) - 1; i >= 0; i--){
        if(i == 0){
            insertFirst(0,0);
        }else{
            insertFirst((PAGE_SIZE * i), 0);
        }
    }

    while (1)
    {
        printf("Instruction? ");
        fgets(input, sizeof(input), stdin);

        //reads input from command line
        char *token = strtok(input, ",");
        process_id = atoi(token);
        token = strtok(NULL, ",");
        char *instruction_type = token;
        token = strtok(NULL, ",");
        virtual_address = atoi(token);
        token = strtok(NULL, ",");
        value = atoi(token);

        if (!strcmp(instruction_type, "map"))
        {
            map();
        }
        else if (!strcmp(instruction_type, "store"))
        {
            store();
        }
        else if (!strcmp(instruction_type, "load"))
        {
            load();
        }
    }
}

void map()
{
    int pageTable = findPageTable(process_id);
    uint8_t framenum = 0;
    if(pageTable == -1){
        startPageTable(process_id);
        pageTable = findPageTable(process_id);
    }
    struct node* freePage = findFreePage();
    if(freePage == NULL){
        printf("Error: no free page\n");
    }
    else{
        freePage->free = 1;
        if(freePage->address == 0){
            framenum = 0;
        } else{
            framenum = (uint8_t) ((freePage->address) / PAGE_SIZE);
        }

        uint8_t PTEbits = 0 | ((uint8_t) value);
        uint8_t VPN = (uint8_t) ((virtual_address & 48) >> 4);
        uint8_t PFN = framenum;

        for(int i = 2; i < PAGE_SIZE; i = i + 3){
            checkVPN = memory[pageTable + i];
            checkBITS = memory[pageTable + i - 1] & 1;
            if(checkVPN == VPN){
                printf("Error: virtual page %u is already mapped with rw_bit = %u\n", VPN, checkBITS);
                freePage->free = 0;
                break;
            }
            if(checkVPN == 255){
                memory[pageTable + i - 1] = PTEbits;
                memory[pageTable + i] = VPN;
                memory[pageTable + i + 1] = PFN;
                printf("Mapped virtual address %d (page %d) into physical frame %d\n", virtual_address, VPN, framenum);
                break;
            }
        }
        
    }
}


// store instructs the memory manager to write the supplied value into the physical memory location
// associated with the provided virtual address, performing translation and page swapping as necessary.
// Note, page swapping is a requirement for part 2 only
void store()
{
    printf("store\n");
}

void load()
{
    printf("load\n");
}



void startPageTable(int PID){
    struct node* freePage = findFreePage();
    uint8_t framenum = 0;
    if(freePage == NULL){
        printf("Error: no free page\n");
    }
    else{
       if(freePage->address == 0){
            framenum = 0;
        } else{
            framenum = (uint8_t) ((freePage->address) / PAGE_SIZE);
        }
        freePage->free = 1;
        memory[freePage->address] = PID;
        
        printf("Put page table for PID %d into physical frame %d\n", PID, framenum);
    }
}

int findPageTable(int PID){
    for(int i = 0; i < (SIZE / PAGE_SIZE); i++){
        if(i == 0){
            page_start = memory[0];
        } else{
            page_start = memory[i*PAGE_SIZE];
        }
        if(page_start == PID){
            return i * PAGE_SIZE;
        }
    }
    return -1;
}