#include <ftplib.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]){
  
  /*
  char* user = argv[1];
  char* host = argv[2];
  char* pass = argv[3];
  */

  char* user = "nao";
  char* host = "157.159.124.129";
  char* pass = "nao";

  netbuf* nControl = NULL;

  if(FtpConnect(host, &nControl) != 1){
    perror("Impossible to connect to the ftp server!\n");
    return EXIT_FAILURE;
  }
  if(FtpLogin(user,pass,nControl) != 1){
    perror("Impossible to log to the ftp server!\n");
    return EXIT_FAILURE;
  }
  FtpDir(NULL,".",nControl);  
  FtpQuit(nControl);
  
  return EXIT_SUCCESS;   
}

