#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>
#include "iformat.c"
#include "rect.c"
#include "font.c"
int WINW,WINH;
int max_namelen;
WNDCLASSEXA wc,wc2;
int wdiff,hdiff;
int paint,refresh;
void *dc,*memdc,*bmp;
unsigned int *pbuf;
int is_root;
char *cwd=NULL;
void *hwnd;
void *hdialog;
#include "icons.c"
int current_y;
void display_all(void);
#include "file.c"
#include "dialog.c"
void display_all(void)
{
	free(pbuf);
	pbuf=malloc(WINW*WINH*4);
	if(pbuf==NULL)
	{
		return;
	}
	dc=GetDC(hwnd);
	memdc=CreateCompatibleDC(dc);
	bmp=CreateCompatibleBitmap(dc,WINW,WINH);
	SelectObject(memdc,bmp);

	rect(pbuf,WINW,WINH,0,0,WINW,WINH,0xd0d0d0);

	p_files();

	rect(pbuf,WINW,WINH,0,0,WINW,24,0xc0ffff);
	rect(pbuf,WINW,WINH,WINW-48,0,48,24,0x00ff00);
	p_str("Paste",5,WINW-48+4,4,0x000000,pbuf,WINW,WINH);
	rect(pbuf,WINW,WINH,WINW-48-32,0,32,24,0xffff00);
	p_str("New",3,WINW-48-32+4,4,0x000000,pbuf,WINW,WINH);

	if(cwd)
	{
		int pos;
		pos=strlen(cwd);
		pos-=(WINW-20-32-48)/8;
		if(pos<0)
		{
			pos=0;
		}
		p_str(cwd+pos,strlen(cwd)-pos,20,4,0x000000,pbuf,WINW,WINH);
	}
	else
	{
		p_str("(ROOT)",6,20,4,0x000000,pbuf,WINW,WINH);
	}
	icon_paint(icon_goback,0,4);

	SetBitmapBits(bmp,WINW*WINH*4,pbuf);
	BitBlt(dc,0,0,WINW,WINH,memdc,0,0,SRCCOPY);
	DeleteObject(bmp);
	DeleteDC(memdc);
}
LRESULT CALLBACK WndProc(void *hwnd,unsigned int Message,WPARAM wParam,LPARAM lParam)
{
	if(hwnd==hdialog)
	{
		dialog_proc(Message,wParam,lParam);
		return DefWindowProcA(hwnd,Message,wParam,lParam);
	}
	if(Message==WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	if(Message==WM_ERASEBKGND)
	{
		InvalidateRect(hwnd,NULL,0);
	}
	if(Message==WM_PAINT)
	{
		PAINTSTRUCT ps;
		RECT winsize;
		BeginPaint(hwnd,&ps);
		GetWindowRect(hwnd,&winsize);
		WINW=winsize.right-winsize.left-wdiff;
		WINH=winsize.bottom-winsize.top-hdiff;
		if (WINW < 500)
		{
			max_namelen = 2;
		}
		else
		{
			max_namelen = (WINW - 480) / 8;
		}
		
		display_all();
		EndPaint(hwnd,&ps);
	}
	if(Message==WM_TIMER)
	{
		get_files();
		InvalidateRect(hwnd,NULL,0);
	}
	if(Message==WM_KEYDOWN)
	{
		if(wParam==38)
		{
			if(current_y<24)
			{
				current_y=0;
			}
			else
			{
				current_y-=24;
			}
		}
		if(wParam==40)
		{
			if(24*num_files>WINH-24)
			{
				if(current_y>24*num_files-WINH)
				{
					current_y=24*num_files-WINH+24;
				}
				else
				{
					current_y+=24;
				}
			}
		}
		if(wParam=='R')
		{
			input_type=5;
			input_x=0;
			input_size_max=4096;
			input_buf[0]=0;
			update_dialog(2,WINW,0);
		}
		InvalidateRect(hwnd,NULL,0);
	}
	if(Message==WM_MOUSEWHEEL)
	{
		int move;
		update_dialog(0,0,0);
		selected_file[0]=0;
		move=GET_Y_LPARAM(wParam);
		move/=120;
		move*=24;
		if(move<0)
		{
			if(24*num_files>WINH-24)
			{
				if(current_y-move>24*num_files-WINH+24)
				{
					current_y=24*num_files-WINH+24;
				}
				else
				{
					current_y-=move;
				}
			}
		}
		else
		{
			if(current_y>move)
			{
				current_y-=move;
			}
			else
			{
				current_y=0;
			}
		}
		InvalidateRect(hwnd,NULL,0);
	}
	if(Message==WM_LBUTTONUP)
	{
		struct file *file;
		update_dialog(0,0,0);
		selected_file[0]=0;
		file=file_from_point(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		if(file)
		{
			if(file->isdir)
			{
				is_root=0;
				change_wd(file->name);
			}
		}
		else if(GET_X_LPARAM(lParam)<16&&GET_Y_LPARAM(lParam)>=4&&GET_Y_LPARAM(lParam)<20)
		{
			if(!is_root)
			{
				if(cwd[3]==0)
				{
					is_root=1;
					current_y=0;
					selected_file[0]=0;
				}
				else
				{
					change_wd("..");
				}
			}
		}
		else if(GET_Y_LPARAM(lParam)<24&&GET_X_LPARAM(lParam)>=WINW-48-32)
		{
			if(GET_X_LPARAM(lParam)<WINW-48)
			{
				update_dialog(5,WINW-48-32,24);
			}
			else
			{
				if(copy_file_path)
				{
					input_type=copy_type;
					input_x=0;
					strcpy(input_buf,copy_file_name);
					input_size_max=255;
					update_dialog(2,WINW-48,24);
				}
			}
		}
	}
	if(Message==WM_RBUTTONUP)
	{
		struct file *file;
		update_dialog(0,0,0);
		selected_file[0]=0;
		file=file_from_point(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		if(file&&cwd)
		{
			strcpy(selected_file,file->name);
			update_dialog(1,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
		}
	}
	return DefWindowProcA(hwnd,Message,wParam,lParam);
}
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	MSG msg;
	RECT wrect,crect;
	SetProcessDPIAware();
	init_icons();
	
	WINW=640;
	WINH=480;
	max_namelen=(WINW-480)/8;
	wc.cbSize=sizeof(wc);
	wc.hInstance=hInstance;
	wc.lpfnWndProc=WndProc;
	wc.lpszClassName="TEST";
	wc.hbrBackground=(void *)8;
	wc.hCursor=LoadCursorA(NULL,IDC_ARROW);
	wc.hIcon=LoadIconA(NULL,IDI_APPLICATION);
	if(!RegisterClassExA(&wc))
	{
		return 0;
	}
	wc2.cbSize=sizeof(wc2);
	wc2.hInstance=hInstance;
	wc2.lpfnWndProc=WndProc;
	wc2.lpszClassName="TEST2";
	wc2.hbrBackground=(void *)8;
	wc2.hCursor=LoadCursorA(NULL,IDC_ARROW);
	wc2.hIcon=LoadIconA(NULL,IDI_APPLICATION);
	if(!RegisterClassExA(&wc2))
	{
		return 0;
	}
	get_files();
	hwnd=CreateWindowExA(WS_EX_WINDOWEDGE,"TEST","Files (Press R to run a command)",WS_SYSMENU|WS_CAPTION|WS_THICKFRAME,CW_USEDEFAULT,CW_USEDEFAULT,WINW,WINH,NULL,NULL,hInstance,NULL);
	if(hwnd==NULL)
	{
		return 0;
	}
	GetWindowRect(hwnd,&wrect);
	GetClientRect(hwnd,&crect);
	wdiff=(wrect.right-wrect.left)-(crect.right-crect.left);
	hdiff=(wrect.bottom-wrect.top)-(crect.bottom-crect.top);
	MoveWindow(hwnd,wrect.left,wrect.top,WINW+wdiff,WINH+hdiff,0);
	hdialog=CreateWindowExA(0,"TEST2","Files",WS_POPUP,0,0,300,300,hwnd,NULL,hInstance,NULL);
	if(hdialog==NULL)
	{
		return 0;
	}
	ShowWindow(hwnd,5);
	SetTimer(hwnd,0,100,NULL);
	while(GetMessageA(&msg,NULL,0,0)>0)
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	return msg.wParam;
}
