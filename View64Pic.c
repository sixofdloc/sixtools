/*
 ============================================================================
 Name        : View64Pic.c
 Author      : Six/Style
 Version     :
 Copyright   : Fuck off.
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>


typedef enum { false, true } bool;

char koalabitmap[0x1ff0];
char koalascreendata[0x3f7];
char koalacolordata[0x3f7];
char FileBuf[65536];
char bgcolor;

FILE *fp;


SDL_Surface *surface;
bool quitflag;

typedef struct {
	int red;
	int green;
	int blue;
} color;

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

color palette[16];


char ifn[255]; //Input filename
bool pok; //parameters ok
int i;
int ifs;
int pi;
ImageFormat ImgTypes[20];
char ImgTypeChr[20] = "0123456789ABCDEFGHIJ";

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

void initpalette(){
//Uses Pepto's Palette.
//	00 00 00
//	FF FF FF
//	68 37 2B
//	70 A4 B2
//	6F 3D 86
//	58 8D 43
//	35 28 79
//	B8 C7 6F
//	6F 4F 25
//	43 39 00
//	9A 67 59
//	44 44 44
//	6C 6C 6C
//	9A D2 84
//	6C 5E B5
//	95 95 95
	//black
	palette[0].red = 0;
	palette[0].green = 0;
	palette[0].blue = 0;
	//white
	palette[1].red = 0xff;
	palette[1].green = 0xff;
	palette[1].blue = 0xff;
	//red
	palette[2].red = 0x68;
	palette[2].green = 0x37;
	palette[2].blue = 0x2b;
	//cyan
	palette[3].red = 0x70;
	palette[3].green = 0xa4;
	palette[3].blue = 0xb2;
	//purple
	palette[4].red = 0x6f;
	palette[4].green = 0x3d;
	palette[4].blue = 0x86;
	//green
	palette[5].red = 0x58;
	palette[5].green = 0x8d;
	palette[5].blue = 0x43;
	//blue
	palette[6].red = 0x35;
	palette[6].green = 0x28;
	palette[6].blue = 0x79;
	//yellow
	palette[7].red = 0xb8;
	palette[7].green = 0xc7;
	palette[7].blue = 0x6f;
	//orange
	palette[8].red = 0x6f;
	palette[8].green = 0x4f;
	palette[8].blue = 0x25;
	//brown
	palette[9].red = 0x43;
	palette[9].green = 0x39;
	palette[9].blue = 0x00;
	//pink
	palette[10].red = 0x9a;
	palette[10].green = 0x67;
	palette[10].blue = 0x59;
	//grey1
	palette[11].red = 0x44;
	palette[11].green = 0x44;
	palette[11].blue = 0x44;
	//grey2
	palette[12].red = 0x6C;
	palette[12].green = 0x6C;
	palette[12].blue = 0x6C;
	//green2
	palette[13].red = 0x9a;
	palette[13].green = 0xd2;
	palette[13].blue = 0x84;
	//blue2
	palette[14].red = 0x6c;
	palette[14].green = 0x5e;
	palette[14].blue = 0xb5;
	//grey3
	palette[15].red = 0x95;
	palette[15].green = 0x95;
	palette[15].blue = 0x95;

}

void Uppercase(char *s)
{
	for(int i = 0; s[i]; i++)
    {
       s[i] = toupper(s[i]);
    }
}
int LetToInt(char c)//Converts our parameter letters to the corresponding imgtype
{
	int k = -1;
	for (int j=0;j<=19;j++){
	  if (c==ImgTypeChr[j]) k = j;
	}
	return k;
}
bool ProcessParams(int argc, char *argv[])
{
	pi = -1;
	if (argc >=1){
		strcpy(ifn,argv[1]);
		for (int j = 2;j<argc;j++) {
			Uppercase(argv[j]);
			if (argv[j][1] == 'I'){
				pi = LetToInt(argv[j][2]);
			}
    	}
    	if (pi==-1) pi = 0;
    	pok = true;
	} else {
    	pok = false;
	}
	return pok;
}


void putpixel(SDL_Surface *surface,int x, int y, int color){
	//This routine handles all scaling.
	int plotx = x*4;
	int ploty = y*2;
	boxRGBA(surface,plotx,ploty,plotx+3,ploty+1,palette[color].red,palette[color].green,palette[color].blue,255);
}

void renderbitpair(SDL_Surface *surface,int pixelx, int pixely, int bp, int cellindex){
	switch(bp){
		case 0:
			putpixel(surface,pixelx,pixely,bgcolor);
			break;
		case 1:
			putpixel(surface,pixelx,pixely,(koalascreendata[cellindex] & 0xf0) >> 4);
			break;
		case 2:
			putpixel(surface,pixelx,pixely,koalascreendata[cellindex] & 0x0f);
			break;
		case 3:
			putpixel(surface,pixelx,pixely,koalacolordata[cellindex] & 0x0f);
			break;
	}

}

void rendercell(SDL_Surface *surface,int cellindex){
	int bitmapbaseindex = cellindex*8;
	char y;
	int pixelx, pixely;
	y = 0;
	pixely = ((cellindex / 40)*8)+y;
	for (y=0;y<8;y++){
		pixelx = (cellindex % 40)*4;

		char b = koalabitmap[bitmapbaseindex+y];

		char c = (b & 0xc0) >> 6;

		renderbitpair(surface,pixelx,pixely,c,cellindex);
		pixelx++;
		c = (b & 0x30) >> 4;
		renderbitpair(surface,pixelx,pixely,c,cellindex);
		pixelx++;
		c = (b & 0x0c) >> 2;
		renderbitpair(surface,pixelx,pixely,c,cellindex);
		pixelx++;
		c = (b & 0x03);
		renderbitpair(surface,pixelx,pixely,c,cellindex);

		pixely++;

	}
}

void LoadPic(){
	int j = 0;
	char b = 0;
	fp = fopen(ifn,"rb");
	if (fp !=NULL){
		//Discard load address
		fread(&b,1,1,fp);
		fread(&b,1,1,fp);

		for (int j = 0;j<ImgTypes[pi].FileSize;j++){
			fread(&FileBuf[j],1,1,fp);
		}

		for (j = 0;j<0x1f40;j++){
			koalabitmap[j] = FileBuf[ImgTypes[pi].BmapBase+j];
		}
		for (j=0;j<0x03e8;j++){
			koalascreendata[j] = FileBuf[ImgTypes[pi].scrnbase+j];
		}
		if (ImgTypes[pi].colrsize > 1){
			for (j=0;j<0x03e8;j++){
				koalacolordata[j] = FileBuf[ImgTypes[pi].colrbase+j];
				fread(&koalacolordata[i],1,1,fp);
			}
		} else {
			for (j=0;j<0x03e8;j++){
				koalacolordata[j] = FileBuf[ImgTypes[pi].colrbase];
				fread(&koalacolordata[i],1,1,fp);
			}
		}
		fclose(fp);
	}
}


int main(int argc, char *argv[]){
	if ( argc < 2 )
   	{
		//Usage Instructions
	    printf("Usage:\n");
	    printf("View64Pic infile [-i#]\n");
	    printf("Default is -i0 \n");
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
    } else {
		InitImgTypes();
		ProcessParams(argc,argv);
		if (pok){
			quitflag = false;

			initpalette();
			LoadPic();

			SDL_Event event;
			SDL_Init(SDL_INIT_EVERYTHING);
			SDL_WM_SetCaption("View64pic 0.1a", NULL);
			surface = SDL_SetVideoMode(640, 400, 32, SDL_HWSURFACE);
			SDL_ShowCursor(0);
//			SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));
			int color = FileBuff[ImgTypes[pi].bgrnd];
			boxRGBA(surface,0,0,640,400,palette[color].red,palette[color].green,palette[color].blue,255);

			int x = 0;
			for (x=0;x<1000;x++){
				rendercell(surface,x);
			}
			SDL_Flip(surface);
			while (!quitflag){

				if (SDL_PollEvent(&event))
				{
					if (event.type == SDL_QUIT)
					{
						quitflag = true;
					}
					if (event.type == SDL_KEYDOWN)
					{
						SDLKey keyPressed = event.key.keysym.sym;
						switch (keyPressed)
						{
							case SDLK_ESCAPE:
								quitflag=true;
								break;
						}
					}
				}
			}
			SDL_ShowCursor(1);
			SDL_Quit();
		}
    }
	return 0;
}
