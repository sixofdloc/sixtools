#include <stdio.h>

FILE *fp;
FILE *fp2;

int main(int argc, char *argv[]){
	if ( argc != 3 )
   	{
        	printf( "usage: %s infile outfile\n", argv[0] );
    	} else {
		fp = fopen(argv[1],"r");
		if (fp == NULL){
			printf("Unable to open input file.\n");
			return 1;
		}
		fseek(fp, 0L, SEEK_END);
		int sz = ftell(fp);
		if (sz > 65535){
			printf("Too big to be a C64 file.  Fuck off.\n");
			return 1;
		} else {
			fseek(fp,0L,SEEK_SET);
			fp2 = fopen(argv[2],"w");
			if (fp2 == NULL){
				printf("Unable to open output file.\n");
				return 1;
			}
			char hibyte;
			char lobyte;
			fread(&lobyte,1,1,fp);
			fread(&hibyte,1,1,fp);
			int address = (hibyte * 256)+lobyte;	
      			printf("File load address was :%d, ($%4X)\n",address,address);
			for (int i=0;i<sz-2;i++){
				fread(&lobyte,1,1,fp);
				fwrite(&lobyte,1,1,fp2);
			}
			fclose(fp);
			fclose(fp2);
			return 0;
		}
	}
}
