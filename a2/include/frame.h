/**
 * Name: Alvi Akbar
 * NSID: ala273
 * 11118887
 */

#ifndef __frame_h__
#define __frame_h__

#include <dbg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _frame
{
  uint16_t index;
  char* message;
  struct _frame* next;
} frame_t;

char*
strdup(const char* str);

frame_t*
Frame_add(const char* message);

void
Frame_delete(frame_t* frame);

int
Frame_serialize(char* buffer, frame_t* frame);

void
Frame_delete_all();

frame_t*
Get_head();

frame_t*
Get_tail();

void
Set_head(frame_t* frame);

void
Set_tail(frame_t* frame);

uint16_t
CRC16(char* buffer, int length);

#endif
