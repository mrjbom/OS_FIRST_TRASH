#include "list.h"

void list_init(list_t* list)
{
    list->first = NULL; //first element is not yet available
    list->count = 0;    //there are no items in the list
}

void list_add(list_t* list, list_item_t* item)
{
    //if the item does not belong to any list
    if (item->list == NULL)
    {
        //if the first item is in the list
        if (list->first)
        {
            //marking an item as belonging to this list
            item->list = list;
            //the next element is the first element
            item->next = list->first;
            //making the previous element that goes before the first one
            item->prev = list->first->prev;
            //our new element is the next one for going before the first one
            item->prev->next = item;
            //and the previous one for the next one
            item->next->prev = item;
        }
        else //if the list is empty, the new item is the first in the list
        {
            item->list = list;  //this item is in this list
            item->next = item;  //he and the next
            item->prev = item;  //and the previous one for yourself
            list->first = item; //and also the first in the list
        }

        list->count++; //increasing the number of list items
    }
}

void list_remove(list_item_t* item)
{
    //if this item is first in the list
    if (item->list->first == item)
    {
        //the first element will be the following
        item->list->first = item->next;
       
        //if the next one is our element again, then
        //he was the only one on the list
        if (item->list->first == item)
        {
            //and now the list is empty
            item->list->first = NULL;
        }
    }

    //previous for this element will be the previous one
    //for the next one
    item->next->prev = item->prev;
    //and the next one after the previous one will be the next one for the item being deleted
    item->prev->next = item->next;
    item->list->count--;
}
