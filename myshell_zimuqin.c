#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
 
#define normal 0  //�������� 
#define out_redirect 1  //����ض��� 
#define in_redirect 2  //�����ض��� 
#define have_pipe 3  //���йܵ������� 
#define out_redict 4  //�����ǵ�����ض��� 
#define BUFSIZE 256

int argc;
char argv[100][BUFSIZE];
char **arg = NULL;
char buf[BUFSIZE];
char buff[BUFSIZE];
char *current;
char history[500][BUFSIZE];
int commandNum;

int get_input(char buf[]);
void explain_input(char buf[], int *argc, char argv[100][BUFSIZE]);
void deal_with_command(int argcount, char arglist[100][BUFSIZE], int isstart, int isend);
int find_command (char *command);
int dealCd(int argc);
int getHistory();
void split_command(int argct, char argl[100][BUFSIZE]);
 
int main()
{
	int i;
	commandNum = 0;
	while(1) {
		argc = 0;
		memset(buf, 0, BUFSIZE);
		printf("ZIMUQIN$myshell_input$ ");
		get_input(buf);
		
		//exit the shell
		if(strcmp(buf,"exit\n") == 0 || strcmp(buf,"logout\n") == 0)
			break;
		for (i=0; i < 100; i++)
		{
			argv[i][0]='\0';
		}
		argc = 0;
		explain_input(buf, &argc, argv); 
		split_command(argc, argv);	
	}
	exit(0);
}

//���ܲ���������ĺ���qaq����֤�ض���͹ܵ�ǰ���пո� 

int get_input(char buf[]) {
	memset(buff, 0, BUFSIZE);
	fgets(buff, BUFSIZE, stdin);
	strcpy(history[commandNum++],buff);
	int i = 0;
	int j = 0;
	while (buff[i] != '\n') {
		//������������ض���ʱ������ 
		if (buff[i] != '<' && buff[i] != '>' && buff[i] != '|') {
			buf[j] = buff[i];
			j = j + 1;
		}
		else {
			if (buff[i-1] == '<' || buff[i-1] == '>' || buff[i-1] == ' ') {
				buf[j] = buff[i];
				j = j + 1;
			}
			else {
				buf[j] = ' ';
				j = j + 1;
				buf[j] = buff[i];
				j = j + 1;
			}
			if (buff[i+1] == '<' || buff[i+1] == '>' || buff[i+1] == ' ') {
				
			}
			else {
				buf[j] = ' ';
				j = j + 1;
			}
		} 
		i ++;
	}
	buf[j] = '\n';
	j = j + 1;
	buf[j] = '\0';
	return j;
}

void explain_input(char buf[], int *argc, char argv[100][BUFSIZE])
{
	char *p	= buf;
	char *q	= buf;
	int	number = 0;
 
	while (1) {
		if ( p[0] == '\n' )
			break;
 
		if ( p[0] == ' '  )
			p++;
		else {
			q = p;
			number = 0;
			while( (q[0]!=' ') && (q[0]!='\n') ) {
				number++;
				q++;
			}
			strncpy(argv[*argc], p, number+1);
			argv[*argc][number] = '\0';
			*argc = *argc + 1;
			p = q;
		}
	}
}

void split_command(int argct, char argl[100][BUFSIZE]) {
	
	int i = 0;
	int now_cmd = 0;
	int background = 0;
	int status;
	char argnext[100][BUFSIZE];
	int argcnext;
	int isstart = 0;
	pid_t pid44;
	
	if (strcmp(argv[0],"cd") == 0) {
 		int res = dealCd(argc);
 		if (!res) {
 			printf("wrong input");
		 }
 		return;
	}
	
	for (i = 0; i < argct; i++) {
		if (strcmp(argl[i], "&") == 0 && i == argct - 1) {
			background = 1;
			argct = argct - 1;
		}
		else if (strcmp(argl[i], "&") == 0){
			printf("wrong command\n");
			return;
		}
	}
	
	pid44 = fork();
	if (pid44 == 0) {
		for (i = 0; i < argct; i++) {
	
			if (strcmp(argl[i], "|") == 0) {
				int j;
				argcnext = 0;
				int flag = 0;
				for (j = now_cmd; j < i; j++) {
					if (strcmp(argl[i], "<") == 0) {
						flag = 1;
					}
					strcpy(argnext[argcnext],argl[j]);
					argcnext++;
				}
				if (now_cmd == 0) {
					isstart = 1;
				}
				else {
					isstart = 0;
				}
				now_cmd = i + 1;
				deal_with_command(argcnext, argnext, isstart, 0);
				//if (flag == 0) {
				int fd2, fd3;
				fd3 = open("/tmp/youdonotknowfile1",O_RDONLY);
				fd2 = open("/tmp/youdonotknowfile",
						O_WRONLY|O_CREAT|O_TRUNC,0644);
				char buffer[1024] = {0};
			    	char *ptr;
 			   	int count;
				while (count = read(fd3, buffer, 1024))    
				{
			        ptr = buffer;
			        write(fd2, ptr, count);
			        memset(buffer, 0, 1024);
 				}
 				//}
 			}
		}
		
		int j;
		argcnext = 0;
		for (j = now_cmd; j < argct; j++) {
			strcpy(argnext[argcnext],argl[j]);
			argcnext++;
		}
		if (now_cmd == 0) {
			isstart = 1;
		}
		else {
			isstart = 0;
		}
		deal_with_command(argcnext, argnext, isstart, 1);
			
		exit(0);
	}
	if ( background == 1 ) {
		printf("[process id %d]\n", pid44);
		return ;
	}
	
	/* �����̵ȴ��ӽ��̽��� */
	if (waitpid (pid44, &status,0) == -1)
		printf("wait for child process error\n");
}


void deal_with_command(int argcount, char arglist[100][BUFSIZE], int isstart, int isend)
{
	int	flag = 0;       
	int	how = 0;        //���з�ʽ�����ض������û�У� 
	int	background = 0; //�����Ƿ���Ҫ�ں�̨���� 
	int	status;
	int	i;
	int	fd;
	int fd2;
	int fd3;
	char* argpp[argcount+1];
	char* argnext[argcount+1];
	char* file;
	pid_t pid;
 
	//get the command
	for (i=0; i < argcount; i++) {
		argpp[i] = (char *) arglist[i];
	}
	argpp[argcount] = NULL; //���һ���ǿ� 
 
	//whether background
	for (i=0; i < argcount; i++) {
		if (strncmp(argpp[i], "&",1) == 0) {
			if (i == argcount-1) {
				background = 1;
				argpp[argcount-1] = NULL;
				break;
			}
			else {
				printf("wrong command\n");
				return ;
			}
		}
	}
 
	for (i=0; argpp[i]!=NULL; i++) {
		if (strcmp(argpp[i], ">") == 0 ) {
			flag++;
			how = out_redirect;
			if (argpp[i+1] == NULL) 
				flag++;
		}
		if (strcmp(argpp[i],"<") == 0 ) {
			flag++;
			how = in_redirect;
			if(i == 0) 
				flag++;
		}
		if (strcmp(argpp[i],">>") == 0 ) {
			flag++;
			how = out_redict;
			if(i == 0) 
				flag++;
		}
	}
	
	 
	if (flag > 1) {
		printf("wrong command\n");
		return;
	}
 
	if (how == out_redirect || how == out_redict) {  //������ض������ 
		for (i=0; argpp[i] != NULL; i++) {
			if (strcmp(argpp[i],">")==0 || strcmp(argpp[i], ">>") == 0) {
				file   = argpp[i+1];
				argpp[i] = NULL;
			}			
		}
	}
 
	if (how == in_redirect) {    //���������ض��� 
		for (i=0; argpp[i] != NULL; i++) {
			if (strcmp (argpp[i],"<") == 0) {
				file   = argpp[i+1];
				argpp[i] = NULL;
			}
		}
	}
 	if ( (pid = fork()) < 0 ) {
		printf("fork not success\n");
		return;
	}
 
	switch(how) {
		case 0:
			//���ӽ���ִ�� 
			if (pid == 0) {
				if (!(find_command(argpp[0])) ) {
					printf("%s : command not found\n", argpp[0]);
					exit (0);
				}
				fd2 = open("/tmp/youdonotknowfile",O_RDONLY);
				if (isstart ==  0) {
					dup2(fd2,0);
				}
				fd3 = open("/tmp/youdonotknowfile1",
							O_WRONLY|O_CREAT|O_TRUNC,0644);
				if (isend == 0) {
					dup2(fd3, 1);
				}
				execvp(argpp[0], argpp);
				close(fd2);
				close(fd3);
				exit(0);
			}
			break;
		case 1: 
			//����ض��� 
			if (pid == 0) {
				if ( !(find_command(argpp[0])) ) {
					printf("%s : command not found\n",argpp[0]);
					exit(0);
				}
				fd2 = open("/tmp/youdonotknowfile",O_RDONLY);
				if (isstart ==  0) {
					dup2(fd2,0);
				}
				fd = open(file,O_RDWR|O_CREAT|O_TRUNC,0644);
				dup2(fd,1);
				execvp(argpp[0],argpp);
				exit(0);
			}
			break;
		case 2:
			//�����ض��� 
			if (pid == 0) {
				if ( !(find_command (argpp[0])) ) {
					printf("%s : command not found\n",argpp[0]);
					exit(0);
				}
				fd2 = open("/tmp/youdonotknowfile1",
							O_WRONLY|O_CREAT|O_TRUNC,0644);
				if (isend == 0) {
					dup2(fd2, 1);
				}
				fd = open(file,O_RDONLY);
				dup2(fd,0);  
				execvp(argpp[0],argpp);
				exit(0);
			}
			break;
		case 4:
			//׷������ض��� 
			if (pid == 0) {
				if ( !(find_command(argpp[0])) ) {
					printf("%s : command not found\n",argpp[0]);
					exit(0);
				}
				fd2 = open("/tmp/youdonotknowfile",O_RDONLY);
				if (isstart ==  0) {
					dup2(fd2,0);
				}
				fd = open(file, O_WRONLY|O_APPEND|O_CREAT|O_APPEND, 7777);
				dup2(fd,1);
				execvp(argpp[0],argpp);
				exit(0);
			}
			break;
		default:
			break;
	}
 
	//�ȴ��ӽ��̽����󷵻� 
	if (waitpid (pid, &status,0) == -1)
		printf("wait for child process error\n");
}
 
//�ҵ�ִ������ĳ��� 
int find_command (char *command)
{
	DIR*             dp;
	struct dirent*   dirp;
	char*			 path[] = { "./", "/bin", "/usr/bin", NULL};
	
	if (strcmp(argv[0], "history") == 0) {
		getHistory();
		return 1;
	}
 
	/* ʹ��ǰĿ¼�µĳ�����Ա����У�������"./fork"���Ա���ȷ���ͺ�ִ�� */
	if( strncmp(command,"./",2) == 0 )
		command = command + 2;
 
	/* �ֱ��ڵ�ǰĿ¼��/bin��/usr/binĿ¼����Ҫ��ִ�г��� */
	int i = 0;
	while (path[i] != NULL) {
		if ( (dp = opendir(path[i]) ) == NULL)  
			printf ("can not open /bin \n");
		while ( (dirp = readdir(dp)) != NULL) {
			if (strcmp(dirp->d_name,command) == 0) {
				closedir(dp);
				return 1;
			}
		}
		closedir (dp);
		i++;
	}
	return 0;
}

int dealCd(int argc) {
	int result = 1;
    if (argc != 2) {
        printf("the command is wrong:please input 'cd dir'");
    } 
	else {
        int ret = chdir(argv[1]);
        if (ret != 0) {
        	return 0;
		}
    }
	if (result) {
        char* res = getcwd(current, BUFSIZE);
        if (res == NULL) {
            printf("wrong path! please enter a existed directory");
        }
        return result;
    }
    return 0;
}

int getHistory() {
	int n;
	int i;
	if (argc == 2) {
		n = atoi(argv[1]);
	}
	else if (argc == 1) {
		n = 10;
	}
	else {
		printf("wrong input: please enter history [commandsNum]");
		return 0;
	}

    for (i = 1; i <= n && commandNum - i >= 0; i++) {
        printf("%d\t%s\n", commandNum - i, history[commandNum - i]);
    }
    return 0;
}


