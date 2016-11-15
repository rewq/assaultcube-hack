/*
$ gcc -Og -o structloop structloop.c 
$ ./structloop 
*/

#include <stdio.h>
#include <unistd.h>

int n;

int main( ) { 

   n = 80085;
   while(1){
      printf("Integer: %i\n", n);
      //printf("Integer: %p\n", &book);
      sleep(2);
   }

   return 0;
}