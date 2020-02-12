/**
 * Name: Alvi Akbar
 * NSID: ala273
 * 11118887
 */

#include <frame.h>

frame_t *head = NULL;
frame_t *tail = NULL;

uint16_t
CRC16(char *buffer, int length)
{
  uint8_t x;
  uint16_t crc = 0xFFFF;

  while (length--)
  {
    x = crc >> 8 ^ *buffer++;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^
          ((unsigned short)(x << 5)) ^ ((unsigned short)x);
  }
  return crc;
}

char *
strdup(const char *str)
{
  int length = strlen(str);
  char *copy = (char *)malloc(length + 1);

  memcpy(copy, str, length);
  copy[length] = '\0';

  return copy;
}

frame_t *
Frame_add(const char *message)
{
  static uint32_t index = 0;

  frame_t *frame = (frame_t *)malloc(sizeof(frame_t));

  frame->index = index++;
  frame->message = strdup(message);
  frame->next = NULL;

  if (tail != NULL)
    tail->next = frame;

  tail = frame;

  if (head == NULL)
    head = frame;

  return frame;
}

void Frame_delete(frame_t *frame)
{
  if (frame == NULL)
    return;

  if (head == frame)
    head = frame->next;

  free(frame->message);
  free(frame);
}

void Frame_delete_all()
{
  while (head != NULL)
    Frame_delete(head);
}

int Frame_serialize(char *buffer, frame_t *frame)
{
  if (frame == NULL)
    return 0;

  uint16_t length = strlen(frame->message);
  *(uint16_t *)&buffer[2] = length;

  memcpy(buffer, (const void *)&frame->index, sizeof(frame->index));
  memcpy(&buffer[4], frame->message, length);

  *(uint16_t *)&buffer[4 + length] =
      CRC16(buffer, length + 2 + sizeof(frame->index));

  return length + sizeof(frame->index) + 2 + 2;
}

frame_t *
Get_head()
{
  return head;
}

frame_t *
Get_tail()
{
  return tail;
}

void Set_head(frame_t *frame)
{
  head = frame;
}

void Set_tail(frame_t *frame)
{
  tail = frame;
}
