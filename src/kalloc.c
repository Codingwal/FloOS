#include "kalloc.h"
#include "mem.h"
#include "io.h"
#include "error.h"

typedef struct FreePage
{
    struct FreePage *next;
} FreePage;

static FreePage *freePages = NULL;

static void freeRange(void *start, void *end)
{
    char *ptr = (char *)PAGE_ROUND_UP((uint64)start);
    end = (void *)PAGE_ROUND_DOWN((uint64)end);

    for (; ptr < (char *)end; ptr += PAGE_SIZE)
    {
        kfree(ptr);
    }
}

void kallocInit()
{
    freeRange((void *)MEM_START, (void *)(MEM_START + 16 * 1024 * PAGE_SIZE)); // Prepare 16K pages

    // Use this to free almost all available memory (slow!)
    // freeRange((void *)MEM_START, (void *)MEM_END);
}

void *kalloc()
{
    if (freePages == NULL)
        return NULL;

    FreePage *page = freePages;
    freePages = page->next;
    return page;
}

void kfree(void *ptr)
{
    if (((uint64)ptr % PAGE_SIZE) != 0)
        panic("ptr is not page aligned.");

    // Zero out chunk
    // mem_set(ptr, 0, PAGE_SIZE);

    // Insertt page to freePages linked list
    FreePage *page = ptr;
    page->next = freePages;
    freePages = page;
}
