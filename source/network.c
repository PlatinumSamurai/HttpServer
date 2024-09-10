#include "network.h"


typedef enum error_t {
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    CONNECT_ERR,
    ACCEPT_ERR,
    SEND_ERR,
    RECV_ERR
} error_t;


int Listen(char *address) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    char *ip = (char *)malloc(16);
    char *port = (char *)malloc(6);
    struct sockaddr_in addr;
	int ipLen = strrchr(address, ':') - address;

	memset(ip, 0, 16);
	memset(port, 0, 6);

	memcpy(ip, address, ipLen);
	strcpy(port, address + ipLen + 1);

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    if(fd == -1) {
        perror("1");
        return LISTEN_ERR;
    }

    addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(ip);

	free(ip);
	free(port);

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		return BIND_ERR;
	}
	if (listen(fd, SOMAXCONN) != 0) {
		return LISTEN_ERR;
	}

	return fd;
}


int Accept(int listenFd) {
    int fd;
    time_t start = clock();

    do {
        if((clock() - start) / CLOCKS_PER_SEC > 30) {
            break;
        }   
        fcntl(listenFd, F_SETFL, O_NONBLOCK);
        fd = accept(listenFd, NULL, NULL);
		// printf("Tpecca\n");
    } while(fd == -1 && errno == EAGAIN);

    if(fd == -1) {
        perror("2");
    }
    
    return fd;
}


int Connect(char *address) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
	char *ip = (char *)malloc(16);
    char *port = (char *)malloc(6);
    struct sockaddr_in addr;
	int ipLen = strrchr(address, ':') - address;

	memset(ip, 0, 16);
	memset(port, 0, 6);

	memcpy(ip, address, ipLen);
	strcpy(port, address + ipLen + 1);
    
    if (fd < 0) {
		return SOCKET_ERR;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
    inet_aton(ip, &addr.sin_addr);
	
	free(ip);
	free(port);
	
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("3");
        return CONNECT_ERR;
	}

	return fd;
}


int Send(int fd, char *buffer, size_t size) {
#ifdef DEBUG
    printf("===================================================="
		   "===================================================="
		   "============================\ninside function Send. "
		   "Buffer to send is \n %s \n\n", buffer);
#endif // DEBUG
	int bytes = send(fd, buffer, (int)size, 0);

    if(bytes == -1) {
        perror("4");
        return SEND_ERR;
    }

    return bytes;
}


int Recv(int fd, char *buffer, size_t size) {
	int bytes = recv(fd, buffer, (int)size, 0);

    if(bytes == -1) {
        perror("5");
        return RECV_ERR;
    }

    return bytes;
}


int Close(int fd) {
    close(fd);

    return 0;
}
