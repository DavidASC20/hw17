#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#define SEM_KEY 24602
#define SHM_KEY 24603

union semun {
  int val;                  //used for SETVAL
  struct semid_ds *buf;     //used for IPC_STAT and IPC_SET
  unsigned short  *array;   //used for SETALL
  struct seminfo  *__buf;
};

void create();
void rem();

int main(int argc, char *argv[]) {

  if (argv[1] == NULL) {
    printf("Add -c to CREATE a file, or -r to REMOVE a file\n");
    return 0;
  }

  if (strcmp(argv[1], "-c") == 0) {
    create();
  }
  else if (strcmp(argv[1], "-r") == 0) {
    rem();
  }

  return 0;
}

void create() {
  printf("Creating File\n");

  int semd = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
  if (semd == -1) {
    semd = semget(SEM_KEY, 1, 0);
  }

  union semun us;
  us.val = 1;
  int x = semctl(semd, 0, SETVAL, us);
  printf("semctl Returned %d\n", x);

  int shmd = shmget(SHM_KEY, sizeof(int), IPC_CREAT | IPC_EXCL | 0644);
  if (shmd == -1) {
    shmd = shmget(SHM_KEY, 0, 0);
  }

  int file = open("tmp.txt", O_CREAT | O_EXCL | O_TRUNC, 0644);
  if (file == -1) {
    file = open("tmp.txt", O_RDWR);
  }

  close(file);

  printf("Create Success\n");
}

void rem() {
  printf("Removing File\n");

  int semd = semget(SEM_KEY, 0, 0);
  if (semd == -1) {
    semd = semget(SEM_KEY, 1, 0);
  }

  semctl(semd, IPC_RMID, 0);

  int shmd = shmget(SHM_KEY, 0, 0);
  if (shmd == -1) {
    shmd = shmget(SHM_KEY, 1, 0);
  }

  shmctl(shmd, IPC_RMID, 0);

  int file = open("tmp.txt", O_RDONLY);
  if (file == -1) {
    printf("Failed Opening\n");
    return;
  }

  struct stat sb;
  stat("tmp.txt", &sb);
  char text[sb.st_size];

  read(file, text, sb.st_size);

  printf("File Contents:\n");
  printf("%s\n", text);
}