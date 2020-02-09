#ifndef __FRAME_H__
#define __FRAME_H__

#include <stdint.h>
#include <stdio.h>
#include <dbg.h>
#include <stdlib.h>

typedef struct _frame{
	uint16_t index;
	char* message;
	struct _frame *next;
}frame_t;

char* strdup( const char *str );
frame_t* Frame_add( const char* message );
void Frame_delete( frame_t* frame );
int Frame_serialize( char *buffer, frame_t* frame );
void Frame_delete_all ();
frame_t * get_Head();
frame_t * get_Tail() ;
void set_Head( frame_t * frame);
void set_Tail( frame_t * frame);
uint16_t CRC16(char *buffer, int length);

#endif
