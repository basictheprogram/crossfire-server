#ifndef GOD_H
#define GOD_H

typedef struct glnk { /* Used to link together the gods */
  const char *name;       		/* name of this god */
  struct archt *arch;      	/* pointer to the archetype of this god */
  int id;               	/* id of the god */ 
  struct glnk *next;    
} godlink; 

#endif /* GOD_H */
