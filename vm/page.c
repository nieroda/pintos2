#include <hash.h>
#include "vm/page.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
// on PF look up faulting addr / pull it in
// Rss management on PEXIT ;;

struct sPageTableEntry *getSupPTE (uint32_t *user_vaddr) {
  struct sPageTableEntry *spte = malloc(sizeof (struct sPageTableEntry));
  spte->user_vaddr = pg_round_down(user_vaddr);
  spte->location = 0;
  spte->file = NULL;
  spte->fileoffset = 0;
  spte->diskOffset = 0;
  return spte;
}

unsigned page_hash (const struct hash_elem *elem, void *aux UNUSED) {
  struct sPageTableEntry *p = hash_entry(elem, struct sPageTableEntry, hash_elem);
  return hash_bytes(&p->user_vaddr, sizeof p->user_vaddr);
}

bool page_less (const struct hash_elem *a_, const struct hash_elem *b_, void *aux UNUSED) {
  const struct sPageTableEntry *a = hash_entry (a_, struct sPageTableEntry, hash_elem);
  const struct sPageTableEntry *b = hash_entry (b_, struct sPageTableEntry, hash_elem);
  return a->user_vaddr < b->user_vaddr;
}

struct sPageTableEntry *page_lookup (void *user_vaddr, struct hash *s_pte) {
  struct sPageTableEntry p;
  struct hash_elem *e;
  p.user_vaddr = user_vaddr;
  e = hash_find (s_pte, &p.hash_elem);
  return e != NULL ? hash_entry (e, struct sPageTableEntry, hash_elem) : NULL;
}

// void page_free(struct hash_elem *hash, void *aux UNUSED) {
//   //free entry vaddr entry in hash table from munmap syscall
//
// }
//








// hash_insert (&pages, &p->hash_elem); /* inserting */
// hash_delete (&pages, &p->hash_elem); /* removing */