Jake Leavitt and Arnav Mishra

Project 4: Memory Manager

Program simulates memory with paging and swapping

Map: finds page table for given process id. If page table does not exist, one is created in a free frame. Next free frame is found
(if no free frame exists, pages are swapped from memory to disk). Then, page table is searched through to find matching Virtual Page
Number if page is already mapped or creates a new page table entry for the given virtual address. Bits are updated and physical frame
number is set. 

Store: If the given address is writeable (checks r/w bit for given virtual address in page table),  finds the physical address using 
the page table and the offset from the virtual address, and stores the given value at that physical address

Load: Finds the page table for the given virtual address, then finds the page table entry for the virtual address, translates it
to the physical address using the offset and the PFN, and reads the value stored at the physical address
