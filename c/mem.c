/* mem.c : memory manager
 */

#include <xeroskernel.h>

/* Your code goes here */
#include <i386.h>
#define PARAGRAPH_MASK (~(0xf))
#define PARAGRAPH_SIZE	0x10

extern long freemem;

memHeader_t *memSlot;

/*
* kmeminit
*
* @desc:
*
* @param:
*/
void kmeminit(void)
{
	memHeader_t *tmp;
	memSlot = (memHeader_t *) ((freemem + (int)PARAGRAPH_SIZE) & PARAGRAPH_MASK);
	memSlot->size = (int)0xA0000 - (int)(&(memSlot->dataStart));
	memSlot->prev = NULL;
	memSlot->next = (memHeader_t *) HOLEEND;

	memSlot->next->size = (int)0x400000 - (int)0x196000 - (int)PARAGRAPH_SIZE;
	memSlot->next->prev = memSlot;
	memSlot->next->next = NULL;
}



/*
* kmalloc
*
* @desc:
*
* @param:
*/
void *kmalloc(int size)
{
	int amnt;
	memHeader_t *allocMemSlot;	// Holds the returned memory block
	memHeader_t *tmp;		

	// Calculate allocate bytes
	amnt = (size / (int)PARAGRAPH_SIZE) + ((size % (int)PARAGRAPH_SIZE) ? 1 : 0);
	amnt = amnt * (int)PARAGRAPH_SIZE + sizeof(memHeader_t);

	// Find memory slot
	for(allocMemSlot = memSlot; allocMemSlot && (((allocMemSlot->size) + sizeof(memHeader_t)) < amnt); allocMemSlot=allocMemSlot->next);
	if(!allocMemSlot)
		return NULL;	// No sufficient free space!

	// Reorder Memory Blocks
	tmp = (memHeader_t*) ((int)allocMemSlot + amnt);
	tmp->size = allocMemSlot->size + sizeof(memHeader_t) - amnt;
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
	allocMemSlot->next = NULL;
	allocMemSlot->prev = NULL;

	return (int) &(allocMemSlot->dataStart);
}


/*
* kfree
*
* @desc:
*
* @param:
*/
void kfree(void *ptr)
{
	int memAddr = (int*) ptr, diff; 
	memHeader_t *allocSlot = NULL;
	memHeader_t *tmpMemSlot = memSlot;
	memHeader_t *tmp = NULL;

	// If address is within the kernel space, bigger than (max_addr - 16), or between the hole, return
	if(memAddr < (freemem + ((int)PARAGRAPH_SIZE * 2) & PARAGRAPH_MASK) ||
	memAddr > (int) 0x400000 ||
	(memAddr > (int) 0xA0000 && memAddr < ((int) 0x196000 + sizeof(memHeader_t))))
		return;
	else
		allocSlot = (memHeader_t *) (memAddr - sizeof(memHeader_t));

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
		if((int)tmpMemSlot + tmpMemSlot->size + (sizeof(memHeader_t)*2) <= memAddr &&
		diff >= (memAddr - sizeof(memHeader_t) - ((int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size)) &&
		(memAddr - sizeof(memHeader_t) >= (int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size))	// Found memory blk
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
			if(((int)tmpMemSlot + sizeof(memHeader_t) + tmpMemSlot->size) == (int)tmp) 
			{
				tmpMemSlot->size = tmpMemSlot->size + tmp->size + sizeof(memHeader_t);
				tmpMemSlot->next = tmp->next;
			}
	
			tmp = tmp->next;		
		}

		tmpMemSlot = tmpMemSlot->next;
	}

/*
	tmp = memSlot;
	kprintf("\n");
	while(tmp)
	{
		kprintf("mem: %d\n", tmp);
		kprintf("mem->size: %d\n", tmp->size);
		tmp = tmp->next;		
	}
	kprintf("\n");
*/
}





