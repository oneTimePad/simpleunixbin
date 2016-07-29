
#define _GNU_SOURCE 1
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "colors.h"

#define CURR_DIRECTORY "PWD"
#define MAX_FULL_PATH 512
typedef enum {FALSE, TRUE} _BOOL;

int main(int argc, char* argv[]){



    char* file;


    if(argc<2) //if no file is specified assume cwd
      file = secure_getenv(CURR_DIRECTORY);
    else
      file = argv[1];

    DIR* dir;

    if((dir=opendir(file))==(DIR*)NULL){
      perror("opendir()");
      exit(EXIT_FAILURE);
    }

    _BOOL show_hidden = FALSE;
    struct dirent *entry;

    while((entry=readdir(dir))!=NULL){ //traverse directory
        //check if we can show hidden files
        if(!show_hidden)
          if(entry->d_name[0]=='.')
            continue;

        //compute the absolute path name of the file
        size_t dir_len = strlen(file);
        size_t file_len = strlen(entry->d_name);
        size_t total_len = dir_len+file_len;

        if(total_len+1>=MAX_FULL_PATH-2){
            fprintf(stderr,"file name too long\n");
            exit(EXIT_FAILURE);
        }

        char full_path[MAX_FULL_PATH];
        strncpy(full_path,file,dir_len);
        full_path[dir_len]=0x0;
        if(file[dir_len-1]!='/')
          strncat(full_path,"/",(size_t)1);
        strncat(full_path,entry->d_name,file_len);

        full_path[((file[dir_len]=='/')? total_len : total_len+1)] = 0x0;

        //get file stats
        struct stat file_stat;
        if(stat(full_path,&file_stat)==-1){
          perror("stat()");
          exit(EXIT_FAILURE);
        }

        mode_t file_mode = file_stat.st_mode;

        int cforeground=WHITE;
        int cbackground=-1;
        //check if not regular file
        if(!S_ISREG(file_mode))
          cforeground= BLUE; //directories are blue

        //if executable make green
        if(file_mode&S_IXUSR | file_mode&S_IXGRP | file_mode&S_IXOTH){
            if(cforeground!=BLUE)
              cforeground=GREEN;

        }

        cprintf(BRIGHT,cforeground,cbackground,"%s\n",entry->d_name);

    }

    printf("\n");
    if(errno!=0){
      perror("readdir_r()");
      exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
