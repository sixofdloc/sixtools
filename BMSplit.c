#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

FILE *fp; //Used when reading/writing files

typedef enum { false, true } bool;

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

char ifn[255]; //Input filename
char ofn[255]; //Output filename
bool pok; //parameters ok
bool sa; //start address flag
int i;
int ifs;
int pi;
int po;
int pa;
int pb;
int pc;
int pn;
ImageFormat ImgTypes[20];
char ImgTypeChr[20] = "0123456789ABCDEFGHIJ";
char FileBuf[65536];
char b;
char c;
int w;
int bn;

int LetToInt(char c)//Converts our parameter letters to the corresponding imgtype
{
	int k = -1;
	for (int j=0;j<=19;j++){
	  if (c==ImgTypeChr[j]) k = j;
	}
	return k;
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

void Uppercase(char *s)
{
	for(int i = 0; s[i]; i++)
    {
       s[i] = toupper(s[i]);
    }
}

bool ProcessParams(int argc, char *argv[])
{
	pi = -1;
	po = -1;
	pa = -1;
	pb = -1;
	pc = -1;
	pn = -1;
	char addrtemp[6];
	if (argc >=1){
		strcpy(ifn,argv[1]);
		for (int j = 2;j<argc;j++) {
			Uppercase(argv[j]);
			if (argv[j][1] == 'I'){
				pi = LetToInt(argv[j][2]);
			}
			//Check for Output type parameter
			if (argv[j][1] == 'O'){
				po = LetToInt(argv[j][2]);
			}
			//Check for .nfo creation parameter
			if (argv[j][1] == 'N'){
				pn = 0;
			}
			//Check for Start Address Parameters
			if (argv[j][1] == 'A'){
				memcpy( addrtemp, &argv[j][4], 4 );
				addrtemp[4] = '\0';
				pa = (int) strtol(addrtemp, (char **)NULL, 16);
			}
			if (argv[j][1] == 'B'){
				memcpy( addrtemp, &argv[j][4], 4 );
				addrtemp[4] = '\0';
				pb = (int) strtol(addrtemp, (char **)NULL, 16);
			}
			if (argv[j][1] == 'C'){
				memcpy( addrtemp, &argv[j][4], 4 );
				addrtemp[4] = '\0';
				pc = (int) strtol(addrtemp, (char **)NULL, 16);
			}
    	}

		if (po==-1) po = 0;
    	if (pi==-1) pi = 0;
    	pok = true;
	} else {
    	pok = false;
	}

	return pok;
}

void ChangeFileExt(char *s, char t[4]) //requires a filename.ext at *s, 3 chars in t, don't fuck around.
{
	char * pch;
	pch=strchr(s,'.');
	if (pch != NULL)
	{
		pch[0] = t[0];
		pch[1] = t[1];
		pch[2] = t[2];
		pch[3] = t[3];
	}
}

void ProcNFO()  //Generate the .nfo file
{
	if (pn == true){
		ChangeFileExt(ofn,"nfo");
		fp = fopen(ofn,"w");
		if (fp == NULL){
			printf("%s","Unable to open output nfo file.\n");
		return;
		}
		fprintf(fp,"%s","Generated by BMSplit\n");
		fprintf(fp,"%s","==========================================================================================\n");
		fprintf(fp,"Input File    : %s",ifn);
		fprintf(fp,"Input Type    : %s",ImgTypes[pi].Name);
		fprintf(fp,"Load Address  : 0x%4X",ImgTypes[pi].LoadAddy);
		fprintf(fp,"Bitmap Offset : 0x%4X",ImgTypes[pi].BmapBase);
		fprintf(fp,"Screen Offset : 0x%4X",ImgTypes[pi].scrnbase);
		fprintf(fp,"Color Offset  : 0x%4X",ImgTypes[pi].colrbase);
		fprintf(fp,"Background    : 0x%2X",FileBuf[ImgTypes[pi].bgrnd]);
		fprintf(fp,"%s","==========================================================================================\n");
		fclose(fp);
	}
}

void SourceWrite()  //printf the appropriate chars to the dest. asm file
{
	printf("\b\b\b\b%4X",i);
	if ((i % 8) == 0) {//This is the {ning of a line
		fprintf(fp,"\tdc.b 0x%2X,",b);
	} else {
		if ((i %8)==7){//This is the end of a line
			fprintf(fp,"%X2",b);
		} else {      //This is in the middle of a line
			fprintf(fp,"%X2,0x",b);
		}
	}
}

void WriteOutRaw()
{
	//-o0 Output *.map, *.scn, *.col in raw byte format
	ChangeFileExt(ofn,".map");
	fp = fopen(ofn,"w");
	if (fp == NULL){
		printf("Unable to open output bitmap file.\n");
		return;
	}
	printf("%s","Writing raw bitmap file:0x0000");
	if (pa > -1) {
		b = pa & 0x00ff;
		fwrite(&b,1,1,fp);
		b = (pa/256) & 0x00ff;
		fwrite(&b,1,1,fp);
	}
	for (int i=0;i<=0x1f3f;i++){
		printf("\b\b\b\b%4X",i);
		b = FileBuf[ImgTypes[pi].BmapBase+i];
		fwrite(&b,1,1,fp);
	}
	fclose(fp);
	printf("...Done!\n");

	ChangeFileExt(ofn,".scn");
	fp = fopen(ofn,"w");
	if (fp == NULL){
		printf("%s\n","Unable to open output screen file.");
		return;
	}
	printf("%s","Writing screen file:0x0000");
	if (pb > -1) {
		b = pb & 0x00ff;
		fwrite(&b,1,1,fp);
		b = (pb/256) & 0x00ff;
		fwrite(&b,1,1,fp);
	}
	for (int i=0;i<=0x03e7;i++){
		printf("\b\b\b\b%4X",i);
		b = FileBuf[ImgTypes[pi].scrnbase+i];
		fwrite(&b,1,1,fp);
	}
	fclose(fp);
	printf("...Done!\n");

	ChangeFileExt(ofn,".col");
	fp = fopen(ofn,"w");
	if (fp == NULL){
		printf("Unable to open output color file.\n");
		return;
	}
	printf("%s","Writing color file:0x0000");
	if (pc > -1) {
		b = pc & 0x00ff;
		fwrite(&b,1,1,fp);
		b = (pc/256) & 0x00ff;
		fwrite(&b,1,1,fp);
	}
	for (int i=0;i<=ImgTypes[pi].colrsize;i++){
		printf("\b\b\b\b%4X",i);
		b = FileBuf[ImgTypes[pi].colrbase+i];
		fwrite(&b,1,1,fp);
	}
	fclose(fp);
	printf("...Done!\n");
	printf("Background color is: 0x%X2",FileBuf[ImgTypes[pi].bgrnd]);
}

void WriteOutDasm(){
//-o1 Output *.map, *.scn, *.col in dasm format
	char tempfilename[255];
	strcpy(tempfilename,"map-");
	ChangeFileExt(ofn,".asm");
	strcat( tempfilename,ofn);
	strcpy(ofn,tempfilename);
	fp = fopen(ofn,"w");
	if (fp == NULL){
		printf("Unable to open output map file.\n");
		return;
	}
	printf("%s","Writing source file for bitmap:0x0000");
	fprintf(fp,"%s\n",";Bitmap Data File Generated by BMSplit");
	if (pa>-1){
		fprintf(fp,"\torg 0x%4X",pa);
	}
	for (int i=0;i<=0x1f3f;i++){
		printf("\b\b\b\b%4X",i);
		b = FileBuf[ImgTypes[pi].BmapBase+i];
		SourceWrite();
	}
	fclose(fp);
	printf("%s\n","...Done!");
	strcpy(tempfilename,"scn-");
	strcpy(ofn,ifn);
	ChangeFileExt(ofn,".asm");
	strcat(tempfilename,ofn);
	strcpy(ofn,tempfilename);
	fp = fopen(ofn,"w");
	if (fp == NULL){
		printf("Unable to open output screen file.\n");
		return;
	}
	printf("%s","Writing source file for screen:0x0000");
	fprintf(fp,"%s\n",";Screen Data File Generated by BMSplit");
	if (pb > -1){
		fprintf(fp,"\torg 0x%4X",pb);
	}
	for (int i= 0; i<=0x03e7;i++){
		printf("\b\b\b\b%4X",i);
		b = FileBuf[ImgTypes[pi].scrnbase+i];
		SourceWrite();
	}
	fclose(fp);
	printf("%s\n","...Done!");
	strcpy(tempfilename,"col-");
	strcpy(ofn,ifn);
	ChangeFileExt(ofn,".asm");
	strcat(tempfilename,ofn);
	strcpy(ofn,tempfilename);
	printf("%s","Writing source file for color:0x0000");
	fprintf(fp,"%s",";Color Data File Generated by BMSplit");
	if (pc >-1){
		fprintf(fp,"\torg 0x%4X",pc);
	}
	for (int i = 0; i<=ImgTypes[pi].colrsize;i++){
		printf("\b\b\b\b%4X",i);
		b = FileBuf[ImgTypes[pi].colrbase+i];
		SourceWrite();
	}
	fclose(fp);
	printf("%s\n","...Done!");
	printf("Background color is: 0x%2X\n",FileBuf[ImgTypes[pi].bgrnd]);
}
void WriteOutSingleFile(){
 	//-o2 Output *.asm in dasm format, single file with comments
	ChangeFileExt(ofn,".map");
	fp = fopen(ofn,"w");
	if (fp == NULL){
		printf("Unable to open output bitmap file.\n");
		return;
	}
	printf("%s","Writing source for bitmap:0x0000");
	fprintf(fp,"%s\n",";File Generated by BMSplit");
	fprintf(fp,";Background color is: 0x%2X\n",FileBuf[ImgTypes[pi].bgrnd]);
	fprintf(fp,"%s\n","");
	fprintf(fp,"%s\n",";Bitmap Data:");
	if (pa >=1 ){
		fprintf(fp,"\torg 0x%4X",pa);
	}
	for (int i = 0;i<=0x1f3f;i++){
		b = FileBuf[ImgTypes[pi].BmapBase+i];
		SourceWrite();
	}
	printf("%s\n","...Done!");
	printf("%s","Writing source for screen:0x0000");
	fprintf(fp,"%s\n","");
	fprintf(fp,"%s\n",";Screen Data:");
	if (pb > -1) {
		fprintf(fp,"\torg 0x%4X",pb);
	}
	for (int i=0;i<=0x03e7;i++){
		b = FileBuf[ImgTypes[pi].scrnbase+i];
		SourceWrite();
	}
	printf("%s\n","...Done!");
	printf("%s","Writing source for color:0x0000");
	fprintf(fp,"%s\n","");
	fprintf(fp,"%s\n",";Color Data:");
	if (pc > -1) {
		fprintf(fp,"\torg 0x%4X",pc);
	}
	for (int i=0;i<= ImgTypes[pi].colrsize;i++){
		b = FileBuf[ImgTypes[pi].colrbase+i];
		SourceWrite();
	}
	fclose(fp);
	printf("%s\n","...Done!");
	printf("Background color is: 0x%2X",FileBuf[ImgTypes[pi].bgrnd]);
}
/*							fp2 = fopen(argv[2],"w");
				if (fp2 == NULL){
					printf("Unable to open output file.\n");
					return 1;
				}
					printf("File load address was :%d, (0x%4X)\n",address,address);
				for (int i=0;i<sz-2;i++){
					fread(&lobyte,1,1,fp);
					fprintf(&lobyte,1,1,fp2);
				}
				*/


int main(int argc, char *argv[]){
	if ( argc < 2 )
   	{
		//Usage Instructions
	    printf("Usage:\n");
	    printf("BMSPLIT infile [-i#] [-o#] [-a0x####] [-b0x####] [-c0x####] [-n]\n");
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
	    printf("-ii Interpaint Multicolor     -ij Run Paint\n");
	    printf("Output (-o) options:\n");
	    printf("-o0 Output *.map, *.scn, *.col in raw byte format\n");
	    printf("-o1 Output *.map, *.scn, *.col in dasm format\n");
	    printf("-o2 Output *.asm in dasm format, single file with comments\n");
	    printf("If start addresses for output files are desired, set them\n");
	    printf("with -a(map), -b(scn), or -c(col).\n");
	    printf("Use -n to force the creation of an .nfo file\n");
    } else {
		printf("\n");
		printf("BMSplit v1.0c/Style (July 2013 by Six/Style)\n");
		printf("\n");
		InitImgTypes();
		printf("Debug.");
		ProcessParams(argc,argv);
		if (pok){
			//Make sure we can open the input file
			fp = fopen(ifn,"r");
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
					char hibyte;
					char lobyte;
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
				strcpy(ofn,ifn);
				printf("%s\n","...Done.");
				switch(po){
					case 0:
						WriteOutRaw();
						break;
					case 1:
						WriteOutDasm();
						break;
					case 2:
						WriteOutSingleFile();
						break;
				}
				ProcNFO();

				//fclose(fp);
				return 0;

			} //file too big
		} // parameters ok
	} //Argument Count
} //Main

