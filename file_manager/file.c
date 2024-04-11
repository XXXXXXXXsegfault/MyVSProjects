struct file
{
	struct file *next;
	char name[512];
	unsigned long long atime;
	unsigned long long ctime;
	unsigned long long size;
	int isdir;
	int exec;
} *file_list;
struct file *files_map[262144];
int num_files;
char selected_file[512];
void release_files(void)
{
	struct file *p;
	while(p=file_list)
	{
		file_list=p->next;
		free(p);
	}
}
int file_has_bad_character(char *name)
{
	int c;
	while(c=*name)
	{
		if(c=='?'||c=='/'||c=='\\'||c=='\"'||c=='>'||c=='<'||c=='*'||c==':'||c=='|')
		{
			return 1;
		}
		if(c<32||c>126)
		{
			return 1;
		}
		++name;
	}
	return 0;
}
int file_exists(char *name)
{
	WIN32_FIND_DATAA fdata;
	void *fh;
	fh=FindFirstFileA(name,&fdata);
	if(fh==INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	FindClose(fh);
	return 1;
}
void get_files(void)
{
	struct file *file,*p,*pp;
	struct file *file_list_end;
	WIN32_FIND_DATAA fdata;
	void *handle;
	int files_map_x;
	memset(files_map,0,sizeof(files_map));
	files_map_x=0;
	file_list_end=NULL;
	free(cwd);
	if(is_root)
	{
		cwd=NULL;
	}
	else
	{
		int cwd_length;
		cwd_length=GetCurrentDirectoryA(0,NULL);
		cwd=malloc(cwd_length+2);
		if(cwd==NULL)
		{
			return;
		}
		GetCurrentDirectoryA(cwd_length,cwd);
		cwd[cwd_length] = 0;
	}
	int name_length;
	release_files();
	int file_selected_exists;
	file_selected_exists=0;
	num_files=0;
	if(is_root)
	{
		unsigned int n,bitmask;
		bitmask=GetLogicalDrives();
		n=26;
		while(n)
		{
			--n;
			if(bitmask&1<<n)
			{
				file=malloc(sizeof(*file));
				if(file)
				{
					memset(file,0,sizeof(*file));
					file->isdir=1;
					file->name[0]=n+'A';
					file->name[1]=':';
					file->name[2]='\\';
					file->name[3]='*';
					if(file_exists(file->name))
					{
						file->name[2]=0;
						file->next=file_list;
						file_list=file;
						files_map[0]=file;
						++num_files;
					}
					else
					{
						free(file);
					}
				}
			}
		}
		return;
	}
	handle=FindFirstFileA("./*",&fdata);
	if(handle!=INVALID_HANDLE_VALUE)
	{
		do
		{
			if(strcmp(fdata.cFileName,".")&&strcmp(fdata.cFileName,"..")&&!file_has_bad_character(fdata.cFileName))
			{
				file=malloc(sizeof(*file));
				if(file!=NULL)
				{
					name_length=strlen(fdata.cFileName);
					file->isdir=fdata.dwFileAttributes&0x10;
					file->atime=(unsigned long long)fdata.ftLastAccessTime.dwHighDateTime<<32|(unsigned long long)fdata.ftLastAccessTime.dwLowDateTime;
					file->ctime=(unsigned long long)fdata.ftLastWriteTime.dwHighDateTime<<32|(unsigned long long)fdata.ftLastWriteTime.dwLowDateTime;
					file->size=(unsigned long long)fdata.nFileSizeHigh<<32|(unsigned long long)fdata.nFileSizeLow;
					file->exec=0;
					strcpy(file->name,fdata.cFileName);
					if(name_length>=4&&
					(!strcmp(file->name+name_length-4,".exe")||!strcmp(file->name+name_length-4,".EXE")||
					!strcmp(file->name+name_length-4,".com")||!strcmp(file->name+name_length-4,".COM")||
					!strcmp(file->name+name_length-4,".cmd")||!strcmp(file->name+name_length-4,".CMD")||
					!strcmp(file->name+name_length-4,".bat")||!strcmp(file->name+name_length-4,".BAT")))
					{
						file->exec=1;
					}
					p=file_list;
					pp=NULL;
					{
						int x;
						x=0;
						while(x<262144&&files_map[x])
						{
							if(strcmp(file->name,files_map[x]->name)>0)
							{
								if(!pp||strcmp(pp->name,files_map[x]->name)<0)
								{
									pp=files_map[x];
									p=pp->next;
								}
							}
							++x;
						}
					}
					while(p)
					{
						if(strcmp(file->name,p->name)<0)
						{
							break;
						}
						pp=p;
						p=p->next;
					}
					if(pp==NULL)
					{
						file_list=file;
					}
					else
					{
						pp->next=file;
					}
					file->next=p;
					if(p==NULL)
					{
						file_list_end=file;
					}
					if(num_files%128==0&&files_map_x<262144)
					{
						files_map[files_map_x]=file;
						++files_map_x;
					}
					++num_files;
				}
			}
			if(!strcmp(selected_file,fdata.cFileName))
			{
				file_selected_exists=1;
			}
		}
		while(FindNextFileA(handle,&fdata));
		FindClose(handle);
	}
	if(!file_selected_exists)
	{
		selected_file[0]=0;
	}
	memset(files_map,0,sizeof(files_map));
	files_map_x=0;
	file=file_list;
	while(file&&files_map_x<8192*64)
	{
		if(files_map_x%64==0)
		{
			files_map[files_map_x/64]=file;
		}
		file=file->next;
		++files_map_x;
	}
}
void size_to_str(unsigned long long size,char *str)
{
	if(size==1)
	{
		strcat(str,"1 Byte");
	}
	else if(size<0x400)
	{
		sprinti(str,size,1);
		strcat(str," Bytes");
	}
	else if(size<0x100000)
	{
		sprinti(str,size>>10,1);
		strcat(str,".");
		sprinti(str,(size&0x3ff)*100>>10,2);
		strcat(str," KB");
	}
	else if(size<0x40000000)
	{
		sprinti(str,size>>20,1);
		strcat(str,".");
		sprinti(str,(size&0xfffff)*100>>20,2);
		strcat(str," MB");
	}
	else
	{
		sprinti(str,size>>30,1);
		strcat(str,".");
		sprinti(str,(size&0x3fffffff)*100>>30,2);
		strcat(str," GB");
	}
}

void time_to_str(unsigned long long t,char *str)
{
	unsigned long long year,month,day,hour,minute,second;
	unsigned long long days;
	int days_in_month[12];
	days_in_month[0]=31;
	days_in_month[2]=31;
	days_in_month[3]=30;
	days_in_month[4]=31;
	days_in_month[5]=30;
	days_in_month[6]=31;
	days_in_month[7]=31;
	days_in_month[8]=30;
	days_in_month[9]=31;
	days_in_month[10]=30;
	days_in_month[11]=31;

	t/=10000000;
	days=t/86400;
	hour=t%86400/3600;
	minute=t%3600/60;
	second=t%60;
	year=1601+400*(days/146097);
	days%=146097;
	while(1)
	{
		if(year%4==0&&year%100!=0||year%400==0)
		{
			days_in_month[1]=29;
			if(days>=366)
			{
				days-=366;
			}
			else
			{
				break;
			}
		}
		else
		{
			days_in_month[1]=28;
			if(days>=365)
			{
				days-=365;
			}
			else
			{
				break;
			}
		}
		++year;
	}
	month=0;
	while(days>=days_in_month[month])
	{
		days-=days_in_month[month];
		++month;
	}
	day=days;
	sprinti(str,month+1,1);
	strcat(str,"/");
	sprinti(str,day+1,1);
	strcat(str,"/");
	sprinti(str,year,1);
	strcat(str," ");
	int is_pm;
	if(hour>=12)
	{
		hour-=12;
		is_pm=1;
	}
	else
	{
		is_pm=0;
	}
	if(hour==0)
	{
		hour=12;
	}
	sprinti(str,hour,1);
	strcat(str,":");
	sprinti(str,minute,2);
	strcat(str,":");
	sprinti(str,second,2);
	if(is_pm)
	{
		strcat(str," PM");
	}
	else
	{
		strcat(str," AM");
	}
}

void p_files(void)
{
	struct file *file;
	char buf[256];
	int name_len;
	int y;
	y=24-current_y%(24*64);
	if(current_y>=8192*64*24)
	{
		file=files_map[8191];
	}
	else
	{
		file=files_map[current_y/(24*64)];
	}
	while(file)
	{
		if(y>=0&&y<WINH)
		{
			if(!strcmp(selected_file,file->name))
			{
				rect(pbuf,WINW,WINH,0,y,WINW,24,0xff80ff);
			}
			if(file->isdir)
			{
				icon_paint(icon_directory,4,y+4);
			}
			else if(file->exec)
			{
				icon_paint(icon_exec_file,4,y+4);
			}
			else
			{
				icon_paint(icon_file,4,y+4);
			}
			name_len=strlen(file->name);
			if(name_len>max_namelen)
			{
				name_len=max_namelen;
			}
			p_str(file->name,name_len,24,y+4,0x000000,pbuf,WINW,WINH);
			if(file->isdir!=1)
			{
				buf[0]=0;
				size_to_str(file->size,buf);
				p_str(buf,strlen(buf),40+max_namelen*8,y+4,0x000000,pbuf,WINW,WINH);
				buf[0]=0;
				time_to_str(file->atime,buf);
				p_str(buf,strlen(buf),240+max_namelen*8,y+4,0x000000,pbuf,WINW,WINH);
			}
		}
		y+=24;
		file=file->next;
	}
}
struct file *file_from_point(int x,int y)
{
	int y1;
	struct file *file;
	if(y<24)
	{
		return NULL;
	}
	y1=24-current_y;
	file=file_list;
	while(file)
	{
		if(y>=y1&&y<y1+24)
		{
			return file;
		}
		y1+=24;
		file=file->next;
	}
	return NULL;
}

void change_wd(char *path)
{
	if(!SetCurrentDirectoryA(path))
	{
		return;
	}
	current_y=0;
	selected_file[0]=0;
	get_files();
	display_all();
}

struct file_op
{
	long long type;
	char *src;
	char *dst;
};
void file_op_release(struct file_op *fop)
{
	free(fop->src);
	free(fop->dst);
	free(fop);
}
struct file_op *file_op_init(int type,char *src,char *dst)
{
	struct file_op *fop;
	fop=malloc(sizeof(*fop));
	if(fop==NULL)
	{
		return NULL;
	}
	memset(fop,0,sizeof(*fop));
	if(src)
	{
		fop->src=malloc(strlen(src)+1);
		if(fop->src==NULL)
		{
			file_op_release(fop);
			return NULL;
		}
		strcpy(fop->src,src);
	}
	if(dst)
	{
		fop->dst=malloc(strlen(dst)+1);
		if(fop->dst==NULL)
		{
			file_op_release(fop);
			return NULL;
		}
		strcpy(fop->dst,dst);
	}
	fop->type=type;
	return fop;
}


int do_copy_file(char *src,char *dst)
{
	char *new_src,*new_dst;
	WIN32_FIND_DATAA fdata;
	void *handle,*fpi,*fpo;
	int src_len;
	src_len=strlen(src);
	if(!strncmp(src,dst,src_len)&&(dst[src_len]=='\\'||src_len==3))
	{
		return 1;
	}
	if(file_exists(dst))
	{
		return 1;
	}
	handle=FindFirstFileA(src,&fdata);
	if(handle==INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	FindClose(handle);
	if(fdata.dwFileAttributes&0x10)
	{
		new_src=malloc(strlen(src)+520);
		if(new_src)
		{
			new_dst=malloc(strlen(dst)+520);
			if(new_dst)
			{
				strcpy(new_src,src);
				strcat(new_src,"/*");
				handle=FindFirstFileA(new_src,&fdata);
				if(handle!=INVALID_HANDLE_VALUE)
				{
					CreateDirectoryA(dst,NULL);
					do
					{
						if(strcmp(fdata.cFileName,".")&&strcmp(fdata.cFileName,".."))
						{
							strcpy(new_src,src);
							strcat(new_src,"/");
							strcat(new_src,fdata.cFileName);
							strcpy(new_dst,dst);
							strcat(new_dst,"/");
							strcat(new_dst,fdata.cFileName);
							if(do_copy_file(new_src,new_dst))
							{
								FindClose(handle);
								free(new_dst);
								free(new_src);
								return 1;
							}
						}
					}
					while(FindNextFileA(handle,&fdata));
					FindClose(handle);
				}
				free(new_dst);
			}
			free(new_src);
		}
	}
	else
	{
		fpi=fopen(src,"rb");
		if(fpi)
		{
			fpo=fopen(dst,"wb");
			if(fpo)
			{
				char buf[256];
				int n;
				while(n=fread(buf,1,256,fpi))
				{
					if(fwrite(buf,1,n,fpo)!=n)
					{
						fclose(fpo);
						fclose(fpi);
						return 1;
					}
				}
				fclose(fpo);
			}
			fclose(fpi);
		}
	}
	return 0;
}
int do_remove_file(char *src)
{
	char *new_src;
	WIN32_FIND_DATAA fdata;
	void *handle;
	handle=FindFirstFileA(src,&fdata);
	if(handle==INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	FindClose(handle);
	if(fdata.dwFileAttributes&0x10)
	{
		new_src=malloc(strlen(src)+520);
		if(new_src)
		{
			strcpy(new_src,src);
			strcat(new_src,"/*");
			handle=FindFirstFileA(new_src,&fdata);
			if(handle!=INVALID_HANDLE_VALUE)
			{
				do
				{
					if(strcmp(fdata.cFileName,".")&&strcmp(fdata.cFileName,".."))
					{
						strcpy(new_src,src);
						strcat(new_src,"/");
						strcat(new_src,fdata.cFileName);
						do_remove_file(new_src);
					}
				}
				while(FindNextFileA(handle,&fdata));
				FindClose(handle);
			}
			free(new_src);
		}
		RemoveDirectoryA(src);
	}
	else
	{
		remove(src);
	}
	return 0;
}
void T_file_op(struct file_op *fop)
{
	if(fop->type==1)
	{
		do_copy_file(fop->src,fop->dst);
	}
	else if(fop->type==2)
	{
		if(rename(fop->src,fop->dst))
		{
			if(!do_copy_file(fop->src,fop->dst))
			{
				do_remove_file(fop->src);
			}
		}
	}
	else if(fop->type==3)
	{
		do_remove_file(fop->src);
	}
	file_op_release(fop);
}
void copy_file(char *src,char *dst)
{
	struct file_op *fop;
	void *handle;
	fop=file_op_init(1,src,dst);
	if(fop==NULL)
	{
		return;
	}
	handle=CreateThread(NULL,0x10000000,T_file_op,fop,0,NULL);
	if(handle)
	{
		CloseHandle(handle);
	}
	else
	{
		file_op_release(fop);
	}
}
void move_file(char *src,char *dst)
{
	struct file_op *fop;
	void *handle;
	fop=file_op_init(2,src,dst);
	if(fop==NULL)
	{
		return;
	}
	handle=CreateThread(NULL,0x10000000,T_file_op,fop,0,NULL);
	if(handle)
	{
		CloseHandle(handle);
	}
	else
	{
		file_op_release(fop);
	}
}
void remove_file(char *name)
{
	struct file_op *fop;
	void *handle;
	fop=file_op_init(3,name,NULL);
	if(fop==NULL)
	{
		return;
	}
	handle=CreateThread(NULL,0x10000000,T_file_op,fop,0,NULL);
	if(handle)
	{
		CloseHandle(handle);
	}
	else
	{
		file_op_release(fop);
	}
}

int fill_file_status(char *name,char *buf)
{
	struct file *file;
	file=file_list;
	while(file)
	{
		if(!strcmp(file->name,name))
		{
			break;
		}
		file=file->next;
	}
	if(!file)
	{
		return 1;
	}
	strcpy(buf,"Type: ");
	if(file->isdir)
	{
		strcat(buf,"Directory\n");
	}
	else
	{
		strcat(buf,"Regular File\n");
	}
	strcat(buf,"Size: ");
	sprinti(buf,file->size,1);
	strcat(buf," (");
	size_to_str(file->size,buf);
	strcat(buf,")\nLast Access: ");
	time_to_str(file->atime,buf);
	strcat(buf,"\nLast Modification: ");
	time_to_str(file->ctime,buf);
	strcat(buf,"\nName: ");
	strcat(buf,file->name);
	return 0;
}
