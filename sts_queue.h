#ifndef STS_QUEUE_H
#define STS_QUEUE_H


typedef struct StsHeader StsHeader;

typedef struct {
  StsHeader* (* const create)();
  void (* const destroy)(StsHeader *handle);
  void (* const push)(StsHeader *handle, void *elem);
  void* (* const pop)(StsHeader *handle);
} _StsQueue;

extern _StsQueue const StsQueue;

#endif