/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

 //https://developer.gnome.org/glib/2.64/glib-Doubly-Linked-Lists.html
 //https://www.opennet.ru/docs/RUS/glib_api/glib-Doubly-Linked-Lists.html

 /*
 From mrjbom:
 I modified this file in order to get rid of the use of "GLib Slice".
 */

#ifndef _G_LIST_H_
#define _G_LIST_H_

typedef struct _GList GList;

struct _GList
{
  void* data;
  GList *next;
  GList *prev;
};

typedef int (*GCompareFunc) (const void* a,
							 const void* b);

typedef int (*GCompareDataFunc) (const void* a,
								 const void* b,
								 void* user_data);

//typedef bool (*GEqualFunc) (const void* a,
//							const void* b);

typedef void (*GDestroyNotify) (void* data);

typedef void (*GFunc) (void* data,
					   void* user_data);

typedef unsigned int (*GHashFunc) (const void* key);

typedef void (*GHFunc) (void* key,
						void* value,
						void* user_data);

/**
 * GCopyFunc:
 * @src: (not nullable): A pointer to the data which should be copied
 * @data: Additional data
 *
 * A function of this signature is used to copy the node data
 * when doing a deep-copy of a tree.
 *
 * Returns: (not nullable): A pointer to the copy
 *
 * Since: 2.4
 */
typedef void* (*GCopyFunc) (const void* src,
							void* data);

/* Doubly linked lists
 */

GList* g_list_alloc(void);

//void g_list_free(GList *list);
//
//void g_list_free_1(GList *list);
//#define g_list_free1 g_list_free_1

//void g_list_free_full(GList            *list,
//					  GDestroyNotify    free_func);

GList* g_list_append(GList *list,
					 void* data);

GList* g_list_prepend(GList *list,
		 			  void* data);
	   
GList* g_list_insert(GList *list,
		 			 void* data,
		 			 int position);
	   
GList* g_list_insert_sorted(GList *list,
							void* data,
							GCompareFunc func);

GList* g_list_insert_sorted_with_data (GList *list,
									   void* data,
									   GCompareDataFunc func,
									   void* user_data);

GList* g_list_insert_before(GList *list,
							GList *sibling,
							void* data);

GList* g_list_insert_before_link(GList *list,
								 GList *sibling,
								 GList *link_);

GList* g_list_concat(GList *list1,
					 GList *list2);

GList* g_list_remove(GList *list,
					 const void* data);

GList* g_list_remove_all(GList *list,
						 const void* data);

GList* g_list_remove_link(GList *list,
						  GList *llink);

GList* g_list_delete_link(GList *list,
						  GList *link_);

GList* g_list_reverse(GList *list) ;

GList* g_list_copy(GList *list);

GList* g_list_copy_deep(GList *list,
						GCopyFunc func,
						void* user_data);


GList* g_list_nth(GList *list,
				  unsigned int n);

GList* g_list_nth_prev(GList *list,
					   unsigned int n);

GList* g_list_find(GList *list,
				   const void* data);

GList* g_list_find_custom(GList *list,
						  const void* data,
						  GCompareFunc func);

int g_list_position(GList *list,
					GList *llink);

int g_list_index(GList *list,
				 const void* data);

GList* g_list_last(GList *list);
	   
GList* g_list_first(GList *list);
	   
unsigned int g_list_length(GList *list);
	   
void g_list_foreach(GList *list,
					GFunc func,
					void* user_data);

GList* g_list_sort(GList *list,
				   GCompareFunc compare_func) ;

GList* g_list_sort_with_data(GList *list,
							 GCompareDataFunc compare_func,
							 void* user_data);

void* g_list_nth_data(GList *list,
					  unsigned int n);

//my function
GList* g_list_free_all(GList* list);                              

#define g_list_previous(list)	        ((list) ? (((GList *)(list))->prev) : NULL)
#define g_list_next(list)	        ((list) ? (((GList *)(list))->next) : NULL)

#endif /* _G_LIST_H_ */
