/* qorg's finger daemon.

 * This is just a meme, it does not try to be RFC-1288 compliant or
 * anything like that. It just tries to be something fun for me. So
 * don't use this thing unironically. Or do it, if you get this code,
 * you can do whatever you want with it.
 *
 * this daemon ONLY tries to read the ~/.plan file from a user and
 * return it to the client. That's it. The finger protocol does more
 * than that.  Now that I think. I think i could have written this in
 * a simple shell script. But well, It wouldn't be the 80s that way
 * :-)
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>
#include <fcntl.h>
#include <syslog.h>
#include <stdlib.h>
#include <signal.h>

int sockfd, clientfd;


void sigterm_handle(int signal)
{
	syslog(LOG_NOTICE,"SIGINT recieved. Dying");
	close(sockfd);
	close(clientfd);
	exit(EXIT_SUCCESS);

}


int
read_from_sock(int sockfd, char *user, size_t size)
{
	char buf[256];
	if(read(sockfd,buf,256) == -1) {
		syslog(LOG_CRIT,"Error reading from socket: %s\n"
			,strerror(errno));
		return -1;
	} else
		strncpy(user,buf,size); /* size shouldn't be >256 */
	return 0;
}

int
logger(struct sockaddr_in client, char *user)
{
	char *client_addr = inet_ntoa(client.sin_addr);
	user[strcspn(user, "\n")] = 0;

	if(strlen(user) == 0)
		syslog(LOG_INFO,"Sending default plan to %s",client_addr);
	else
		syslog(LOG_INFO,"Sending %s's plan to %s",user,client_addr);
	return 0;
}

int
write_plan(const char *user, int clientfd)
{
	/* Check if string is empty */
	if(strlen(user) == 0) {
		char *s = "This is the default fingerd message, you gave no user\n";
		write(clientfd,s,strlen(s));
	} else {
		struct passwd *user_info = getpwnam(user);
		if(user_info == NULL) {
			return -1;
		}
		char plan_buf[8192];
		char plan_file[256]; /* From bell labs */
		char *user_homedir = user_info->pw_dir;
		int read_bytes;

		snprintf(plan_file,256,"%s/.plan",user_homedir);
		int plan_fd = open(plan_file,O_RDONLY); /* I wouldn't change the
										 * O_RDONLY thing */
		if(plan_fd == -1) {
			syslog(LOG_CRIT,"Error opening file: %s",strerror(errno));
			return -1;
		}
		while((read_bytes = read(plan_fd, plan_buf, 8192)) > 0)
			write(clientfd,plan_buf,read_bytes);
		memset(plan_buf,0,812);
		close(plan_fd);
	}
	return 0;
}

int
main(int argc, char **argv)
{
	/* Initiate logger */
	openlog("fingerd", LOG_PID|LOG_PERROR,0);
	/* Signal handler */
	signal(SIGINT,sigterm_handle);
	
	sockfd = socket(AF_INET,SOCK_STREAM,0); /* TODO: IPv6 support */
	struct sockaddr_in addr, client_addr;
	char user[256];
	socklen_t len = sizeof(addr);
	if(sockfd == -1) {
		syslog(LOG_CRIT,"Error creating socket: %s\n",strerror(errno));
		return -1;
	}

	char *bind_address = "0.0.0.0";
	int port = 79;
	int c;
	while((c = getopt(argc, argv,"p:a:")) != -1) {
		switch(c) {
		case 'p':
			port = atoi(optarg);
			break;
		case 'a':
			bind_address = optarg;
			break;
		}

	}
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, bind_address, &addr.sin_addr);
	
	if(bind(sockfd,(struct sockaddr*)&addr,len) == -1) {
		syslog(LOG_CRIT,"Error binding: %s\n",strerror(errno));
		return -1;
	}
	if(listen(sockfd,5) == -1) {
		syslog(LOG_CRIT,"Error on listen(): %s\n",strerror(errno));
		return -1;
	}
	syslog(1,"Binded fingerd in %s:%i",bind_address,port);
	while(1) {
		clientfd = accept(sockfd,(struct sockaddr*)&client_addr,&len);
		if(clientfd == -1) {
			syslog(LOG_CRIT,"Error on accept(): %s\n",strerror(errno));
			return -1;
		}
		
		read_from_sock(clientfd,user,256);
		/* Some clients do whathever the fuck they want */
		
		if(user[0] == ' ')
			memmove(user, user+1, strlen(user));

		logger(client_addr,user);
		write_plan(user,clientfd);
		close(clientfd);
	}
	
	close(sockfd);
	return 0;
}

