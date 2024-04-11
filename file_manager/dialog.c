int dialog_type;
char *copy_file_path;
char copy_file_name[512];
int copy_type;
int input_type;
char input_buf[16384];
int input_x,input_size_max;
unsigned int dialog_pbuf[600*600];
void dialog_paint(void);
void exec_program(char* path, char* arg)
{
	PROCESS_INFORMATION pinfo;
	STARTUPINFOA sinfo;
	memset(&sinfo, 0, sizeof(sinfo));
	sinfo.cb = sizeof(sinfo);
	if (CreateProcessA(path, arg, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &pinfo))
	{
		CloseHandle(pinfo.hProcess);
		CloseHandle(pinfo.hThread);
	}

}
void update_dialog(int type,int x,int y)
{
	RECT rect;
	POINT pos;
	dialog_type=type;
	GetWindowRect(hwnd,&rect);
	pos.x=0;
	pos.y=0;
	ClientToScreen(hwnd,&pos);
	if(!cwd||type==0) // hide
	{
		ShowWindow(hdialog,0);
	}
	else if(type==1) // context menu
	{
		MoveWindow(hdialog,x+pos.x,y+pos.y,64,120,0);
		ShowWindow(hdialog,4);
		dialog_paint();
	}
	else if(type==2) // input
	{
		MoveWindow(hdialog,x+pos.x,y+pos.y,300,300,0);
		ShowWindow(hdialog,5);
		dialog_paint();
	}
	else if(type==3) // remove
	{
		MoveWindow(hdialog,x+pos.x,y+pos.y,300,300,0);
		ShowWindow(hdialog,5);
		dialog_paint();
	}
	else if(type==4) // status
	{
		MoveWindow(hdialog,20,20,300,600,0);
		ShowWindow(hdialog,5);
		dialog_paint();
	}
	else if(type==5) // 'new' menu
	{
		MoveWindow(hdialog,x+pos.x,y+pos.y,80,48,0);
		ShowWindow(hdialog,4);
		dialog_paint();
	}
}

int is_input_valid(void)
{
	if(input_buf[0]==0)
	{
		return 0;
	}
	if(input_type<5)
	{
		if(file_has_bad_character(input_buf))
		{
			return 0;
		}
		if(file_exists(input_buf))
		{
			return 0;
		}
	}
	return 1;
}
void dialog_paint(void)
{
	void *dc,*memdc,*bmp;
	rect(dialog_pbuf,600,600,0,0,600,600,0xffffc0);
	if(dialog_type==1)
	{
		rect(dialog_pbuf,600,600,0,0,64,1,0x000000);
		rect(dialog_pbuf,600,600,0,24,64,1,0x000000);
		rect(dialog_pbuf,600,600,0,48,64,1,0x000000);
		rect(dialog_pbuf,600,600,0,72,64,1,0x000000);
		rect(dialog_pbuf,600,600,0,96,64,1,0x000000);
		rect(dialog_pbuf,600,600,0,119,64,1,0x000000);
		rect(dialog_pbuf,600,600,0,0,1,120,0x000000);
		rect(dialog_pbuf,600,600,63,0,1,120,0x000000);
		p_str("Copy",4,4,4,0x000000,dialog_pbuf,600,600);
		p_str("Execute",7,4,28,0x000000,dialog_pbuf,600,600);
		p_str("Move",4,4,52,0x000000,dialog_pbuf,600,600);
		p_str("Remove",6,4,76,0x000000,dialog_pbuf,600,600);
		p_str("Status",6,4,100,0x000000,dialog_pbuf,600,600);
	}
	else if(dialog_type==2)
	{
		int x,y,i;
		rect(dialog_pbuf,600,600,0,0,300,1,0x000000);
		rect(dialog_pbuf,600,600,0,0,1,300,0x000000);
		rect(dialog_pbuf,600,600,0,299,300,1,0x000000);
		rect(dialog_pbuf,600,600,299,0,1,300,0x000000);
		if(input_type==1)
		{
			p_str("Paste to",8,4,4,0x0000ff,dialog_pbuf,600,600);
		}
		else if(input_type==2)
		{
			p_str("Move to",7,4,4,0x0000ff,dialog_pbuf,600,600);
		}
		else if(input_type==3)
		{
			p_str("File Name",9,4,4,0x0000ff,dialog_pbuf,600,600);
		}
		else if(input_type==4)
		{
			p_str("Directory Name",14,4,4,0x0000ff,dialog_pbuf,600,600);
		}
		else if(input_type==5)
		{
			p_str("Execute",7,4,4,0x0000ff,dialog_pbuf,600,600);
		}
		y=24;
		x=4;
		i=0;
		while(input_buf[i])
		{
			p_char(input_buf[i],x,y,0x000000,dialog_pbuf,600,600);
			if(i==input_x)
			{
				rect(dialog_pbuf,600,600,x,y+14,8,2,0x00ffff);
			}
			x+=8;
			if(x>288)
			{
				y+=16;
				x=4;
			}
			++i;
		}
		if(i==input_x)
		{
			rect(dialog_pbuf,600,600,x,y+14,8,2,0x00ffff);
		}
		if(is_input_valid())
		{
			rect(dialog_pbuf,600,600,150-12,300-32,24,24,0xff0000);
			p_str("OK",2,150-12+4,300-32+4,0xffffff,dialog_pbuf,600,600);
		}
		else
		{
			rect(dialog_pbuf,600,600,150-12,300-32,24,24,0x404040);
			p_str("OK",2,150-12+4,300-32+4,0x808080,dialog_pbuf,600,600);
		}
	}
	else if(dialog_type==3)
	{
		int x,y,i;
		rect(dialog_pbuf,600,600,0,0,300,1,0x000000);
		rect(dialog_pbuf,600,600,0,0,1,300,0x000000);
		rect(dialog_pbuf,600,600,0,299,300,1,0x000000);
		rect(dialog_pbuf,600,600,299,0,1,300,0x000000);
		p_str("Remove",6,4,4,0xff0000,dialog_pbuf,600,600);
		y=24;
		x=4;
		i=0;
		while(input_buf[i])
		{
			p_char(input_buf[i],x,y,0x000000,dialog_pbuf,600,600);
			x+=8;
			if(x>288)
			{
				y+=16;
				x=4;
			}
			++i;
		}
		rect(dialog_pbuf,600,600,4,300-32,24,24,0xff0000);
		p_str("OK",2,4+4,300-32+4,0xffffff,dialog_pbuf,600,600);
		rect(dialog_pbuf,600,600,300-4-56,300-32,56,24,0xff0000);
		p_str("Cancel",6,300-56,300-32+4,0xffffff,dialog_pbuf,600,600);
	}
	else if(dialog_type==4)
	{
		int x,y,i;
		rect(dialog_pbuf,600,600,0,0,300,1,0x000000);
		rect(dialog_pbuf,600,600,0,0,1,600,0x000000);
		rect(dialog_pbuf,600,600,0,599,300,1,0x000000);
		rect(dialog_pbuf,600,600,299,0,1,600,0x000000);
		p_str("Status",6,4,4,0x00ff00,dialog_pbuf,600,600);
		y=24;
		x=4;
		i=0;
		while(input_buf[i])
		{
			p_char(input_buf[i],x,y,0x000000,dialog_pbuf,600,600);
			x+=8;
			if(x>288||input_buf[i]=='\n')
			{
				y+=16;
				x=4;
			}
			++i;
		}
		rect(dialog_pbuf,600,600,150-12,600-32,24,24,0xff0000);
		p_str("OK",2,150-12+4,600-32+4,0xffffff,dialog_pbuf,600,600);
	}
	else if(dialog_type==5)
	{
		rect(dialog_pbuf,600,600,0,0,80,1,0x000000);
		rect(dialog_pbuf,600,600,0,24,80,1,0x000000);
		rect(dialog_pbuf,600,600,0,47,80,1,0x000000);
		rect(dialog_pbuf,600,600,0,0,1,48,0x000000);
		rect(dialog_pbuf,600,600,79,0,1,48,0x000000);
		p_str("File",4,4,4,0x000000,dialog_pbuf,600,600);
		p_str("Directory",9,4,28,0x000000,dialog_pbuf,600,600);
	}
	dc=GetDC(hdialog);
	memdc=CreateCompatibleDC(dc);
	bmp=CreateCompatibleBitmap(dc,600,600);
	SelectObject(memdc,bmp);
	SetBitmapBits(bmp,sizeof(dialog_pbuf),dialog_pbuf);
	BitBlt(dc,0,0,600,600,memdc,0,0,SRCCOPY);
	DeleteDC(memdc);
	DeleteObject(bmp);
}
void dialog_proc(unsigned int Message,WPARAM wParam,LPARAM lParam)
{
	if(Message==WM_PAINT)
	{
		PAINTSTRUCT ps;
		BeginPaint(hdialog,&ps);
		dialog_paint();
		EndPaint(hdialog,&ps);
	}
	else if(Message==WM_ACTIVATE)
	{
		if(wParam==0)
		{
			update_dialog(0,0,0);
			selected_file[0]=0;
		}
	}
	else if(Message==WM_LBUTTONUP)
	{
		int x,y;
		int close_dialog;
		close_dialog=1;
		x=LOWORD(lParam);
		y=HIWORD(lParam);
		if(dialog_type==1)
		{
			if(y<24)
			{
				if(cwd)
				{
					free(copy_file_path);
					copy_file_path=malloc(strlen(cwd)+strlen(selected_file)+100);
					if(copy_file_path)
					{
						strcpy(copy_file_path,cwd);
						strcat(copy_file_path,"\\");
						strcat(copy_file_path,selected_file);
						strcpy(copy_file_name,selected_file);
						copy_type=1;
					}
				}
			}
			else if(y<48)
			{
				exec_program(selected_file,NULL);
			}
			else if(y<72)
			{
				if(cwd)
				{
					free(copy_file_path);
					copy_file_path=malloc(strlen(cwd)+strlen(selected_file)+10);
					if(copy_file_path)
					{
						strcpy(copy_file_path,cwd);
						strcat(copy_file_path,"\\");
						strcat(copy_file_path,selected_file);
						strcpy(copy_file_name,selected_file);
						copy_type=2;
					}
				}
			}
			else if(y<96)
			{
				close_dialog=0;
				strcpy(input_buf,"Remove file \"");
				strcat(input_buf,selected_file);
				strcat(input_buf,"\" permanently?");
				update_dialog(3,WINW,0);
			}
			else
			{
				close_dialog=0;
				if(!fill_file_status(selected_file,input_buf))
				{
					update_dialog(4,x,y);
				}
			}
		}
		else if(dialog_type==2)
		{
			x-=150-12;
			y-=300-32;
			if(x>=0&&x<24&&y>=0&&y<24)
			{
				if(is_input_valid())
				{
					if(input_type==1&&cwd)
					{
						char *new_path;
						new_path=malloc(strlen(cwd)+520);
						if(new_path)
						{
							strcpy(new_path,cwd);
							strcat(new_path,"/");
							strcat(new_path,input_buf);
							copy_file(copy_file_path,new_path);
							free(new_path);
						}
					}
					if(input_type==2&&cwd)
					{
						char *new_path;
						new_path=malloc(strlen(cwd)+520);
						if(new_path)
						{
							strcpy(new_path,cwd);
							strcat(new_path,"/");
							strcat(new_path,input_buf);
							move_file(copy_file_path,new_path);
							free(new_path);
						}
					}
					if(input_type==3&&cwd)
					{
						void *fp;
						fp=fopen(input_buf,"wb");
						if(fp)
						{
							fclose(fp);
						}
					}
					if(input_type==4&&cwd)
					{
						CreateDirectoryA(input_buf,NULL);
					}
					if(input_type==5&&cwd)
					{
						exec_program(NULL,input_buf);
					}
				}
			}
		}
		else if(dialog_type==3)
		{
			if(x>=4&&x<4+24&&y>=300-32&&y<300-32+24&&cwd)
			{
				char *new_path;
				new_path=malloc(strlen(cwd)+520);
				if(new_path)
				{
					strcpy(new_path,cwd);
					strcat(new_path,"/");
					strcat(new_path,selected_file);
					remove_file(new_path);
					free(new_path);
				}
			}
		}
		else if(dialog_type==5)
		{
			close_dialog=0;
			if(y<24)
			{
				input_type=3;
			}
			else
			{
				input_type=4;
			}
			input_x=0;
			input_buf[0]=0;
			input_size_max=255;
			update_dialog(2,WINW-48,24);
		}
		if(close_dialog)
		{
			update_dialog(0,0,0);
			selected_file[0]=0;
		}
	}
	else if(Message==WM_CHAR)
	{
		if(dialog_type==2)
		{
			if(wParam>=32&&wParam<=126)
			{
				if(strlen(input_buf)<input_size_max)
				{
					memmove(input_buf+input_x+1,input_buf+input_x,16384-input_x-1);
					input_buf[input_x]=wParam;
					++input_x;
					dialog_paint();
				}
			}
		}
	}
	else if(Message==WM_KEYDOWN)
	{
		if(dialog_type==2)
		{
			if(wParam==37)
			{
				if(input_x)
				{
					--input_x;
					dialog_paint();
				}
			}
			else if(wParam==39)
			{
				if(input_buf[input_x])
				{
					++input_x;
					dialog_paint();
				}
			}
			else if(wParam==8)
			{
				if(input_x)
				{
					memmove(input_buf+input_x-1,input_buf+input_x,16384-input_x);
					--input_x;
					dialog_paint();
				}
			}
		}
	}
}

