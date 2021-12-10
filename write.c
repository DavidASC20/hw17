#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#define KEY 10000
#define KEY2 10001

int main() {
  int semd;
  semd = semget(KEY2, 0, 0);


  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1;

  semop(semd, &sb, 1);

  int shmd;
  int *data;
  shmd = shmget(KEY, 0, 0);
  data = shmat(shmd, 0, 0);

  int file = open("file.txt", O_RDONLY, 0777);
  if(file != 0){
    printf("Error, Message is: %s", strerror(errno));
  }
  lseek(file, -(*data), SEEK_END);

  char line[*data + 1];
  read(file, line, *data);
  line[*data] = 0;
  printf("Previous line: %s\n", line);
  close(file);

  open("file.txt", O_WRONLY | O_APPEND, 0777);
  printf("Next line: ");

  char buff[1000];
  fgets(buff, 1000, stdin);

  *data = strlen(buff);
  write(file, buff, *data);
  close(file);
  shmdt(data);
  sb.sem_op = 1;
  semop(semd, &sb, 1);

  return 0;
}