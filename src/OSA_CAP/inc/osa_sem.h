

#ifndef _OSA_SEM_H_
#define _OSA_SEM_H_

#include <osa.h>

typedef struct {

  Uint32 count;
  Uint32 maxCount;
  pthread_mutex_t lock;
  pthread_cond_t  cond;
} OSA_SemHndl;


// sem
int OSA_semCreate(OSA_SemHndl *hndl, Uint32 maxCount, Uint32 initVal);
int OSA_semWait(OSA_SemHndl *hndl, Uint32 timeout);
int OSA_semSignal(OSA_SemHndl *hndl);
int OSA_semDelete(OSA_SemHndl *hndl);




typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t  cond;
} SELF_SemHndl;

int SELF_semCreate(SELF_SemHndl *hndl);
int SELF_semWait(SELF_SemHndl *hndl, Uint32 timeout);
int SELF_semSignal(SELF_SemHndl *hndl);
int SELF_semDelete(SELF_SemHndl *hndl);


#endif /* _OSA_FLG_H_ */



