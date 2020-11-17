#include<stdio.h>

#include<stdlib.h>

#include<unistd.h>

#include<fcntl.h>

#include<string.h>

#define MAXFILES 100

#define FILESIZE 1024

#define READ 4

#define WRITE 2

#define REGULER 1

#define SPECIAL 2

#define START 0

#define CURRENT 1

#define END 2

void BackupFS();

struct SuperBlock

{

    	int TotalInodes;
    
	int FreeInode;

}Obj_Super;

struct inode

{

    	char File_name[50];
    
	int Inode_number;

    	int File_Size;
    	
    	int permission;

    	int File_Type;

    	int ActualFileSize;

    	int Link_Count;

    	int Reference_Count;

    	char * Data;

    	struct inode *next;
};


typedef struct inode INODE;

typedef struct inode * PINODE;

typedef struct inode ** PPINODE;

struct FileTable
{

    	int ReadOffset;
	
	int WriteOffset;

    	int Count;

    	PINODE iptr;

    	int Mode;

};


typedef FileTable FILETABLE;

typedef FileTable * PFILETABLE;

struct UFDT

{

    	PFILETABLE ufdt[MAXFILES];

}UFDTObj;



PINODE Head = NULL; // Global pointer of inode
    
//it is used to check weather the given file name is already present or not

bool CheckFile(char *name)
{
	PINODE temp=Head;

	while(temp != NULL)
	{
		if(temp->File_Type != 0)			//means the file is delete
		{
			if(strcmp(temp->File_name,name)==0)
			{
				break;
			}
		}

		temp=temp->next;
	}

	if(temp == NULL)
	{
		return false;
	}
	
	else
	{
		return true;
	}



}
void CreateUFDT()
{

    	int i = 0;
    

    	for(i = 0; i < MAXFILES; i++)

    	{
    
	    	UFDTObj.ufdt[i] = NULL;

    	}

}


//create linkedlist of inode

void CreateDILB()   
{

    	int i = 1;
    
	PINODE newn = NULL;

    	PINODE temp = Head;
    
    while(i <= MAXFILES)   // Loop iterates 100 times

    {

    	    newn = (PINODE)malloc(sizeof(INODE));
    

        
	    newn->Inode_number = i;

    	    newn->File_Size = FILESIZE;

    	    newn->File_Type = 0;

    	    newn->ActualFileSize = 0;

    	    newn->Link_Count = 0;

    	    newn->Reference_Count = 0;

    	    newn->Data = NULL;

    	    newn->next = NULL;
        

    	    if(Head == NULL)    // First inode
    
	    {
        
		    Head = newn;

		    temp = Head;
        
	    }
       
	    else    // iNode second onwords
        
	    {
            
		    temp->next = newn;
           
		    temp = temp ->next;
       
	    }
        
        
	    i++;
    
    
    }
    
    
    printf("DILB created succesfully!!\n");

}


void CreateSuperBlock()

{

    	Obj_Super.TotalInodes = MAXFILES;
    
	Obj_Super.FreeInode = MAXFILES;
    
   
       	printf("Super block created Succesfully\n");
}

void SetEnvoirnment()

{

    	CreateDILB();
    
	CreateSuperBlock();

    	CreateUFDT();

    	printf("Envoirnment for the Virtual file system is set...\n");

}


void DeleteFile(char *name)

{
	bool bret=false;

	if(name == NULL)
	{
		return ;
	}

	bret=CheckFile(name);
	{
		if(bret == false)
		{
			printf("there is no file \n");

			return ;	
		}

		int i=0;

		for(i=0;i<MAXFILES;i++)
		{
			if(strcmp(UFDTObj.ufdt[i]->iptr->File_name,name)==0)
			{
				break;
			}
		}

		strcpy(UFDTObj.ufdt[i]->iptr->File_name,"");
		
		UFDTObj.ufdt[i]->iptr->File_Type = 0;			//if it is zero means file is deleted

 	      	UFDTObj.ufdt[i]->iptr->ActualFileSize = 0;
	
	       	UFDTObj.ufdt[i]->iptr->Link_Count=0;

        	UFDTObj.ufdt[i]->iptr->Reference_Count = 0;

		//free the memory of file
		

		free(UFDTObj.ufdt[i]->iptr->Data);

		free(UFDTObj.ufdt[i]);

		UFDTObj.ufdt[i]=NULL;



	}
	
}

int GetFDFromName(char *name)

{

       	int i = 0;


	while(i<50)
 
	{


		if(UFDTObj.ufdt[i] != NULL)

		     	if(strcmp((UFDTObj.ufdt[i]->iptr->File_name),name)==0)

			  	break;
		i++;
 
	}


       	if(i == 50)

		return -1;

       	else

		return i;

}

PINODE Get_Inode(char * name)

{

       	PINODE temp = Head;

       	int i = 0;

	 if(name == NULL)

	return NULL;

	 while(temp!= NULL)
 	{

		if(strcmp(name,temp->File_name) == 0)

		break;

		temp = temp->next;
 	}
	
	 return temp;

}

int CreateFile(char *name,int Permission)

{

	bool bret=false;

	if( (name == NULL) || (Permission > READ+WRITE) || (Permission  < WRITE ))
	{
		return -1;
	}

	bret=CheckFile(name);

	if(bret == true)
	{
		printf("file is already present\n");

		return -1;
	}

	if(Obj_Super.FreeInode == 0)
	{
		printf("there is no inode to create a file\n");

		return -1;
	}

	//search for entry from UFDT
	
	int i=0;

	for(i=0;i<MAXFILES;i++)
	{
		if(UFDTObj.ufdt[i] == NULL)
		{
			break;
		}
	}

	if(i==MAXFILES)
	{
		printf("unable to get entry in UFDT\n");

	}

	//allocate the memory for file table
	
	UFDTObj.ufdt[i]=(PFILETABLE)malloc(sizeof(FILETABLE));

	//initialize the file table
	

	UFDTObj.ufdt[i]->ReadOffset = 0;

	UFDTObj.ufdt[i]->WriteOffset = 0;

	UFDTObj.ufdt[i]->Mode = Permission;

	UFDTObj.ufdt[i]->Count = 1;

	//search empty inode
	
	PINODE temp=Head;

	while(temp != NULL)
	{
		if(temp->File_Type == 0)
		{
			break;
		}

		temp = temp->next;
	}

	UFDTObj.ufdt[i]->iptr=temp;

	strcpy( UFDTObj.ufdt[i]->iptr->File_name,name);

         UFDTObj.ufdt[i]->iptr->File_Type = REGULER;  //if it is 0 means file is dleted

         UFDTObj.ufdt[i]->iptr->ActualFileSize = 0;

         UFDTObj.ufdt[i]->iptr->Link_Count = 1;

         UFDTObj.ufdt[i]->iptr->Reference_Count=1;

       //allocation memory for file name
       
	 UFDTObj.ufdt[i]->iptr->Data=(char *)malloc(sizeof(FILESIZE));

	 Obj_Super.FreeInode--;

	return i;
}



int fstat_file(int fd)

{

       	PINODE temp = Head;

       	int i = 0;


       	if(fd < 0)

		return -1;


	if(UFDTObj.ufdt[fd] == NULL)


	       	return -2;


       	temp = UFDTObj.ufdt[fd]->iptr;


       	printf("\n---------Statistical Information about file----------\n");

       	printf("File name : %s\n",temp->File_name);

	printf("Inode Number %d\n",temp->Inode_number);

       	printf("File size : %d\n",temp->File_Size);

       	printf("Actual File size : %d\n",temp->ActualFileSize);

       	printf("Link count : %d\n",temp->Link_Count);

	printf("Reference count : %d\n",temp->Reference_Count);


	if(temp->permission == 1)


		printf("File Permission : Read only\n");

       	else if(temp->permission == 2)


		printf("File Permission : Write\n");

       	else if(temp->permission == 3)


		printf("File Permission : Read & Write\n");

       	printf("------------------------------------------------------\n\n");


       	return 0;
}


int stat_file(char *name)

{

       	PINODE temp = Head;

       	int i = 0;


       	if(name == NULL) return -1;


       	while(temp!= NULL)

       	{


		if(strcmp(name,temp->File_name) == 0)


			break;


		temp = temp->next;

	}


       	if(temp == NULL) return -2;


       	printf("\n---------Statistical Information about file----------\n");

       	printf("File name : %s\n",temp->File_name);

       	printf("Inode Number %d\n",temp->Inode_number);

       	printf("File size : %d\n",temp->File_Size);

       	printf("Actual File size : %d\n",temp->ActualFileSize);

       	printf("Link count : %d\n",temp->Link_Count);

       	printf("Reference count : %d\n",temp->Reference_Count);


       	if(temp->permission == 1)


		printf("File Permission : Read only\n");

       	else if(temp->permission == 2)


		printf("File Permission : Write\n");

       	else if(temp->permission == 3)


		printf("File Permission : Read & Write\n");

       	printf("------------------------------------------------------\n\n");


       	return 0;

}


int LseekFile(int fd, int size, int from)

{

	if((fd<0) || (from > 2)) return -1;

	if(UFDTObj.ufdt[fd] == NULL) return -1;


	if((UFDTObj.ufdt[fd]->Mode == READ) || (UFDTObj.ufdt[fd]->Mode == READ+WRITE))
 
	{


		if(from == CURRENT)


		{


			if(((UFDTObj.ufdt[fd]->ReadOffset) + size) > UFDTObj.ufdt[fd]->iptr->ActualFileSize)

				return -1;


			if(((UFDTObj.ufdt[fd]->ReadOffset) + size) < 0) 


				return -1;


			(UFDTObj.ufdt[fd]->ReadOffset) = (UFDTObj.ufdt[fd]->ReadOffset) +size;


		}


		else if(from == START)


		{


			if(size > (UFDTObj.ufdt[fd]->iptr->ActualFileSize))


			       	return -1;


			if(size < 0) 


				return -1;


			(UFDTObj.ufdt[fd]->ReadOffset) = size;


		}


		else if(from == END)


		{


			if((UFDTObj.ufdt[fd]->iptr->ActualFileSize) + size > FILESIZE)

				return -1;


			if(((UFDTObj.ufdt[fd]->ReadOffset) + size) < 0)


			       	return -1;

			(UFDTObj.ufdt[fd]->ReadOffset) = (UFDTObj.ufdt[fd]->iptr->ActualFileSize) + size;


		}
 
	}

       	else if(UFDTObj.ufdt[fd]->Mode == WRITE)
 
	{

		if(from == CURRENT)


		{

			if(((UFDTObj.ufdt[fd]->WriteOffset) + size) > FILESIZE)

				return -1;


			if(((UFDTObj.ufdt[fd]->WriteOffset) + size) < 0)

				return -1;

			if(((UFDTObj.ufdt[fd]->WriteOffset) + size) > (UFDTObj.ufdt[fd]->iptr->ActualFileSize))


				(UFDTObj.ufdt[fd]->iptr->ActualFileSize) =(UFDTObj.ufdt[fd]->WriteOffset) + size;


			(UFDTObj.ufdt[fd]->WriteOffset) = (UFDTObj.ufdt[fd]->WriteOffset) +size;


		}


		else if(from == START)


		{


			if(size > FILESIZE) return -1;


			if(size < 0) return -1;


			if(size > (UFDTObj.ufdt[fd]->iptr->ActualFileSize))


				(UFDTObj.ufdt[fd]->iptr->ActualFileSize) = size;


			(UFDTObj.ufdt[fd]->WriteOffset) = size;


		}


		else if(from == END)


		{


			if((UFDTObj.ufdt[fd]->iptr->ActualFileSize) + size > FILESIZE)


				return -1;


			if(((UFDTObj.ufdt[fd]->WriteOffset) + size) < 0)


			       	return -1;


			(UFDTObj.ufdt[fd]->WriteOffset) = (UFDTObj.ufdt[fd]->iptr->ActualFileSize) + size;


		}

	}
}
void LS()
{
	PINODE temp=Head;

	while(temp !=NULL)
	{
		if(temp -> File_Type != 0)
		{
			printf("%s\n",temp->File_name);
		}

		temp=temp->next;
	}	
}


int OpenFile(char *name, int mode)

{

       	int i = 0;

       	PINODE temp = NULL;


       	if(name == NULL || mode <= 0)


		return -1;


       	temp = Get_Inode(name);

       	if(temp == NULL)


		return -2;


       	if(temp->permission < mode)


		return -3;


       	while(i<50)
 
	{


		if(UFDTObj.ufdt[i] == NULL)


			break;


		i++;
 
	}


       	UFDTObj.ufdt[i] = (PFILETABLE)malloc(sizeof(FILETABLE));

       	if(UFDTObj.ufdt[i] == NULL)

		return -1;

       	UFDTObj.ufdt[i]->Count = 1;

       	UFDTObj.ufdt[i]->Mode = mode;

       	if(mode == READ + WRITE)
 
	{


		UFDTObj.ufdt[i]->ReadOffset = 0;


		UFDTObj.ufdt[i]->WriteOffset = 0;
 
	}

	else if(mode == READ)
 
	{

		UFDTObj.ufdt[i]->ReadOffset = 0;
 
	}

       	else if(mode == WRITE)
 
	{


		UFDTObj.ufdt[i]->WriteOffset = 0;
 
	}

       	UFDTObj.ufdt[i]->iptr = temp;

       	(UFDTObj.ufdt[i]->iptr->Reference_Count)++;


       	return i;

}



int ReadFile(int fd, char *arr, int isize)

{

      	int read_size = 0;


       	if(UFDTObj.ufdt[fd] == NULL)
  
 
	      	return -1;


       	if(UFDTObj.ufdt[fd]->Mode !=READ && UFDTObj.ufdt[fd]->Mode !=READ+WRITE)
	

     		return -2;


       	if(UFDTObj.ufdt[fd]->iptr->permission != READ && UFDTObj.ufdt[fd]->iptr->permission != READ+WRITE)
 
 
       		return -2;


       	if(UFDTObj.ufdt[fd]->ReadOffset == UFDTObj.ufdt[fd]->iptr->ActualFileSize)


	   	return -3;


       	if(UFDTObj.ufdt[fd]->iptr->File_Type != REGULER)

     
		return -4;


       	read_size = (UFDTObj.ufdt[fd]->iptr->ActualFileSize) - (UFDTObj.ufdt[fd]->ReadOffset);

       	if(read_size < isize)
 
	{


	      	strncpy(arr,(UFDTObj.ufdt[fd]->iptr->Data) + (UFDTObj.ufdt[fd]->ReadOffset),read_size);


 
	       	UFDTObj.ufdt[fd]->ReadOffset = UFDTObj.ufdt[fd]->ReadOffset + read_size;

       	}
 

       	else
 
	{

		strncpy(arr,(UFDTObj.ufdt[fd]->iptr->Data) + (UFDTObj.ufdt[fd]->ReadOffset),isize);


		(UFDTObj.ufdt[fd]->ReadOffset) = (UFDTObj.ufdt[fd]->ReadOffset) + isize;
 
	}


       	return isize;

}

int  WriteFile(int fd,char *arr,int size)

{
	if(UFDTObj.ufdt[fd] == NULL)

	{
		printf("invalid file descripton\n");

		return -1;
	}

	if(UFDTObj.ufdt[fd]->Mode==READ)
	{
		printf("there is no write permision\n");

		return -1 ;	
	}

	//data gets copied into the buffer


	strncpy(((UFDTObj.ufdt[fd]->iptr->Data)+(UFDTObj.ufdt[fd]->ReadOffset)),arr,size);

	 UFDTObj.ufdt[fd]->WriteOffset=UFDTObj.ufdt[fd]->WriteOffset + size;

	 return size;

}


int CloseFileByName(char *name)

{

	int i = 0;

       	i = GetFDFromName(name);


	if(i == -1)


		return -1;


       	UFDTObj.ufdt[i]->ReadOffset = 0;

       	UFDTObj.ufdt[i]->WriteOffset = 0;

       	(UFDTObj.ufdt[i]->iptr->Reference_Count)--;


       	return 0;

}

void CloseAllFile()

{

       	int i = 0;

       	while(i<50)
 
	{


		if(UFDTObj.ufdt[i] != NULL)


		{


			UFDTObj.ufdt[i]->ReadOffset = 0;

			UFDTObj.ufdt[i]->WriteOffset = 0;


			(UFDTObj.ufdt[i]->iptr->Reference_Count)--;

			break;


		}


		i++;
 
	}

}
void DisplayHelp()

{

    	printf("*********************************************************************************************************\n");

    
	printf("open : It is used to open the existing file\n");

    	printf("close : It is used to close the opened file\n");

    	printf("read : It is used to read the contents of file\n");

    	printf("write : It is used to write the data into file\n");
    
	printf("lseek : It is used to change the offset of file\n");
	   
	printf("stat : It is used to odisplay the information of file\n");

	printf("fstat : It is used to display the information of opened file\n");

	printf("creat : It is used to create new regular file\n");

	printf("rm : It is used to delete regular file\n");


    
	printf("******************************************************************************************************\n");
}


void ManPage(char *str)

{

    	if(strcmp(str,"open") == 0)
    
	{
        
		printf("Desricption : It is used to open an existing file \n");
        
		printf("Usage : open File_name Mode\n");
    
	}
    
	else if (strcmp(str,"close") == 0)
    
	{
        
		printf("Desricption : It is used to close the existing file\n");
      
	      	printf("Usage : close File_name\n");
    
	}
    
	else if (strcmp(str,"ls") == 0)
    
	{
        
		printf("Desricption : It is used to list out all names of the files\n");
       
	       	printf("Usage : ls\n");
    
	}
	
	else if(strcmp(str,"truncate") == 0)

	{

		printf("Description : Used to remove data from file\n");

		printf("Usage : truncate File_name\n");

	}
	
	else if (strcmp(str,"lseek") == 0)
	
	{
	
	printf("Description : Used to change file offset\n");

	printf("Usage : lseek File_Name ChangeInOffset StartPoint\n");
 	}

	else if(strcmp(str,"creat") == 0)

        {

                printf("Desricption : It is used to create file \n");

                printf("Usage : creat File_name permission\n");

        }

	
	else if (strcmp(str,"rm") == 0)
	
    	{
	        
		printf("Desricption : It is used to delete regular file\n");
        
		printf("Usage : rm File_name\n");
    	
	}
	
	else if(strcmp(str,"stat") == 0)
	
	{	

		printf("Description : Used to display information of file\n");

		printf("Usage : stat File_name\n");

	}

	else if(strcmp(str,"fstat") == 0)

	{

	printf("Description : Used to display information of file\n");

	printf("Usage : stat File_Descriptor\n");

	}
	
	else if (strcmp(str,"read") == 0)
	
    	{
	        
		printf("Desricption : It is used to read regular file\n");
        
		printf("Usage : read File_name No_Of_Bytes_To_Read\n");
    	
	}
	
	else if(strcmp(str,"write") == 0)

        {

                printf("Desricption : It is used to write the data into the file \n");

                printf("Usage : write File_Descriptor\n");

		printf("After the command please enter the data\n");


        }


       	else
    
	{

        
		printf("Man page not found\n");
    
	}

}



int truncate_File(char *name)

{


       	int fd = GetFDFromName(name);

       	if(fd == -1)


		return -1;


       	memset(UFDTObj.ufdt[fd]->iptr->Data,0,1024);

       	UFDTObj.ufdt[fd]->ReadOffset = 0;

       	UFDTObj.ufdt[fd]->WriteOffset = 0;

       	UFDTObj.ufdt[fd]->iptr->ActualFileSize = 0;

}

int main()

{

    	// Variable Declations

    	char str[80];
    	
    	char *ptr=NULL;
    	
    	int fd=0;

    	char command[4][80];

    	int count = 0;

	int ret=0;
 
     	printf("Customised Virtual File System\n");
    

    	SetEnvoirnment();
    
    
	while(1)
   
       	{
       
	       	printf("shell :> ");
       
	       	fgets(str,80,stdin); // Accept the coomand
        
		fflush(stdin);
        
        
		count = sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]);  // Break that command ito tokens
        
      
	      	if(count == 1)
        
		{
                
			if(strcmp(command[0], "help") == 0)
                
			{
                    
				DisplayHelp();
                
			}
                
			else if(strcmp(command[0],"exit") == 0)
                
			{
				 BackupFS();

              			printf("Thank you for using This  Virtual file system");

				break;
                
			}
                
			else if(strcmp(command[0],"clear") == 0)
                
			{
               
			   	system("clear");    // cls
                
			}

			else if(strcmp(command[0],"ls") == 0)
			{
				LS();
			}
			
			else if(strcmp(command[0],"closeall") == 0)

   				{

					CloseAllFile();
					
					printf("All files closed successfully\n");
					
					continue;
				}
               
		       	else
                
			   {
                    
				printf("Command not found !!\n");
                
			   }
        
		}
        
		else if(count == 2)
        
		{
           
		       	if(strcmp(command[0],"man") == 0)   // man open
            
			{
                
				ManPage(command[1]);
            
			}

		
			else if(strcmp(command[0],"rm")==0)
		
			{
		
				DeleteFile(command[1]);
		
			}
			
			else if(strcmp(command[0],"truncate") == 0)
 
			{

				ret = truncate_File(command[1]);

				if(ret == -1)

					printf("Error : Incorrect parameter\n");

			}
			
			else if(strcmp(command[0],"stat") == 0)

			{

				ret = stat_file(command[1]);

				if(ret == -1)

					printf("ERROR : Incorrect parameters\n");

				if(ret == -2)

					printf("ERROR : There is no such file\n");

				continue;

			}

			else if(strcmp(command[0],"fstat") == 0)

			{

				ret = fstat_file(atoi(command[1]));

				if(ret == -1)

					printf("ERROR : Incorrect parameters\n");

				if(ret == -2)

					printf("ERROR : There is no such file\n");

				continue;

			}
			
			
			else if(strcmp(command[0],"close") == 0)
		
			{

			ret = CloseFileByName(command[1]);
			
			printf("file close successfully\n");
			
			if(ret == -1)

			printf("ERROR : There is no such file\n");
			
			continue;
			
		}
			
			else if(strcmp(command[0],"write")==0)
			
			{
				char arr[1024];

				printf("please eneter data to write\n");

				//scanf("%[^'\n']s",arr);

				fgets(arr,1024,stdin);

				fflush(stdin);

				int ret=WriteFile(atoi(command[1]),arr,strlen(arr)-1);

				if(ret == -1)
				{
					printf("%d bytes get written successfully in the file\n",ret);

					return -1;
				}
			}
            
			else
            
			{
                
				printf("Command not found !!\n");
            
			}
        
		}
        
		else if (count == 3)
        	{
			if(strcmp(command[0],"creat")==0)
			{
				int fd=0;

				fd=CreateFile(command[1],atoi(command[2]));

				if(fd == -1)

				
				{
					printf("Error :unable to create file\n");


				}

				else
				{
					printf("successfully open with FD %d\n",fd);
				}
			}
			
			else if(strcmp(command[0],"open") == 0)
	
			{
			
			ret = OpenFile(command[1],atoi(command[2]));
		
			if(ret >= 0)
			
				printf("File is successfully opened with file descriptor : %d\n",ret);
			
			if(ret == -1)
			
				printf("ERROR : Incorrect parameters\n");
			
			if(ret == -2)
				
				printf("ERROR : File not present\n");
			
			if(ret == -3)

				printf("ERROR : Permission denied\n");
			continue;
			
			}
			
			else if(strcmp(command[0],"read") == 0)

			{

				fd = GetFDFromName(command[1]);

				 if(fd == -1)

				 {

					 printf("Error : Incorrect parameter\n");

					 continue;

				 }

				 ptr = (char *)malloc(sizeof(atoi(command[2]))+1);


				 if(ptr == NULL)


				 {


					 printf("Error : Memory allocation failure\n");


					 continue;


				 }


				 ret = ReadFile(fd,ptr,atoi(command[2]));


				 if(ret == -1)


					 printf("ERROR : File not existing\n");


				 if(ret == -2)


					 printf("ERROR : Permission denied\n");


				 if(ret == -3)


					 printf("ERROR : Reached at end of file\n");


				 if(ret == -4)


					 printf("ERROR : It is not regular file\n");

				 if(ret == 0)


					 printf("ERROR : File empty\n");


				 if(ret > 0)


				 {


					 write(2,ptr,ret);


				 }

				 continue;


			}
			
			else
			{
				printf("\n command not found\n");
			}
	
		}
        
		else if(count == 4)
        	
		{
		if(strcmp(command[0],"lseek") == 0)


		{


			fd = GetFDFromName(command[1]);


			if(fd == -1)


			{

				printf("Error : Incorrect parameter\n");


				continue;


			}


			ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));


			if(ret == -1)


			{


				printf("ERROR : Unable to perform lseek\n");


			}


		}

		else


		{

			printf("\nERROR : Command not found !!!\n");


			continue;


		}


		}
	
		
       
	       	else
        
		{
            
			printf("Bad command or file name\n");
        
		}
        
    
	}
    
    
	return 0;
}


void BackupFS()
{
    int fd = 0;
   
    PINODE temp = Head;

    fd = creat("backup.txt",0777);

    if(fd == -1)
    {

    	    printf("Unable to create the file");
    
	    return ;
    }

  
    while(temp != NULL)
    {
          if(temp->File_Type != 0)
        {
       
            write(fd,temp,sizeof(INODE));
       
	    write(fd,temp->Data, 1024);
        }

  	  temp = temp -> next;
    }
}

