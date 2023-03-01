#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

struct node
{
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
uint8_t checkPFN;
uint8_t checkBITS;
uint8_t physical_address;
bool testFile;
static FILE *ptr;
char *fileName;

unsigned char memory[SIZE];

char* readLine();

void insertFirst(int address, int free)
{
    struct node *link = (struct node *)malloc(sizeof(struct node));

    link->address = address;
    link->free = free;

    // point it to old first node
    link->next = head;

    // point first to new first node
    head = link;
}

struct node *findFreePage()
{

    // start from the first link
    struct node *current = head;

    // if list is empty
    if (head == NULL)
    {
        return NULL;
    }

    // navigate through list
    while (current->free != 0)
    {

        // if it is last node
        if (current->next == NULL)
        {
            return NULL;
        }
        else
        {
            // go to next link
            current = current->next;
        }
    }

    // if data found, return the current Link
    return current;
}

int main(int argc, char **argv)
{
    for (int i = 0; i < SIZE; i++)
    {
        memory[i] = 255;
    }
    for (int i = (SIZE / PAGE_SIZE) - 1; i >= 0; i--)
    {
        if (i == 0)
        {
            insertFirst(0, 0);
        }
        else
        {
            insertFirst((PAGE_SIZE * i), 0);
        }
    }
    if(argv[1] != NULL){
        openFile(argv[1]);
        testFile = true;
    }
    else{
        testFile = false;
    }
    // printf("Test file? [y/n]: ");
    // fgets(input, sizeof(input), stdin);
    // //char *test = strtok(input, ",");
    // char *test;
    // strcpy(test, input);
    // printf("input%s\n", test);
    // if(!strcmp(test, "y")){
    //     testFile = true;
    //     //strcpy(fileName, test);
    //     //printf("filename%s\n", fileName);
    //     // FILE *fp = fopen(test, "r");
    //     // ptr = fp;
    //     openFile(argv[1]);
    //     if(ptr == NULL){
    //         printf("Error opening file\n");
    //         exit(2);
    //     }
    //     printf("check\n");
    // }
    // else{
    //     testFile = false;
    // }

    while (1)
    {
        printf("Instruction? ");
        if(testFile){
            char* string = readLine();
            strcpy(input, string);
        }
        else{
        fgets(input, sizeof(input), stdin);
        }

        // reads input from command line
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
    if (pageTable == -1)
    {
        startPageTable(process_id);
        pageTable = findPageTable(process_id);
    }
    struct node *freePage = findFreePage();
    if (freePage == NULL)
    {
        printf("Error: no free page\n");
    }
    else
    {
        freePage->free = 1;
        if (freePage->address == 0)
        {
            framenum = 0;
        }
        else
        {
            framenum = (uint8_t)((freePage->address) / PAGE_SIZE);
        }

        uint8_t PTEbits = 0 | ((uint8_t)value);
        uint8_t VPN = (uint8_t)((virtual_address & 48) >> 4);
        uint8_t PFN = framenum;

        for (int i = 2; i < PAGE_SIZE; i = i + 3)
        {
            checkVPN = memory[pageTable + i];
            checkBITS = memory[pageTable + i - 1] & 1;
            if (checkVPN == VPN)
            {
                if (checkBITS == value)
                {
                    printf("Error: virtual page %u is already mapped with rw_bit = %u\n", VPN, checkBITS);
                }
                else
                {
                    uint8_t temp = (uint8_t)(((memory[pageTable + i - 1]) & (~value)) | value);
                    memory[pageTable + i - 1] = temp;
                    printf("Updating permissions for virtual page %d (frame %d)\n", VPN, framenum);
                }
                freePage->free = 0;
                break;
            }
            if (checkVPN == 255)
            {
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

// Stored value 255 at virtual address 7 (physical address 23)

// 		a. args: VA, process-id, value
// 		b. isWritable
// 			i. No - error
// 			ii. Yes -
// 		c. get VPN, offset
// 		d. get physical address
// memory[PA] = value

void store()
{
    int writeable = isWritable(process_id, virtual_address);
    if (writeable == 1)
    {
        int pageTable = findPageTable(process_id);
        uint8_t VPN = (uint8_t)((virtual_address & 48) >> 4);
        for (int i = 2; i < PAGE_SIZE; i = i + 3)
        {
            checkVPN = memory[pageTable + i];
            checkPFN = memory[pageTable + i + 1];
            if (checkVPN == VPN)
            {
                uint8_t offset = (uint8_t)((virtual_address & 15));
                physical_address = (checkPFN << 4) | offset;
                memory[physical_address] = value;
                printf("Stored value %d at virtual address %d (physical address %d)\n", value, virtual_address, physical_address);
            }
        }
    }
}


// load instructs the memory manager to return the byte stored at the memory location specified by
// virtual address. Like the store instruction, it is the memory manager’s responsibility to translate
// and swap pages as needed. Note, the value parameter is not used for this instruction, but a dummy
// value (e.g., 0) should always be provided
void load()
{
    int pageTable = findPageTable(process_id);
    uint8_t VPN = (uint8_t)((virtual_address & 48) >> 4);
    for (int i = 2; i < PAGE_SIZE; i = i + 3)
    {
        checkVPN = memory[pageTable + i];
        checkPFN = memory[pageTable + i + 1];
        if (checkVPN == VPN)
        {
            uint8_t offset = (uint8_t)((virtual_address & 15));
            physical_address = (checkPFN << 4) | offset;
            uint8_t load = memory[physical_address];
            printf("The value %d is stored at virtual address %d (physical address %d)\n", load, virtual_address, physical_address);
        }
    }
}

int isWritable(int PID, int va)
{
    int pageTable = findPageTable(PID);
    uint8_t VPN = (uint8_t)((va & 48) >> 4);

    for (int i = 2; i < PAGE_SIZE; i = i + 3)
    {
        checkVPN = memory[pageTable + i];
        checkBITS = memory[pageTable + i - 1] & 1;
        if (checkVPN == VPN)
        {
            if (checkBITS == 1)
            {
                return 1;
            }
            else
            {
                printf("Error: writes are not allowed to this page\n");
                return 0;
            }
        }
    }
}

void startPageTable(int PID)
{
    struct node *freePage = findFreePage();
    uint8_t framenum = 0;
    if (freePage == NULL)
    {
        printf("Error: no free page\n");
    }
    else
    {
        if (freePage->address == 0)
        {
            framenum = 0;
        }
        else
        {
            framenum = (uint8_t)((freePage->address) / PAGE_SIZE);
        }
        freePage->free = 1;
        memory[freePage->address] = PID;

        printf("Put page table for PID %d into physical frame %d\n", PID, framenum);
    }
}

int findPageTable(int PID)
{
    for (int i = 0; i < (SIZE / PAGE_SIZE); i++)
    {
        if (i == 0)
        {
            page_start = memory[0];
        }
        else
        {
            page_start = memory[i * PAGE_SIZE];
        }
        if (page_start == PID)
        {
            return i * PAGE_SIZE;
        }
    }
    return -1;
}

char* readLine(){
    char *string;
    size_t len = 0;

    //fscanf (ptr, "%s", &string);
    int cnt;
    cnt = getline(&string, &len, ptr);

    if(cnt == EOF){
        printf("End of file, exiting...\n");
        exit(0);
    }

    return string;
}

void openFile(const char* file_name){
    FILE* file = fopen (file_name, "r");
    ptr = file;
}