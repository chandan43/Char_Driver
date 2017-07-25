#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>

#define BUFFER_SIZE 256
static char receive[BUFFER_SIZE];
int main(){
int fd,ret;
char stringTOsend[BUFFER_SIZE];
printf("Starting Testing of char Device driver\n");
fd=open("/dev/Char_Driver", O_RDWR);
if (fd < 0){
      perror("Failed to open the device...");
      return errno;
}
printf("Type in a short string to send to the kernel module:\n");
scanf("%[^\n]%*c", stringTOsend);                // Read in a string (with spaces)
printf("Writing message to the device [%s].\n", stringTOsend);
ret=write(fd,stringTOsend, strlen(stringTOsend));
if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
}
printf("Press ENTER to read back from the device...\n");
getchar();
printf("Reading from the device...\n");
memset(receive,0,sizeof(receive));
ret = read(fd, receive, BUFFER_SIZE);        // Read the response from the LKM
if (ret < 0){
perror("Failed to read the message from the device.");
return errno;
}
printf("The received message is: [%s]\n", receive);
printf("End of the program\n");
return 0; 

}
