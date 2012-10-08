/* mem.c : memory manager
 */

#include <xeroskernel.h>

/* Your code goes here */
#include <i386.h>
#define PARAGRAPH_MASK 	(~(0xf))
#define PARAGRAPH_SIZE	0x10
#define SANITY_CHECK	0xff

extern long freemem;

memHeader_t *memSlot;

/*
* kmeminit
*
* @desc:	Initialize the memory manager
*/
void kmeminit(void)
{
	// Set up two free blocks
	// 1. Between freemem+hdr and HOLESTART
	// 2. Between HOLEEND-hdr and MAX_ADDR
	memHeader_t *tmp;
	memSlot = (memHeader_t *) ((freemem + (int)PARAGRAPH_SIZE) & PARAGRAPH_MASK);
	memSlot->size = HOLESTART - (int)(&(memSlot->dataStart));
	memSlot->sanityCheck = SANITY_CHECK;
	memSlot->prev = NULL;
	memSlot->next = (memHeader_t *) HOLEEND;

	memSlot->next->size = (int)0x400000 - HOLEEND - (int)PARAGRAPH_SIZE;
	memSlot->next->sanityCheck = SANITY_CHECK;
	memSlot->next->prev = memSlot;
	memSlot->next->next = NULL;

#ifdef	TEST_MODE
#ifdef 	MEM_TEST
	kmemprint();
#endif
#endif
}

/*
* kmemprint
*
* @desc: 	Debug print memory space
*/
void kmemprint ()
{
	memHeader_t *tmp = memSlot;
	kprintf("\n");
	while(tmp)
	{
		kprintf("mem:%d\t", tmp);
		kprintf("mem->size:%d\n", tmp->size);
		tmp = tmp->next;		
	}
	kprintf("\n");
}


/*
* kmalloc
*
* @desc:	Allocate unallocated memory space and return the start of the free memory space
*
* @param:	size		Amount of memory space to allocate
*/
void *kmalloc(int size)
{
	int amnt;			// Allocate memory amount
	memHeader_t *allocMemSlot;	// Holds the returned memory block
	memHeader_t *tmp;		

	if(size <= 16) return NULL;

	// Calculate allocate bytes
	amnt = (size / (int)PARAGRAPH_SIZE) + ((size % (int)PARAGRAPH_SIZE) ? 1 : 0);
	amnt = amnt * (int)PARAGRAPH_SIZE + sizeof(memHeader_t);	// Append hdr to amnt

	// Find memory slot
	// Cycle through the list of memory blocks until the size+hdr of a memory block is bigger than the calculate amount
	// Otherwise return NULL
	for(allocMemSlot = memSlot; allocMemSlot && (((allocMemSlot->size) + sizeof(memHeader_t)) < amnt); allocMemSlot=allocMemSlot->next);
	if(!allocMemSlot)
		return NULL;		// No sufficient free space!

	// Reorder Memory Blocks
	tmp = (memHeader_t*) ((int)allocMemSlot + amnt);
	tmp->size = allocMemSlot->size - amnt;
	tmp->next = allocMemSlot->next;
	tmp->prev = allocMemSlot->prev;

	if(tmp->prev)
		tmp->prev->next = tmp;			

	if(tmp->next)
		tmp->next->prev = tmp;			

	if(!(tmp->prev)) 
		memSlot = tmp;

	// Set Alloc Memory Block
	allocMemSlot->size = amnt - sizeof(memHeader_t);
	allocMemSlot->sanityCheck = SANITY_CHECK;
	allocMemSlot->next = NULL;
	allocMemSlot->prev = NULL;

#ifdef	TEST_MODE
#ifdef 	MEM_TEST
	kmemprint();
#endif
#endif

	return (int) &(allocMemSlot->dataStart);
}


/*
* kfree
*
* @desc:	Free allocated memory space and coalese to existing unallocated blocks if possible
*
* @param:	ptr		Start of the allocated memory location
*/
void kfree(void *ptr)
{
	if(ptr == NULL) return;
	int memAddr = (int*) ptr, diff; 
	memHeader_t *allocSlot = NULL;
	memHeader_t *tmpMemSlot = memSlot;
	memHeader_t *tmp = NULL;

	// Check for invalid 'dataStart' address,
	// smaller than the first memory block's 'dataStart',
	// in between HOLESTART and HOLEEND+hdr
	// bigger than (max_addr - 16), return
	if(memAddr < (freemem + ((int)PARAGRAPH_SIZE * 2) & PARAGRAPH_MASK) ||
	memAddr > (int) 0x400000 ||
	(memAddr > HOLESTART && memAddr < (HOLEEND + sizeof(memHeader_t))))
		return;
	else
		allocSlot = (memHeader_t *) (memAddr - sizeof(memHeader_t));

	// Sanity check
	if(allocSlot->sanityCheck != SANITY_CHECK) return;

	// Find closiest block to allocated memory block
	// Check the allocated block is not a header block
	// 'diff' is used as a comparison value between the following,
	// FREE_MEMORY_BLOCK_END - ALLOCATED_MEMORY_BLOCK_START
	if(memAddr - sizeof(memHeader_t) >= (int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size)
		diff = memAddr - sizeof(memHeader_t) - (((int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size));

	// Reattach allocated block into memory list
	while(tmpMemSlot) 
	{
		// Reattach allocated block at the start of memory list
		if(memAddr + allocSlot->size <= (int)tmpMemSlot && !(tmpMemSlot->prev))
		{
			allocSlot->next = tmpMemSlot;
			tmpMemSlot->prev = allocSlot;			
			memSlot = allocSlot;
			break;
		}

		// Reattach allocated block at the body/tail of memory list
		// Update the 'diff' value until the smallest diff is found, that is place where the allocated 
		// block will be placed back in
		if((int)tmpMemSlot + tmpMemSlot->size + (sizeof(memHeader_t)*2) <= memAddr &&
		diff >= (memAddr - sizeof(memHeader_t) - ((int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size)) &&
		(memAddr - sizeof(memHeader_t) >= (int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size))	
		{
			diff = memAddr - sizeof(memHeader_t) - (((int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size));
			allocSlot->prev = tmpMemSlot;
		}

		tmpMemSlot = tmpMemSlot->next;			
	}

	if(allocSlot->prev)
	{
		allocSlot->next = allocSlot->prev->next;
		allocSlot->prev->next = allocSlot;
	}

	if(allocSlot->next)
		allocSlot->next->prev = allocSlot;

	// Coalese memory blocks
	tmpMemSlot = memSlot;
	while(tmpMemSlot) 
	{
		tmp = tmpMemSlot->next;
		while(tmp)
		{
			// Check for base+hdr+size is equal the next base
			if(((int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size) == (int)tmp) 
			{
				tmpMemSlot->size = tmpMemSlot->size + tmp->size + sizeof(memHeader_t);
				tmpMemSlot->next = tmp->next;
			}
	
			tmp = tmp->next;		
		}

		tmpMemSlot = tmpMemSlot->next;
	}

#ifdef	TEST_MODE
#ifdef 	MEM_TEST
	kmemprint();
#endif
#endif
}


/*
* kmemcount
*
* @desc:	Gets the size of the head free memory blocks
*/
int kmemcount (void)
{
	memHeader_t *tmp = memSlot;
	return tmp->size;
}




