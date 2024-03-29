#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include <iostream>
#include <string>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void* request_handler(void * arg );
void send_data(FILE *fp, char *ct, char * file_name );
char * content_type(char * file );
void send_error(FILE *fp );
void error_handling(char * message );

void Find ( char * str, const char *old_str)
{

	int size= strlen(old_str);
	int i,j,cnt;
	
	for (  i = 0; i <= strlen(str); i++ )
	{
		cnt = 0;
		if ( str[i] == old_str[cnt] )
		{
			for (  j = 0; j <= size-1; j++ )
			{
				if ( str[i+j] != old_str[j] ) break;
				str[i+j] = 'A';
			}
			
			if ( j != size ) continue;
			else {
				str[i] = 'T';
				str[i+size-1] = 'T';
			}
			
			
		}
	}

}


void parser ( char * str )
{
	char temp[strlen(str)+1];
	strcpy(temp, str );
	
	Find ( temp, "<br>");
	
	
	strcpy( str,  temp);
}

int main ( int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_size;
	char buf[BUF_SIZE];
	pthread_t t_id;

	if ( argc != 2 ) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0 );
	memset(&serv_adr, 0, sizeof(serv_adr) );
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons ( atoi ( argv[1] ) );
	
	if ( bind ( serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		printf("bind() error");
	
	if ( listen ( serv_sock, 20 ) == -1 )
		printf("listen() error");

	while(1)
	{
		clnt_adr_size = sizeof (clnt_adr );
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr,(socklen_t *__restrict) &clnt_adr_size);
		printf("Connection Request : %s:%d\n",
			inet_ntoa(clnt_adr.sin_addr), ntohs( clnt_adr.sin_port));

		pthread_create (&t_id, NULL, request_handler, &clnt_sock );
		pthread_detach(t_id);
	
	}

	close( serv_sock);
	return 0;
}

void* request_handler ( void * arg )
{

	int clnt_sock = *((int*)arg );
	char req_line[SMALL_BUF] ;
	FILE * clnt_read;
	FILE * clnt_write;

	char method[10];
	char ct[15];
	char file_name[30];

	void * ret;

	clnt_read= fdopen(clnt_sock,"r");
	clnt_write = fdopen(dup(clnt_sock), "w");
	fgets(req_line, SMALL_BUF, clnt_read );

	if ( strstr(req_line, "HTTP/") == NULL )
	{
		// send_error (clnt_write ) ;
		printf ("request HTTP/ error\n" );
		fclose ( clnt_read );
		fclose ( clnt_write );
		return ret;
	}

	strcpy (method, strtok(req_line, " /"));
	strcpy (file_name, strtok(NULL," /"));
	strcpy (ct, content_type(file_name));
	if ( strcmp(method, "GET") != 0 )
	{
		// send_error(clnt_write);
		printf ("GET not found!\n");
		fclose(clnt_read);
		fclose(clnt_write);
		return ret;
	}
	fclose(clnt_read);
	printf("send data\n");	
	send_data(clnt_write, ct, file_name );

	return ret;
}


char * content_type ( char * file )
{
	char *ret = new char[20];
	char extension[SMALL_BUF];
	char file_name[SMALL_BUF];
	strcpy(file_name, file );
	strtok(file_name, ".");
	strcpy(extension, strtok(NULL,"."));
	if (!strcmp(extension, "html")||!strcmp(extension, "htm") )
		strcpy(ret,"text/html");
	else
		strcpy(ret,"text/plain");

	return ret;
}

void send_data ( FILE *fp, char * ct, char * file_name )
{
	char protocol[] = "HTTP/1.0 200 OK\r\n";
	char server[] = "Server:Linux Web Server\r\n";
	char cnt_len[] = "Content-length:2048\r\n";
	char cnt_type[SMALL_BUF];
	char buf[BUF_SIZE];
	FILE *send_file;
	

	sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct );
	send_file = fopen(file_name, "r");
	printf("filename:%s\n", file_name );
	if (send_file == NULL) { printf("fp error\n"); return; }
	
	fputs(protocol, fp );
	fputs(server, fp);
	fputs(cnt_len, fp );
	fputs(cnt_type, fp );

	
	long size;
	fseek ( send_file, 0, SEEK_END );
	size = ftell( send_file );
	fseek ( send_file, 0, SEEK_SET );
	
	char * temp = new char[size];
	
	while( fgets (buf, size, send_file) != NULL) strcat (temp, buf );


	parser ( temp );
	
	
	
	fputs(temp, fp );
	fflush(fp );
	
	

/*
	while ( fgets (buf, BUF_SIZE, send_file ) != NULL )
	{
		fputs(buf, fp );
		fflush(fp);
	}
	fflush(fp);
*/
	
	
	fclose(fp);

}

