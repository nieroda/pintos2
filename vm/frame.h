#ifndef VM_FRAME_H
#define VM_FRAME_H

// #include "vm/page.h"

struct frame_table_entry {
  uint32_t *frame;
  struct thread *owner;
  struct sPageTableEntry *aux;
  struct list_elem elem;
};

void frame_init(void);

uint32_t *getFrameToInstall(enum palloc_flags, bool);
void install_frame(uint32_t*, struct sPageTableEntry *);
void evict_frame(void);
void setUpFrame(uint32_t*, bool);


#endif /* vm/frame.h */