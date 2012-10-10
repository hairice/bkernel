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
* @desc:	initialize the memory manager
*/
void kmeminit(void)
{
	/*	
	* set up two free blocks (data portion)
	* 1. between freemem+hdr and HOLESTART
	* 2. between HOLEEND-hdr and MAX_ADDR
	*/
	memSlot = (memHeader_t *) ((freemem + (int)PARAGRAPH_SIZE) & PARAGRAPH_MASK);
	memSlot->size = HOLESTART - (int)(&(memSlot->dataStart));
	memSlot->sanityCheck = (char*)SANITY_CHECK;
	memSlot->prev = NULL;
	memSlot->next = (memHeader_t *) HOLEEND;

	memSlot->next->size = (int)0x400000 - HOLEEND - (int)PARAGRAPH_SIZE;
	memSlot->next->sanityCheck = (char*)SANITY_CHECK;
	memSlot->next->prev = memSlot;
	memSlot->next->next = NULL;

#ifdef	MEM_DEBUG
	kmemprint();
#endif
}

/*
* kmalloc
*
* @desc:	allocate unallocated memory space and return the start of the free memory space
*
* @param:	size		amount of memory space to allocate
*
* @output:	dataStart	start address of the data portion for an allocated memory block
*/
void *kmalloc(int size)
{
	int amnt;			/* allocate memory amount */
	memHeader_t *allocMemSlot;	/* holds the returned memory block */
	memHeader_t *tmp;		

	if(size <= 0) return NULL;

	/* calculate allocate bytes */
	amnt = (size / (int)PARAGRAPH_SIZE) + ((size % (int)PARAGRAPH_SIZE) ? 1 : 0);
	amnt = amnt * (int)PARAGRAPH_SIZE + sizeof(memHeader_t);	/* Append hdr to amnt */

	/*
	* find memory slot by cycling through the list of memory blocks until 
	* the size+hdr of a memory block is bigger than the calculated amount
	*/
	for(allocMemSlot = memSlot; allocMemSlot && (((allocMemSlot->size) + sizeof(memHeader_t)) < amnt); allocMemSlot=allocMemSlot->next);
	if(!allocMemSlot)
		return NULL;		/* no sufficient free space! */

	/* reorder Memory Blocks */
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

	/* set allocated memory block */
	allocMemSlot->size = amnt - sizeof(memHeader_t);
	allocMemSlot->sanityCheck = (char*)SANITY_CHECK;
	allocMemSlot->next = NULL;
	allocMemSlot->prev = NULL;

#ifdef	MEM_DEBUG
	kmemprint();
#endif

	return (void*) &(allocMemSlot->dataStart);
}

/*
* kfree
*
* @desc:	free allocated memory space and coalese to existing unallocated blocks if possible
*
* @param:	ptr		start of the allocated memory location
*/
void kfree(void *ptr)
{
	if(ptr == NULL) return;
	int memAddr = (int)((int*)ptr), diff; 
	memHeader_t *allocSlot = NULL;
	memHeader_t *tmpMemSlot = memSlot;
	memHeader_t *tmp = NULL;

	/*
	* check for invalid 'dataStart' address,
	* 1. smaller than the first memory block's 'dataStart',
	* 2. in between HOLESTART and HOLEEND+hdr
	* 3. bigger than (max_addr - 16)
	*/
	if(memAddr < ((freemem + ((int)PARAGRAPH_SIZE * 2)) & PARAGRAPH_MASK) ||
	memAddr > (int) 0x400000 ||
	(memAddr > HOLESTART && memAddr < (HOLEEND + sizeof(memHeader_t))))
		return;
	else
		allocSlot = (memHeader_t *) (memAddr - sizeof(memHeader_t));

	/* sanity check */
	if(allocSlot->sanityCheck != (char*)SANITY_CHECK) return;

	/*
	* find closiest block to allocated memory block where the block is a non-header block,
	* 'diff' is used as a comparison value between the following,
	*
	* 		FREE_MEMORY_BLOCK_END - ALLOCATED_MEMORY_BLOCK_START
	*/
	if(memAddr - sizeof(memHeader_t) >= (int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size)
		diff = memAddr - sizeof(memHeader_t) - (((int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size));

	/* reattach allocated block into memory list */
	while(tmpMemSlot) 
	{
		/* reattach allocated block at the start of memory list */
		if(memAddr + allocSlot->size <= (int)tmpMemSlot && !(tmpMemSlot->prev))
		{
			allocSlot->next = tmpMemSlot;
			tmpMemSlot->prev = allocSlot;			
			memSlot = allocSlot;
			break;
		}

		/*
		* reattach allocated block at the body/tail of memory list
		*
		* the 'diff' value is updated until the smallest diff is found, 
		* that is place where the allocated block will be placed back in
		*/
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

	/* coalese memory blocks */
	tmpMemSlot = memSlot;
	while(tmpMemSlot) 
	{
		tmp = tmpMemSlot->next;
		while(tmp)
		{
			/* check for base+hdr+size is equal to the next base */
			if(((int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size) == (int)tmp) 
			{
				tmpMemSlot->size = tmpMemSlot->size + tmp->size + sizeof(memHeader_t);
				tmpMemSlot->next = tmp->next;
			}
	
			tmp = tmp->next;		
		}

		tmpMemSlot = tmpMemSlot->next;
	}

#ifdef	MEM_DEBUG
	kmemprint();
#endif
}

/*
* kmemprint
*
* @desc: 	debug print memory space
*/
void kmemprint ()
{
	int i=1;
	memHeader_t *tmp = memSlot;

	kprintf("\n");
	while(tmp)
	{
		kprintf("mem[%i]:%d\t", i, tmp);
		kprintf("mem[%i]->size:%d\n", i, tmp->size);
		tmp = tmp->next;		
		i++;
	}
	kprintf("\n");
}

/*
* kmemhdsize
*
* @desc:	gets the size of the head free memory blocks
*
* @output:	size		size of first unallocated memory block
*/
int kmemhdsize (void)
{
	memHeader_t *tmp = memSlot;
	return tmp->size;
}

/*
* kmemtotalsize
*
* @desc:	gets the size of the head free memory blocks
*
* @output:	total_size	total size of unallocated memory space
*/
int kmemtotalsize (void)
{
	int total_size=0;
	memHeader_t *tmp = memSlot;

	while(tmp)
	{
		total_size += tmp->size + sizeof(memHeader_t);
		tmp = tmp->next;		
	}
	return total_size;
}
