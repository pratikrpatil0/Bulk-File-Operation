#define _GNU_SOURCE
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/syscall.h>


struct linux_dirent {
           unsigned long  d_ino;
           off_t          d_off;
           unsigned short d_reclen;
           char           d_name[];
  };





int generateLog(char* op){
//    char log_file[100];
//    strcpy(log_file,path);
//    strcat(log_file,"_log");

    int fd = 0;
    int drfd = open("Log", O_RDONLY | O_DIRECTORY);
    if (drfd==-1){
        return errno;
    }
    char buf[1024];
    struct linux_dirent *f;
    int nread = syscall(SYS_getdents, drfd, buf, 1024);
    for (int pos = 0;pos<nread;){
        f = (struct linux_dirent *)(buf + pos);
        if (strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0)
                 {
                    pos += f->d_reclen;
                    continue;}  

        char path[100] = "Log/";
        strcat(path,f->d_name);
        if (fd == 0){
            fd = open(path,O_WRONLY|O_APPEND);
            if (fd == -1){
                return errno;
            }
            break;
        }
        else{
            return -1;
        }
    }

    if (fd == 0){
        return -1;
    }

    if (write(fd,op,strlen(op))==-1){
        return errno;
    }else{
        return 0;
    }
}

int isDirectory(char* path){
    struct stat path_stat;
    int status = stat(path, &path_stat);
    if (status == -1){
        return errno;
    }
    return S_ISDIR(path_stat.st_mode);
}

int delete(char* path){
    int status = 0;
    if (isDirectory(path)){
        status = rmdir(path);
        if (status == -1){
            return errno;
        }
    }
    else{
        status = unlink(path);
        if (status == -1){
            return errno;
        }
    }
    int ec = 0;
}

int deleteDirectory(char* dir){


  if (delete(dir)==0){
        return 0;		  	
    }
  struct linux_dirent *f;
  int fd = open(dir, O_RDONLY | O_DIRECTORY);
    if (fd==-1){
        return errno;
    }
    int nread;
    char buf[1024];
    char path[500];
  strcpy(path,dir);
    strcat(path,"/");
  while ((nread = syscall(SYS_getdents, fd, buf, 1024)) > 0){

    for (int pos = 0;pos<nread;) {
            char temp[500];
            f = (struct linux_dirent *)(buf + pos);
            strcpy(temp,path);
            strcat(temp,f->d_name);

            if (strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0)
                 {
                    pos += f->d_reclen;
                    continue;}  
            if (isDirectory(temp)==1){

                deleteDirectory(temp);
            }else{

                delete(temp);
             }
            char msg[500] = "Deleted ";
            strcat(msg,temp);
            strcat(msg,"\n");
            int ec = 0;

            if ((ec =generateLog(msg))>0){
           return ec;
        }
           pos+=f->d_reclen;
        }
    }
    close(fd);
    int ec =delete(dir);
    if (ec ==0){
        char msg[500] = "Deleted ";
        strcat(msg,dir);
        strcat(msg,"\n");
        ec = generateLog(msg);
        if (ec>0){
            return ec;
        }
        return 0;
    }else{
        return ec;
    }
}

int rname(char* old, char* new){
    int status = rename(old, new);
    if (status == -1){
        return errno;
    }
    int ec = 0;
    char msg[500] = "Renamed ";
    strcat(msg,old);
    strcat(msg," to ");
    strcat(msg,new);
    strcat(msg,"\n");
    if(ec = generateLog(msg)>0){
        return ec;
    }

}

int appendText(char* path, char* text){
    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd == -1){
        return errno;
    }
    int size = 0;
    int i = 0;
    char buff[1];
    int status = 0;
    while (text[i] != '\0' && size <= 50 ){
        buff[0] = text[i];
        status = write(fd, buff, 1);
        if (status == -1){
            return errno;
        }
        size = size + status;
        i++;
    }
    int ec = close(fd);
    if (ec == -1){
    return errno;
   } 
    return 0;    
}

int appendNum(char* path, char* num){
    int n = atoi(num);
    if (n < 50 || n > 200){
        return -1;
    }
    if (n % 2 != 0){
        n = n + 1;
    }
    int size = 0;
    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd == -1){
        return errno;
    }
    int status = 0;
    while (size <= 50){
        if (n > 199){
            break;
        }
        char buff[5];
        sprintf(buff, "%d", n);
        status = write(fd, buff, strlen(buff));
        if (status == -1){
            return errno;
        }
        size = size + status;
        n = n + 2;
    }
    int ec = close(fd);
    if (ec == -1){
    return errno;
   } 

    return 0; 
}


int isBinary(char* path){

  int fd = open(path,O_RDONLY);
  char ch[1];
  while (read(fd,ch,1)>0){
    if (ch[0] == '\0' ){
       return 1;
    }
  }
  return 0;
}

int create(char* path, char* flag){
    int n = 0;
    char msg[500] = "Created ";
    char type[25];
    if (strcmp("-f", flag) == 0){
        n = open(path, O_CREAT | O_EXCL, S_IRWXU);
        if (n == -1){
            return errno;
        }
        close(n);
        n = errno;
        strcpy(type,"file ");
    }
    else if(strcmp("-d", flag) == 0){
        n = mkdir(path, S_IRWXU);
        n = errno;
        strcpy(type,"directory ");
    }
    strcat(msg,type);
    strcat(msg,path);
    strcat(msg,"\n");
    int ec = 0;
    if ((ec=generateLog(msg))>0){
        return ec;
    }
    return n;
}


int append(char* dir,char* text,char* num){
    struct linux_dirent *f;
  int fd = open(dir, O_RDONLY | O_DIRECTORY);
    if (fd==-1){
        return errno;
    }
    int nread;
    char buf[1024];
    char path[500];
  strcpy(path,dir);
  strcat(path,"/");
    int bin = 0;
      while ((nread = syscall(SYS_getdents, fd, buf, 1024)) > 0){

    for (int pos = 0;pos<nread;) {
            char temp[500];
            f = (struct linux_dirent *)(buf + pos);
            strcpy(temp,path);
            strcat(temp,f->d_name);

            if (strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0)
                 {
                    pos += f->d_reclen;
                    continue;}  
            if (isDirectory(temp)==1){

                int ec = 0;
                append(temp,text,num);

            }else{

            if (isBinary(temp)){
                bin = 1;
                appendNum(temp,num);
}           else{
                appendText(temp,text);
}
             }
            int ec = 0;
            char msg[500] = "Appended 50 bytes of ";
            strcat(msg,(bin)?"even numbers to ":"text to ");
            strcat(msg,temp);
            strcat(msg,"\n");
            if (ec =generateLog(msg)>0){
             return ec;
        }
           pos+=f->d_reclen;
        }
        return 0;
    }

}

int isDirectoryEmpty() {
     struct linux_dirent *f;
  int fd = open("Log", O_RDONLY | O_DIRECTORY);
    if (fd==-1){
        return errno;
    }
    char buf[1024];
    int is_empty = 1;
    int nread = syscall(SYS_getdents, fd, buf, 1024);
     for (int pos = 0;pos<nread;){
        f = (struct linux_dirent *)(buf + pos);
        if (strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0)
                 {
                    pos += f->d_reclen;
                    continue;}  
        is_empty = 0;
        break;
     }return is_empty;

}

int main(int argc, char* argv[]) {
    if (access("Log",F_OK)== -1)
    {int stat = mkdir("Log",S_IRWXU);
    if (stat == -1){
        return errno;
    }
    }
    char path[500] = "";
    char new_path[500] = "";
    char option[3] = ""; 
    char text[50] = "";
    char num[20] = "";


    if (argc<3){
        return -1;
    }
    if (strlen(argv[1])!=2){
        return -1;
    }
    switch(argv[1][1]){
        case 'a':
            strcpy(path,argv[2]);
            if (argc == 5){
                if(strlen(argv[3])!=2){
                    return -1;
                }
                switch(argv[3][1]){
                    case 'e':
                      strcpy(num,argv[4]);
                      break;
                    case 's':
                       strcpy(text,argv[4]);
                       break;
                    default:
                        return -1;  
                }
            }else if(argc == 7){
                for(int i = 3;i<6;i=i+2){
                    if (argv[i][1]=='e'){
                        strcpy(num,argv[i+1]);
                    }else if(argv[i][1]=='s'){
                        strcpy(text,argv[i+1]);
                    }else{
                        return -1;
                    }
                }
            }else{
                return -1;
            }
            int dir_stat = isDirectory(path);
            if (dir_stat == 1)
            {return append(path,text,num);
            }else if (dir_stat == 0){
                int stat = 0;
                int bin = 0;

                if (isBinary(path)){
                    stat = appendNum(path,num);
                    bin = 1;
                }else{
                    stat = appendText(path,text);  
                }
                if (stat != 0){
                    return stat;
                }
                int ec = 0;
                char msg[500];
                strcat(msg,(bin)?"Appended 50 bytes of text ":"Appendned 50 bytes of even numbers ");
                strcat(msg,path);
                strcat(msg,"\n");
                if ((ec =generateLog(msg))>0){
                    return ec;

            }
            }else{
                return dir_stat;
            }
        case 'd':
            if (argc!=3){
                return -1;
            }
            strcpy(path,argv[2]);
            dir_stat = isDirectory(path);
            if (dir_stat == 1){
                int del_stat = deleteDirectory(path);
                if (del_stat != 0){
                    return del_stat;
                }
            }else if (dir_stat==0){
                int del_stat = delete(path);
                if (del_stat != 0){
                    return del_stat;
                }

            }else{
                return dir_stat;
            }

        case 'r':
            if (argc!=4){
                return -1;
            }
            strcpy(path,argv[2]);
            strcpy(new_path,argv[3]);
            if (access(new_path,F_OK)!= -1){
                return -1;
            }
            return rname(path,new_path);

        case 'c':
            if (argc==4){
                if (strlen(argv[2])!=2){
                    return -1;
                }
                if (argv[2][0]!='-'){
                    return -1;
                }
                if (argv[2][1] == 'f' || argv[2][1] == 'd'){
                    return create(argv[3],argv[2]);
                }else{
                    return -1;
                }
            }else{
                return -1;
            }
        case 'l':
            if (argc!=3){
                return -1;
            }
            if (isDirectoryEmpty()==0){
                return -1;
            }
            strcat(path,"Log/");
            strcat(path,argv[2]);
            int fd = open(path,O_CREAT,S_IRWXU);
            return errno;


        default: 
            return -1;
    }
}
