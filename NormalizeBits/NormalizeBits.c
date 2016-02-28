#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#include "six_string.h"
#include "six_bool.h"

FILE *fp; //Used when reading/writing files


typedef struct  {
	char Name[50];
	int BmapBase;
	int colrbase;
	int colrsize;
	int scrnbase;
	int bgrnd;
	int border;
	bool hasbg;
	bool hasbdr;
	int LoadAddy;
	int FileSize;
} ImageFormat;

typedef struct {
	int mc1;
	int mc2;
	int scn;
} ScreenCell;

typedef struct {
	ScreenCell cell;
	int score;
} ScoreCell;

char ifn[255]; //Input filename
char ofn[255]; //Output filename
bool pok; //parameters ok
bool sa; //start address flag
int i;
int ifs;
int pi;
int pscl;
int psch;
int pcol;
int pbkg;
ImageFormat ImgTypes[20];
char ImgTypeChr[20] = "0123456789ABCDEFGHIJ";
char FileBuf[65536];
char b;
char c;
int w;
int bn;
bool hasaddress = false;
char hibyte;
char lobyte;

int LetToInt(char c)//Converts our parameter letters to the corresponding imgtype
{
	int k = -1;
	for (int j=0;j<=19;j++){
	  if (c==ImgTypeChr[j]) k = j;
	}
	return k;
}

bool CheckColors(){
	//Check Screen Data
	char lb = 0x00;
	for (int i=0;i<=0x03e7;i++){
	//Check Screen Data
		char b = FileBuf[ImgTypes[pi].scrnbase+i];
		char ub = (b&0xf0)>>4;
		lb = (b&0x0f);
		if (
				(lb!=psch) && (lb!=pscl) && (lb!=pbkg) && (lb!=pcol) &&
				(ub!=psch) && (ub!=pscl) && (ub!=pbkg) && (ub!=pcol)
			) return false;
		//Check Color Data
		b = FileBuf[ImgTypes[pi].colrbase+i];
		lb = (b&0x0f);
		if (
				(lb!=psch) && (lb!=pscl) && (lb!=pbkg) && (lb!=pcol)
			) return false;
	}
	//Check Background Color
	char bg = FileBuf[ImgTypes[pi].bgrnd];
	lb = bg & 0x0f;
	if (
			(lb!=psch) && (lb!=pscl) && (lb!=pbkg) && (lb!=pcol)
		) return false;
	return true;
}

char BitpairForColor(char intendedcolor){
	if (intendedcolor== pbkg) return 0b00;
	if (intendedcolor== psch) return 0b01;
	if (intendedcolor== pscl) return 0b10;
	if (intendedcolor== pcol) return 0b11;
	return 0b00;
}

char WhatColor(char bitpair, char screen, char color, char bground){
	char c = 0x00;
	switch (bitpair){
		case 0b00000000:
			c = bground & 0x0f;
			break;
		case 0b00000001:
			c = (screen & 0xf0) >> 4;
			break;
		case 0b00000010:
			c = screen & 0x0f;
			break;
		case 0b00000011:
			c = color & 0x0f;
			break;
	}
	return c;
}

char NormalizeByte(char b, char scn, char col, char bgc){
	char d = 0x00;
	//Get first bitpair
	char bp = (b & 0xc0)>>6;
	//What color is that pixel in the original image?
	char c = WhatColor(bp,scn,col,bgc);
	//What bitpair represents that color in the normalized image?
	char nbp = BitpairForColor(c);
	//put it in our destination
	d = (d & 0x3f) | (nbp << 6);

	//Get second bitpair
	bp = (b & 0x30)>>4;
	//What color is that pixel in the original image?
	c = WhatColor(bp,scn,col,bgc);
	//What bitpair represents that color in the normalized image?
	nbp = BitpairForColor(c);
	//put it in our destination
	d = (d & 0xcf) | (nbp << 4);

	//Get third bitpair
	bp = (b & 0x0c)>>2;
	//What color is that pixel in the original image?
	c = WhatColor(bp,scn,col,bgc);
	//What bitpair represents that color in the normalized image?
	nbp = BitpairForColor(c);
	//put it in our destination
	d = (d & 0xf3) | (nbp << 2);

	//Get fourth bitpair
	bp = b & 0x03;
	//What color is that pixel in the original image?
	c = WhatColor(bp,scn,col,bgc);
	//What bitpair represents that color in the normalized image?
	nbp = BitpairForColor(c);
	//put it in our destination
	d = (d & 0xfc) | nbp;

	return d;
}

void NormalizeCell(int cellidx){

	//Index into screen data
	int scnindex = ImgTypes[pi].scrnbase+cellidx;
	//Index into color data
	int clrindex = ImgTypes[pi].colrbase+cellidx;

	char scn = FileBuf[scnindex];
	char col = FileBuf[clrindex];
	char bgc = FileBuf[ImgTypes[pi].bgrnd];

	//first byte of cell is bitmapbase + 8 * cellidx
	for (int i = 0;i<8;i++){
		//INdex into bitmap
		int curindex = ImgTypes[pi].BmapBase+(cellidx*8)+i;

		//Fetch bitmap data byte.
		char b = FileBuf[curindex];

		//Normalize this byte
		FileBuf[curindex]= NormalizeByte(b,scn,col,bgc);
	}
}

void NormalizeImage(){
	//00 - background
	//01 - mc1 (scn hi)
	//10 - mc2 (scn lo)
	//11 - color ram low nybble
	char screendest = (psch << 4) + pscl;
	char colordest = pcol | 0xf0;
	char bkdest = pbkg;
	for (int i=0;i<0x3e8;i++){
		NormalizeCell(i);
	}
	for (int i = 0;i<0x3e8;i++){
		FileBuf[ImgTypes[pi].scrnbase+i] = screendest;
		FileBuf[ImgTypes[pi].colrbase+i] = colordest;
	}
	FileBuf[ImgTypes[pi].bgrnd] = bkdest;
}


void InitImgTypes()
{

  //Koala -i0
  strcpy(ImgTypes[0].Name,"Koala");
  ImgTypes[0].BmapBase = 0x0000;
  ImgTypes[0].scrnbase = 0x1f40;
  ImgTypes[0].colrbase = 0x2328;
  ImgTypes[0].colrsize = 0x03e7;
  ImgTypes[0].bgrnd    = 0x2710;
  ImgTypes[0].border   = 0x0000;
  ImgTypes[0].LoadAddy = 0x6000;
  ImgTypes[0].FileSize = 0x2713;
  ImgTypes[0].hasbg    = true;
  ImgTypes[0].hasbdr   = false;

  //Drazpaint -i1
  strcpy(ImgTypes[1].Name,"Drazpaint");
  ImgTypes[1].BmapBase = 0x0800;
  ImgTypes[1].scrnbase = 0x0400;
  ImgTypes[1].colrbase = 0x0000;
  ImgTypes[1].colrsize = 0x03e7;
  ImgTypes[1].bgrnd    = 0x2740;
  ImgTypes[1].border   = 0x0000;
  ImgTypes[1].LoadAddy = 0x5800;
  ImgTypes[1].FileSize = 0x2743;
  ImgTypes[1].hasbg    = true;
  ImgTypes[1].hasbdr   = false;

  //-i2 Paint Magic
  strcpy(ImgTypes[2].Name,"Paint magic");
  ImgTypes[2].BmapBase = 0x0072;
  ImgTypes[2].scrnbase = 0x2072;
  ImgTypes[2].colrbase = 0x1fb5;
  ImgTypes[2].colrsize = 0x0001;
  ImgTypes[2].bgrnd    = 0x1fb2;
  ImgTypes[2].border   = 0x1fb6;
  ImgTypes[2].LoadAddy = 0x38fe;
  ImgTypes[2].FileSize = 0x2474;
  ImgTypes[2].hasbg    = true;
  ImgTypes[2].hasbdr   = true;

  //-i3 Picasso64
  strcpy(ImgTypes[3].Name,"Picasso64");
  ImgTypes[3].BmapBase = 0x0800;
  ImgTypes[3].scrnbase = 0x0400;
  ImgTypes[3].colrbase = 0x0000;
  ImgTypes[3].colrsize = 0x03e7;
  ImgTypes[3].bgrnd    = 0x07ff;
  ImgTypes[3].border   = 0x0000;
  ImgTypes[3].LoadAddy = 0x1800;
  ImgTypes[3].FileSize = 0x2742;
  ImgTypes[3].hasbg    = true;
  ImgTypes[3].hasbdr   = false;

  //-i4 Rainbow Painter
  strcpy(ImgTypes[4].Name,"Rainbow Painter");
  ImgTypes[4].BmapBase = 0x0400;
  ImgTypes[4].scrnbase = 0x0000;
  ImgTypes[4].colrbase = 0x2400;
  ImgTypes[4].colrsize = 0x03e7;
  ImgTypes[4].bgrnd    = 0x23ff;
  ImgTypes[4].border   = 0x0000;
  ImgTypes[4].LoadAddy = 0x1800;
  ImgTypes[4].FileSize = 0x273a;
  ImgTypes[4].hasbg    = true;
  ImgTypes[4].hasbdr   = false;

  //-i5 Advanced Art Studio
  strcpy(ImgTypes[5].Name,"Advanced Art Studio");
  ImgTypes[5].BmapBase = 0x0000;
  ImgTypes[5].scrnbase = 0x1f40;
  ImgTypes[5].colrbase = 0x2338;
  ImgTypes[5].colrsize = 0x03e7;
  ImgTypes[5].bgrnd    = 0x2329;
  ImgTypes[5].border   = 0x0000;
  ImgTypes[5].LoadAddy = 0x2000;
  ImgTypes[5].FileSize = 0x2722;
  ImgTypes[5].hasbg    = true;
  ImgTypes[5].hasbdr   = false;

  //-i6 Saracen Paint
  strcpy(ImgTypes[6].Name,"Saracen Paint");
  ImgTypes[6].BmapBase = 0x0400;
  ImgTypes[6].scrnbase = 0x0000;
  ImgTypes[6].colrbase = 0x2400;
  ImgTypes[6].colrsize = 0x03e7;
  ImgTypes[6].bgrnd    = 0x03f0;
  ImgTypes[6].border   = 0x0000;
  ImgTypes[6].LoadAddy = 0x7800;
  ImgTypes[6].FileSize = 0x2722;
  ImgTypes[6].hasbg    = true;
  ImgTypes[6].hasbdr   = false;

  //-i7 Blazing Paddles
  strcpy(ImgTypes[7].Name,"Blazing Paddles");
  ImgTypes[7].BmapBase = 0x0000;
  ImgTypes[7].scrnbase = 0x2000;
  ImgTypes[7].colrbase = 0x2400;
  ImgTypes[7].colrsize = 0x03e7;
  ImgTypes[7].bgrnd    = 0x1f80;
  ImgTypes[7].border   = 0x0000;
  ImgTypes[7].LoadAddy = 0xa000;
  ImgTypes[7].FileSize = 0x2802;
  ImgTypes[7].hasbg    = true;
  ImgTypes[7].hasbdr   = false;

  //-i8 Micro Illustrator
  strcpy(ImgTypes[8].Name,"Micro Illustrator");
  ImgTypes[8].BmapBase = 0x07e4;
  ImgTypes[8].scrnbase = 0x0014;
  ImgTypes[8].colrbase = 0x13fc;
  ImgTypes[8].colrsize = 0x03e7;
  ImgTypes[8].bgrnd    = 0x0000;
  ImgTypes[8].border   = 0x0000;
  ImgTypes[8].LoadAddy = 0x18DC;
  ImgTypes[8].FileSize = 0x2726;
  ImgTypes[8].hasbg    = true;
  ImgTypes[8].hasbdr   = false;

  //-i9 CDU-Paint
  strcpy(ImgTypes[9].Name,"CDU-Paint");
  ImgTypes[9].BmapBase = 0x0111;
  ImgTypes[9].scrnbase = 0x2051;
  ImgTypes[9].colrbase = 0x2439;
  ImgTypes[9].colrsize = 0x03e7;
  ImgTypes[9].bgrnd    = 0x2821;
  ImgTypes[9].border   = 0x0000;
  ImgTypes[9].LoadAddy = 0x7eef;
  ImgTypes[9].FileSize = 0x2825;
  ImgTypes[9].hasbg    = true;
  ImgTypes[9].hasbdr   = false;

  //-ia Artist 64
  strcpy(ImgTypes[10].Name,"Artist 64");
  ImgTypes[10].BmapBase = 0x0000;
  ImgTypes[10].scrnbase = 0x2000;
  ImgTypes[10].colrbase = 0x2400;
  ImgTypes[10].colrsize = 0x03e7;
  ImgTypes[10].bgrnd    = 0x27ff;
  ImgTypes[10].border   = 0x0000;
  ImgTypes[10].LoadAddy = 0x4000;
  ImgTypes[10].FileSize = 0x2742;
  ImgTypes[10].hasbg    = true;
  ImgTypes[10].hasbdr   = false;

  //-ib Vidcom 64
  strcpy(ImgTypes[11].Name,"Vidcom 64");
  ImgTypes[11].BmapBase = 0x0800;
  ImgTypes[11].scrnbase = 0x0400;
  ImgTypes[11].colrbase = 0x0000;
  ImgTypes[11].colrsize = 0x03e7;
  ImgTypes[11].bgrnd    = 0x07e8;
  ImgTypes[11].border   = 0x0000;
  ImgTypes[11].LoadAddy = 0x5800;
  ImgTypes[11].FileSize = 0x2742;
  ImgTypes[11].hasbg    = true;
  ImgTypes[11].hasbdr   = false;

  //-ic Image System (multicolor)
  strcpy(ImgTypes[12].Name,"Image System");
  ImgTypes[12].BmapBase = 0x0400;
  ImgTypes[12].scrnbase = 0x2400;
  ImgTypes[12].colrbase = 0x0000;
  ImgTypes[12].colrsize = 0x03e7;
  ImgTypes[12].bgrnd    = 0x23ff;
  ImgTypes[12].border   = 0x0000;
  ImgTypes[12].LoadAddy = 0x3C00;
  ImgTypes[12].FileSize = 0x27ea;
  ImgTypes[12].hasbg    = true;
  ImgTypes[12].hasbdr   = false;

  //-id Face Painter
  strcpy(ImgTypes[13].Name,"Face Painter");
  ImgTypes[13].BmapBase = 0x0000;
  ImgTypes[13].scrnbase = 0x1f40;
  ImgTypes[13].colrbase = 0x2328;
  ImgTypes[13].colrsize = 0x03e7;
  ImgTypes[13].bgrnd    = 0x2712;
  ImgTypes[13].border   = 0x0000;
  ImgTypes[13].LoadAddy = 0x4000;
  ImgTypes[13].FileSize = 0x2714;
  ImgTypes[13].hasbg    = true;
  ImgTypes[13].hasbdr   = false;

  //-ie Cheese
  strcpy(ImgTypes[14].Name,"Cheese");
  ImgTypes[14].BmapBase = 0x0000;
  ImgTypes[14].scrnbase = 0x4200;
  ImgTypes[14].colrbase = 0x4800;
  ImgTypes[14].colrsize = 0x03e7;
  ImgTypes[14].bgrnd    = 0x0000;
  ImgTypes[14].border   = 0x0000;
  ImgTypes[14].LoadAddy = 0x8000;
  ImgTypes[14].FileSize = 0x5002;
  ImgTypes[14].hasbg    = true;
  ImgTypes[14].hasbdr   = false;

  //-if Create With Garfield
  strcpy(ImgTypes[15].Name,"Create With Garfield");
  ImgTypes[15].BmapBase = 0x0000;
  ImgTypes[15].scrnbase = 0x1f40;
  ImgTypes[15].colrbase = 0x2328;
  ImgTypes[15].colrsize = 0x03e7;
  ImgTypes[15].bgrnd    = 0x2711;
  ImgTypes[15].border   = 0x0000;
  ImgTypes[15].LoadAddy = 0x8000;
  ImgTypes[15].FileSize = 0x2717;
  ImgTypes[15].hasbg    = true;
  ImgTypes[15].hasbdr   = false;

  //-ig Dolphin Ed
  strcpy(ImgTypes[16].Name,"Dolphin Ed");
  ImgTypes[16].BmapBase = 0x0800;
  ImgTypes[16].scrnbase = 0x0400;
  ImgTypes[16].colrbase = 0x0000;
  ImgTypes[16].colrsize = 0x03e7;
  ImgTypes[16].bgrnd    = 0x07e8;
  ImgTypes[16].border   = 0x0000;
  ImgTypes[16].LoadAddy = 0x5800;
  ImgTypes[16].FileSize = 0x2802;
  ImgTypes[16].hasbg    = true;
  ImgTypes[16].hasbdr   = false;

  //-ih Gigapaint Multicolor
  strcpy(ImgTypes[17].Name,"Gigapaint Multicolor");
  ImgTypes[17].BmapBase = 0x0000;
  ImgTypes[17].scrnbase = 0x1f40;
  ImgTypes[17].colrbase = 0x2328;
  ImgTypes[17].colrsize = 0x03e7;
  ImgTypes[17].bgrnd    = 0x2710;
  ImgTypes[17].border   = 0x0000;
  ImgTypes[17].LoadAddy = 0x6000;
  ImgTypes[17].FileSize = 0x2713;
  ImgTypes[17].hasbg    = true;
  ImgTypes[17].hasbdr   = false;

  //-ii Interpaint Multicolor
  strcpy(ImgTypes[18].Name,"Interpaint Multicolor");
  ImgTypes[18].BmapBase = 0x0000;
  ImgTypes[18].scrnbase = 0x1f40;
  ImgTypes[18].colrbase = 0x2328;
  ImgTypes[18].colrsize = 0x03e7;
  ImgTypes[18].bgrnd    = 0x2711;
  ImgTypes[18].border   = 0x0000;
  ImgTypes[18].LoadAddy = 0x4000;
  ImgTypes[18].FileSize = 0x2713;
  ImgTypes[18].hasbg    = true;
  ImgTypes[18].hasbdr   = false;

  //-ij Run Paint
  strcpy(ImgTypes[19].Name,"Run Paint");
  ImgTypes[19].BmapBase = 0x0000;
  ImgTypes[19].scrnbase = 0x1f40;
  ImgTypes[19].colrbase = 0x2328;
  ImgTypes[19].colrsize = 0x03e7;
  ImgTypes[19].bgrnd    = 0x2710;
  ImgTypes[19].border   = 0x0000;
  ImgTypes[19].LoadAddy = 0x6000;
  ImgTypes[19].FileSize = 0x2716;
  ImgTypes[19].hasbg    = true;
  ImgTypes[19].hasbdr   = false;
}


bool ProcessParams(int argc, char *argv[])
{
	pi = -1;
	pscl = -1;
	psch = -1;
	pcol = -1;
	pbkg = -1;
	//char addrtemp[6];
	if (argc >=1){
//	    NORMBITS infile outfile -l=# -h=# -c=# -b=# [-i#]
		strcpy(ifn,argv[1]);
		strcpy(ofn,argv[2]);
		for (int j = 3;j<argc;j++) {
			Uppercase(argv[j]);
			if (argv[j][1] == 'I'){
				pi = LetToInt(argv[j][2]);
			}
			//Check for Screen Low parameter
			if (argv[j][1] == 'L'){
				pscl = atoi(&argv[j][2]);
			}
			//Check for Screen Low parameter
			if (argv[j][1] == 'H'){
				psch = atoi(&argv[j][2]);
			}
			//Check for Screen Low parameter
			if (argv[j][1] == 'C'){
				pcol = atoi(&argv[j][2]);
			}
			//Check for Screen Low parameter
			if (argv[j][1] == 'B'){
				pbkg = atoi(&argv[j][2]);
			}
    	}

    	if (pi==-1) pi = 0;
    	pok = true;
	} else {
    	pok = false;
	}

	return pok;
}




int main(int argc, char *argv[]){
	if ( argc < 2 )
   	{
		//Usage Instructions
	    printf("Usage:\n");
	    printf("NORMBITS infile outfile -l# -h# -c# -b# [-i#]\n");
	    printf("Default is -i0 -o0\n");
	    printf("Input (-i) options:\n");
	    printf("-i0 Koala                     -i1 Drazpaint\n");
	    printf("-i2 Paint Magic               -i3 Picasso64\n");
	    printf("-i4 Rainbow Painter           -i5 Advanced Art Studio\n");
	    printf("-i6 Saracen Paint             -i7 Blazing Paddles\n");
	    printf("-i8 Micro Illustrator         -i9 CDU-Paint\n");
	    printf("-ia Artist 64                 -ib Vidcom 64\n");
	    printf("-ic Image System (multicolor) -id Face Painter\n");
	    printf("-ie Cheese                    -if Create With Garfield\n");
	    printf("-ig Dolphin Ed                -ih Gigapaint Multicolor\n");
	    printf("-ii Interpaint Multicolor     -ij Run Paint\n\n");
	    printf("-b parameter sets background color (bitpair 00)");
	    printf("-h parameter sets high-nybble of screen-ram data (bitpair 01)");
	    printf("-l parameter sets low-nybble of screen-ram data (bitpair 10)");
	    printf("-c parameter sets color-ram data (bitpair 11)");
    } else {
		printf("\n");
		printf("NORMBITS v1.0a/Style (July 2013 by Six/Style)\n");
		printf("\n");
		InitImgTypes();
		ProcessParams(argc,argv);
		if (pok){
			//Make sure we can open the input file
			fp = fopen(ifn,"rb");
			if (fp == NULL){
				printf("Unable to open input file.\n");
				return 1;
			}
			//Make sure it"s not too big to be a C64 file
			fseek(fp, 0L, SEEK_END);
			int sz = ftell(fp);
			if (sz > 65535){
				printf("Too big to be a C64 file.\n");
				return 1;
			} else {
				fseek(fp,0L,SEEK_SET);
				printf("Input file is %s\n",ImgTypes[pi].Name);
				printf("Input file size is 0x%4X bytes\n",sz);
				sa = true;
				if (ifs != ImgTypes[pi].FileSize) {
					if (ImgTypes[pi].FileSize-ifs == 2) {
						printf("%s\n","Filesize is two bytes smaller than expected, assuming no start address.");
						sa = false;
					} else {
						printf("Filesize wrong! Expected 0x%4X, found 0x%4X bytes.\n",ImgTypes[pi].FileSize,sz);
					}
				}
				if (sa==true){
					printf("%s","Checking for start address...");
					//Determine if Input File has start address
					fread(&lobyte,1,1,fp);
					fread(&hibyte,1,1,fp);
					int address = (hibyte * 256)+lobyte;
					fseek(fp,0L,SEEK_SET);
					if (address == ImgTypes[pi].LoadAddy)
					{
						printf("%s\n","found...discarding.");
						//discard load address
						fseek(fp,2L,SEEK_SET);
					} else {
						printf("%s\n","not found.");
					}
				}
				//Read Input File into Buffer
				printf("%s","Reading file into buffer:0x0000");
				for (int i = 0;i<ImgTypes[pi].FileSize;i++){
					printf("\b\b\b\b%4X",i);
					fread(&FileBuf[i],1,1,fp);
				}
				fclose(fp);
				//strcpy(ofn,ifn);
				printf("%s\n","...Done.");

				printf("%s","Checking colors...");
				if (CheckColors()){
					printf("%s\n","good.");
					NormalizeImage();
					fp = fopen(ofn,"wb");
					if (fp == NULL){
						printf("Unable to open output file.\n");
						return 1;
					}
					printf("%s","Writing output file:0x0000");
					if (sa==true){
						fwrite(&lobyte,1,1,fp);
						fwrite(&hibyte,1,1,fp);
					}
					for (int i=0;i<ImgTypes[pi].FileSize;i++){
						printf("\b\b\b\b%4X",i);
						b = FileBuf[i];
						fwrite(&b,1,1,fp);
					}
					fclose(fp);
					printf("...Done!\n");

				} else {
					printf("%s\n","unexpected color data found.");
					return 1;
				}


				//fclose(fp);
				return 0;

			} //file too big
		} // parameters ok
	} //Argument Count
} //Main

