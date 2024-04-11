char *icon_goback;
char *icon_directory;
char *icon_exec_file;
char *icon_file;

void init_icons(void)
{
	icon_goback="\
WWWWWWWWWWWWWWWW\
WggggggggggggggW\
WgggggggWWgggggW\
WggggggWWggggggW\
WgggggWWgggggggW\
WggggWWggggggggW\
WgggWWgggggggggW\
WggWWWWWWWWWWggW\
WggWWWWWWWWWWggW\
WgggWWgggggggggW\
WggggWWggggggggW\
WgggggWWgggggggW\
WggggggWWggggggW\
WgggggggWWgggggW\
WggggggggggggggW\
WWWWWWWWWWWWWWWW";
	icon_directory="\
     BBBBBBBBBBB\
    BBbbbbbbbbBB\
   BbBbbbbbbbBYB\
  BbbBbbbbbbBYYB\
 BbbbBbbbbbBYYYB\
BBBBBBBBBBBYYYYB\
BYYYYYYYYYBYYYYB\
BYYYYYYYYYBYYYYB\
BYYYYYYYYYBYYYYB\
BYYYYYYYYYBYYYYB\
BYYYYYYYYYBYYYYB\
BYYYYYYYYYBYYYB \
BYYYYYYYYYBYYB  \
BYYYYYYYYYBYB   \
BYYYYYYYYYBB    \
BBBBBBBBBBB     ";
	icon_exec_file="\
BBBBBBBBBBBBBBBB\
BWWWWWWWWWWWWWWB\
BWBBBBBBBBBBBBWB\
BWBGBGBGGBGBGBWB\
BWBBBBBBBBBBBBWB\
BWBGBGBGGBGBGBWB\
BWBBBBBBBBBBBBWB\
BWBGBGBGGBGBGBWB\
BWBGBGBGGBGBGBWB\
BWBBBBBBBBBBBBWB\
BWBGBGBGGBGBGBWB\
BWBBBBBBBBBBBBWB\
BWBGBGBGGBGBGBWB\
BWBBBBBBBBBBBBWB\
BWWWWWWWWWWWWWWB\
BBBBBBBBBBBBBBBB";
	icon_file="\
     BBBBBBBBBBB\
    BBWWWWWWWWWB\
   BWBWWWWWWWWWB\
  BWWBWWWWWWWWWB\
 BWWWBgWgWgWgWWB\
BBBBBBWgWgWgWgWB\
BWWWWWWWWWWWWWWB\
BWWWWWWWWWWWWWWB\
BWgWgWgWgWgWgWWB\
BWWgWgWgWgWgWgWB\
BWWWWWWWWWWWWWWB\
BWWWWWWWWWWWWWWB\
BWgWgWgWgWgWgWWB\
BWWgWgWgWgWgWgWB\
BWWWWWWWWWWWWWWB\
BBBBBBBBBBBBBBBB";

	
}

void icon_paint(char *icon,int x,int y)
{
	int x1,y1;
	unsigned int color;
	y1=0;
	while(y1<16)
	{
		x1=0;
		while(x1<16)
		{
			color=0x000000;
			if(*icon=='W')
			{
				color=0xffffff;
			}
			else if(*icon=='g')
			{
				color=0x404040;
			}
			else if(*icon=='Y')
			{
				color=0xffff00;
			}
			else if(*icon=='b')
			{
				color=0x808000;
			}
			else if(*icon=='G')
			{
				color=0x00ff00;
			}
			if(*icon!=' '&&x+x1>=0&&x+x1<WINW&&y+y1>=0&&y+y1<WINH)
			{
				pbuf[(y+y1)*WINW+x+x1]=color;
			}
			++icon;
			++x1;
		}
		++y1;
	}
}
