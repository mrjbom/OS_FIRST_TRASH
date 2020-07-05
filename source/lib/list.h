#ifndef _LIST_H_
#define _LIST_H_

#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stddef.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdint.h"
#include "../../i386-elf-4.9.1-Linux-x86_64/lib/gcc/i386-elf/4.9.1/include/stdbool.h"
#include "../debug/debug.h"

//http://phantomexos.blogspot.com/2013/08/phantomex_6375.html?m=1

//ring double-linked list

typedef struct _list_item_t list_item_t;

//type that implements the list
typedef struct
{
    list_item_t* first; //pointer to the first element
    size_t count; //number of items in the list
   
} list_t;

//type - the list item
struct _list_item_t
{
    list_item_t* prev; //previous element of the list
    list_item_t* next; //next element of the list
    list_t* list; //list that belongs to this element
};

//initializing the list
void list_init(list_t* list);

//adding an item to the list
void list_add(list_t* list, list_item_t* item);

//deleting an item from the list
void list_remove(list_item_t* item);

#endif
