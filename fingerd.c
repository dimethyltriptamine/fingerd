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

int sockfd, clientfd;

#define DEBUG 1

/* TODO: Use pointers to char instead of arrays lol */

int
read_from_sock(int sockfd, char *user, size_t size)
{
	char buf[256];
	if(read(sockfd,buf,256) == -1) {
		fprintf(stderr,"Error reading from socket: %s\n"
			,strerror(errno));
		return -1;
	} else
		strncpy(user,buf,size); /* size shouldn't be >256 */
	return 0;
}

int
write_plan(const char *user, int clientfd)
{
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
		fprintf(stderr,"Error opening file: %s",strerror(errno));
		return -1;
	}
	while((read_bytes = read(plan_fd, plan_buf, 8192)) > 0)
		write(clientfd,plan_buf,read_bytes);
	memset(plan_buf,0,812);
	return 0;
}

int
main(void)
{
	sockfd = socket(AF_INET,SOCK_STREAM,0); /* TODO: IPv6 support */
	struct sockaddr_in addr;
	char user[256];
	socklen_t len = sizeof(addr);
	if(sockfd == -1) {
		fprintf(stderr,"Error creating socket: %s\n",strerror(errno));
		return -1;
	}
	/* TODO: ask user where to bind using a config file or something
	 * like that */
	
	addr.sin_port = htons(79);
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	
	if(bind(sockfd,(struct sockaddr*)&addr,len) == -1) {
		fprintf(stderr,"Error binding: %s\n",strerror(errno));
		return -1;
	}
	if(listen(sockfd,5) == -1) {
		fprintf(stderr,"Error on listen(): %s\n",strerror(errno));
		return -1;
	}
	while(1) {
		clientfd = accept(sockfd,(struct sockaddr*)&addr,&len);
		if(clientfd == -1) {
			fprintf(stderr,"Error on accept(): %s\n",strerror(errno));
			return -1;
		}

		read_from_sock(clientfd,user,256);
		write_plan(user,clientfd);
		close(clientfd);
	}
	
	close(sockfd);
	return 0;
}

