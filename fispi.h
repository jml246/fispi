#ifndef _fispi_h_
#define _fispi_h_ 1

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define _USE_FIS_DEBUG_ 1
#define FISOK() ok( ) ;
#ifdef _USE_FIS_DEBUG_
#define FISDEBUG(...) printf( __VA_ARGS__) ;
#else
#define FISDEBUG(...) ;
#endif

#define fdmax(x,y) ((x) > (y) ? (x) : (y))

#define MAXFD           5
#define MAXBUFSIZE      250

#define FISCOREPORT     5000
#define FISIOPORT 	5001
#define FISALGPORT 	5002
#define FISCOMPORT 	5003

#define FISCOREID	0 
#define FISIOID		1
#define FISALGID	2	
#define FICOMID		3	

struct fis_comm_context {
	int     FD[MAXFD];
	int     bufpos[MAXFD];
	char    buf[MAXFD][MAXBUFSIZE];
};

int set_timer (struct timeval* result,struct timeval *x,int i){
       /* Perform the carry for the later subtraction by updating y. */
	struct timeval y;
	gettimeofday( &y, NULL );
       if (x->tv_usec < y.tv_usec) {
         int nsec = (y.tv_usec - x->tv_usec) / 1000000 + 1;
         y.tv_usec -= 1000000 * nsec;
         y.tv_sec += nsec;
       }
       if (x->tv_usec - y.tv_usec > 1000000) {
         int nsec = (x->tv_usec - y.tv_usec) / 1000000;
         y.tv_usec += 1000000 * nsec;
         y.tv_sec -= nsec;
       }
     
       /* Compute the time remaining to wait.
          tv_usec is certainly positive. */
       result->tv_sec = x->tv_sec - y.tv_sec;
       result->tv_usec = x->tv_usec - y.tv_usec;
     
       /* Return 1 if result is negative. */
	if (x->tv_sec < y.tv_sec){
		x->tv_sec = x->tv_sec +i;
		result->tv_sec = x->tv_sec - y.tv_sec;
       		result->tv_usec = x->tv_usec - y.tv_usec;
		return(1);
	}
	return(0);
     }

char* strtok_fis(char *s, const char *delimiters, char **lasts) {
        char *sbegin = NULL;
        char *send   = NULL;
        int b = 1;
        sbegin = s ? s : *lasts;
        sbegin += strspn(sbegin, delimiters);

        if (*sbegin == '\0') {
                *lasts = sbegin;
                return NULL;
        }

        if (*sbegin == '{') {
                send=++sbegin;
                while(*send!='\0'){
                        if( *send == '{' ) b++;
                        if( *send == '}' ) b--;
                        if( b == 0 )  break;
                        send++;
                }
        }
        else{
                send = sbegin + strcspn(sbegin, delimiters);
        }

        if(sbegin == send) {
                *lasts = sbegin;
                return NULL;
        }

        if (*send != '\0') *send++ = '\0';
        *lasts = send;

        return sbegin;
}

int fis_tokenize(char *s, char *arg[], int maxtoken){
        const char *sep = " \t\n\r";
        char *lasts;
        int i=0;

        if (maxtoken <= 1) {
                return(0);
        }

        arg[0] = strtok_fis(s, sep, &lasts);

        if (arg[0] == NULL) {
                 return(0);
        }

        for(i=1; i<maxtoken; i++) {
                arg[i] = strtok_fis(NULL, sep, &lasts);
                if (arg[i] == NULL) {
                        return(i);
                }
        }

        return(i);
}

int fis_comm_open(struct fis_comm_context *ctx, int port){
	int i;
	struct sockaddr_in s_addr;
        int yes = 1;


	/* invalidate all filedescriptors */
	for (i=0;i<MAXFD;i++) {
		ctx->FD[i]=-1;
	}

	/* prepare FD[0] to accept new connections on the given tcp port */  

        if((ctx->FD[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                FISDEBUG("failed to create socket\n")
                return(-1);
        }
        if(setsockopt(ctx->FD[0], SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
                FISDEBUG("setsockop failed\n")
                ctx->FD[0]=-1;
                return(-1);
        }
	memset( &s_addr, 0, sizeof(s_addr) );
        s_addr.sin_family = AF_INET;
        s_addr.sin_addr.s_addr = INADDR_ANY;
        s_addr.sin_port = htons(port);

        if(bind(ctx->FD[0], (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1){
                FISDEBUG("bind failed\n")
                ctx->FD[0]=-1;
                return(-1);
        }
        if(listen(ctx->FD[0], 2) == -1){
                FISDEBUG("listen failed\n")
                ctx->FD[0]=-1;
                return(-1);
        }
	
	return(0);
}

int fis_comm_close(struct fis_comm_context *ctx){
	int i;

	/* invalidate all filedescriptors */
	for (i=0;i<MAXFD;i++) {
		if (ctx->FD[i]!=-1) {
			close(ctx->FD[i]);
			ctx->FD[i]=-1;
		}
	}
	return(0);
}

int fis_comm_select(struct fis_comm_context *ctx, struct timeval* t){
	int i;
	fd_set fdset;
	int m = 0;
	int fd = -1;
        struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(clientaddr);

	FD_ZERO(&fdset);
        for (i=0;i<MAXFD;i++){
        	if(ctx->FD[i]>=0){
        		FD_SET(ctx->FD[i],&fdset);
        		m=fdmax(m,ctx->FD[i]);
        	}
        }

        select(m+1,&fdset,NULL,NULL,t);
	
	/* socket accepting connections */
	if (ctx->FD[0]>=0 && FD_ISSET(ctx->FD[0], &fdset)) {
		// queue new client
		if((fd = accept(ctx->FD[0], (struct sockaddr *)&clientaddr, &addrlen)) == -1) {
                	FISDEBUG("accept failed\n")
			return(-2);
                }
                else{
        		for (i=0;i<MAXFD;i++){
                                if (ctx->FD[i]<0){
                                        ctx->FD[i]=fd;
					return(0);
                                }
                        }
			close(fd);
                        FISDEBUG("no socket available to handle new client\n")
			return(0);
		}
		return(-2);
	}

	/* all connected sockets */
        for (i=1;i<MAXFD;i++){
		if (ctx->FD[i]>=0 && FD_ISSET(ctx->FD[i], &fdset)) 
			return(i);
	}

	/* no fd, must be timeout */
	return(-1);
}

int fis_comm_send(struct fis_comm_context *ctx, int dst,char *line){
	int i;
	if ((dst>MAXFD) || (dst < 0)) return(-1); 
	if (dst == 0) { // broadcast
		for (i=1;i<MAXFD;i++){
                	if (ctx->FD[i]>=0){
				if(send(ctx->FD[i],line, strlen(line),0)<0){
        				FISDEBUG("send error\n")
					return(-1);
				}
                        }
                }
		return(1);
	}
	
	if (ctx->FD[dst]!=-1){ // unicast
		if(send(ctx->FD[dst],line, strlen(line),0)<0){
        		FISDEBUG("send error\n")
			return(-1);
		}
		else {
			return(1);
		}
	}

	return(-1);
}

int fis_comm_read(struct fis_comm_context *ctx, int i){
	int r;
	unsigned char c;

	r = read(ctx->FD[i],&c,1);

        if (r <= 0){
        	FISDEBUG("client socket read error or socket closed\n")
                close(ctx->FD[i]);
                ctx->FD[i]=-1;
		return(-1);
	}
        else{
        	(ctx->bufpos[i]) += r;
        	(ctx->buf[i][(ctx->bufpos[i])-1]) = c;
        	if((ctx->bufpos[i]>0) && (ctx->buf[i][ctx->bufpos[i]-1]=='\n')) {
        		ctx->buf[i][ctx->bufpos[i]-1]='\0';
        		ctx->bufpos[i]=0;
			return(1);
		}
                if(ctx->bufpos[i]>=MAXBUFSIZE) ctx->bufpos[i]=0;
        }
	return(0);
}
int fis_comm_connect(struct fis_comm_context *ctx, int port){
	int fd = -1;
	int i;
	char *host = "127.0.0.1";
	struct sockaddr_in l_addr;
	struct sockaddr_in p_addr;

        if ((fd = socket(AF_INET, SOCK_STREAM,0))<0){
        	FISDEBUG("allocate socket (errno: %d %s)\n",errno,strerror(errno))
        	return(-1);
        }


        memset( &l_addr, 0, sizeof(l_addr) );
        l_addr.sin_family       = AF_INET;
        l_addr.sin_addr.s_addr  = INADDR_ANY;
        l_addr.sin_port         = htons(0);
        if (bind(fd,(struct sockaddr*)&l_addr,sizeof(l_addr))<0){
        	FISDEBUG("bind (errno: %d %s)\n",errno,strerror(errno))
        	close(fd);
        	return(-1);
        }

       	memset( &p_addr, 0, sizeof(p_addr) );
        p_addr.sin_family       = AF_INET;
        if ((inet_aton(host, &(p_addr.sin_addr)))<=0) {
        	FISDEBUG("invalid ip address (errno: %d %s)\n",errno,strerror(errno))
        	close(fd);
        	return(-1);
        }
        p_addr.sin_port         = htons(port);

        if (connect(fd,(struct sockaddr*) &p_addr,sizeof(p_addr)) < 0) {
        	FISDEBUG("connect (errno: %d %s)\n",errno,strerror(errno))
        	close(fd);
                return(-1);
        }

	for (i=1;i<MAXFD;i++){
                if (ctx->FD[i]==-1){
			ctx->FD[i]=fd;
			ctx->bufpos[i]=0;
			return(i);
		}
        }
	
	close(fd);

	return(-1);
}

#endif
