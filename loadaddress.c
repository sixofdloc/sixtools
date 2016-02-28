#include <stdio.h>

FILE *fp;

int main(int argc, char *argv[]){
	if ( argc != 2 )
   	{
        	printf( "usage: %s filename", argv[0] );
    	} else {
		fp = fopen(argv[1],"r");
		if (fp == NULL){
			printf("Unable to open file.\n");
			return 1;
		}
		char hibyte;
		char lobyte;
		fread(&lobyte,1,1,fp);
		fread(&hibyte,1,1,fp);
		fclose(fp);
		int address = (hibyte * 256)+lobyte;	
      		printf("File load address is:%d, ($%4X)\n",address,address);
		return 0;
	}
}
