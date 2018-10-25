#include <list.h>
#include "vm/frame.h"
#include "vm/page.h"
#include "vm/swap.h"
#include "threads/thread.h"

//CC vs _ inconsistent

static struct list frame_table;
static struct lock frame_table_lock;
// add frame on call to palloc_get_page

struct frame_table_entry {
  uint32_t *frame;
  struct thread *owner;
  struct sup_page_entry *aux;
  struct list_elem elem;
}

void frame_init() {
  list_init(&frame_table);
  lock_init(&frame_table_lock);
}

//palloc_get_page returns KVA
void install_frame(uint32_t* kv_addr, struct sPageTableEntry *entry) {
  struct frame_table_entry *fte = malloc(sizeof(struct frame_table_entry));
  fte->frame = kv_addr;
  fte->owner = thread_current();
  fte->aux   = entry == NULL
                  ? getSupPTE(kv_addr); /* user vaddr vs kvaddr ? */
                  : entry;


  lock_acquire(&frame_table_lock);
  list_push_back(&frame_table, &fte->elem);
  lock_release(&frame_table_lock);
}

void evict_frame() {
  // Plain and simple eviction for now
  // LRU / Clock later
  ASSERT(list_begin(&frame_table) != NULL);

  enum intr_level old_level;
  old_level = intr_disable ();

  struct frame_table_entry *fte = list_entry(list_pop_front(&frame_table), \
                                  struct frame_table_entry, elem);

  ASSERT(fte->owner->pagedir && PTE_P);

  //mark bit as not present in page table
  // check vaddr / kvaddr
  uint32_t *pt = pde_get_pt(fte->owner->pagedir);
  *pt &= ~PTE_P;

  //write frame to swap disk ... save offset in SPTE
  size_t swapDiskOffset = write_to_block(fte->frame);
  fte->aux->location = setLocation(LOC_SWAP, 0);
  fte->aux->diskOffset = swapDiskOffset;

  palloc_free_page(kv_addr);

  intr_set_level(old_level);

  /*
    check if disk ? check if zeroed
  */
  free(fte);
}

void setUpFrame(uint32_t *faultingAddr) {
  // @ rounded down ?

  struct thread *t = thread_current();
  struct sPageTableEntry *sPTE = page_lookup(pg_round_down(faultingAddr), &t->s_pte);

  if (sPTE == NULL)
    PANIC ("Couldn't find sPTE for vaddr.\n");

  evict_frame();

  uint8_t *kpage = palloc_get_page(PAL_USER | PAL_ZERO);

  if (kpage == NULL || !install_page(pg_round_down(faultingAddr), kpage, true)
    PANIC("Kpage null || Couldn't install page.\n");

  if (sPTE->location & LOC_SWAP) {
    read_From_block(kpage, sPTE->diskOffset);
  } else if (sPTE->location & LOC_FILE) {
    //to be implemented
  }

  install_frame(pg_round_down(FaultingAddr), sPTE;)
  // else nothing for us to do
//PF done
}











// @on eviction mark not present
