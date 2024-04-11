
void w4(unsigned int *buf,unsigned int val,unsigned long n)
{
	while (n)
	{
		*buf = val;
		++buf;
		--n;
	}
}
void rect(unsigned int *dst,int dstw,int dsth,int rect_x,int rect_y,int rect_w,int rect_h,unsigned int color)
{
	int off;
	if(rect_x<0)
	{
		rect_w+=rect_x;
		rect_x=0;
	}
	if(rect_y<0)
	{
		rect_h+=rect_y;
		rect_y=0;
	}
	if(rect_x+rect_w>dstw)
	{
		rect_w=dstw-rect_x;
	}
	if(rect_y+rect_h>dsth)
	{
		rect_h=dsth-rect_y;
	}
	if(rect_w<=0||rect_h<=0)
	{
		return;
	}
	off=rect_y*dstw+rect_x;
	do
	{
		w4(dst+off,color,rect_w);
		off+=dstw;
		--rect_h;
	}
	while(rect_h);
}
