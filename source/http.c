#include "http.h"


#define METHOD_SIZE 16
#define PATH_SIZE   2048
#define PROTO_SIZE  16


typedef struct Http {
    char *host;
    List *files;
} Http;


typedef struct HttpRequest {
    char method[16];
    char path[2048];
    char protocol[16];
    uint8_t state;
    size_t index;
    char postData[2048];
} HttpRequest;


static HttpRequest RequestNew(void);
static void RequestParse(HttpRequest *request, char *buffer, size_t size);
static void RequestNull(HttpRequest *request);
static int HttpSwitch(Http *http, int conn, HttpRequest *request);
static void Http404(int conn);


Http* HttpInit(char *address) {
	Node *node = (Node *)malloc(sizeof(Node));
	node->key = (char *)malloc(2);
	node->value = (char *)malloc(sizeof("./Pages/index_2.html") + 1);
	strcpy(node->key, "/");
	strcpy(node->value, "./Pages/index_2.html");

	Http *http = (Http *)malloc(sizeof(Http));
	http->host = (char *)malloc(sizeof(char) * strlen(address) + 1);
	strcpy(http->host, address);
    http->files = ListInit(node);
    
    return http;
}


void HttpFinalize(Http *http) {
    free(http->host);
    ListFree(http->files);
	free(http);
}


void HttpHandle(Http *http) {
	char filename[256];
	int stdin = dup(0);
	int stdout = dup(1);
	int pipeFd[2];
	int bufSize;

	pipe(pipeFd);
	close(0);
	close(1);
	dup(pipeFd[0]);
	dup(pipeFd[1]);
	
	system("find ./Pages ./Styles -maxdepth 1 -type f");
	close(1);
	close(pipeFd[1]);
	dup(stdout);

	memset(filename, 0, sizeof(filename));
	while (scanf("%[^\n]", filename)) {
		if (getchar() == -1) {
			break;
		}
		Node *node = (Node *)malloc(sizeof(Node));
		int nameStartOffset = strrchr(filename, '/') + 1 - filename;
		int lenStr = strrchr(filename, '.') - filename;
		char *substr;

		if (!strcmp(filename + lenStr, ".css")) {
			lenStr = strlen(filename);
		}
		substr = (char *)malloc(lenStr + 1);
		memset(substr, 0, lenStr + 1);
		
		memcpy(substr, filename, lenStr);
		node->key = (char *)malloc(strlen(substr) - nameStartOffset + 2);
		node->value = (char *)malloc(strlen(filename) + 1);
		strcpy(node->key, substr + nameStartOffset - 1);
		strcpy(node->value, filename);

		free(substr);
		
		ListInsert(http->files, node);
	}

	close(0);
	close(pipeFd[0]);
	dup(stdin);
}


int HttpListen(Http *http) {
    int fd = Listen(http->host);

	if (fd < 0) {
		return -1;
	}
	while(1) {
        int conn = Accept(fd);
        if (conn < 0) {
            break;
		}
		HttpRequest req = RequestNew();
		while(1) {
			char buffer[BUFSIZ] = {0};
			int n = Recv(conn, buffer, BUFSIZ);
			if (n <= 0) {
				goto closeConn;
			}

            RequestParse(&req, buffer, n);
			if (n != BUFSIZ) {
                break;
			}
		}
		HttpSwitch(http, conn, &req);

closeConn:
		Close(conn);
	}

	Close(fd);
	return 0;
}

void HttpParse(int conn, char *filename) {
	char buffer[BUFSIZ] = "HTTP/1.1 200 OK\nContent-type: text/html\n\n";
	size_t readsize = strlen(buffer);

	Send(conn, buffer, readsize);
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		return;
	}
	while((readsize = fread(buffer, sizeof(char), BUFSIZ, file)) != 0) {
		Send(conn, buffer, readsize);
	}

	fclose(file);
}

static HttpRequest RequestNew(void) {
    return (HttpRequest) {
        .method = {0},
		.path = {0},
		.protocol = {0},
		.state = 0,
		.index = 0,
        .postData = {0},
    };
}


static void RequestParse(HttpRequest *request, char *buffer, size_t size) {
    for (size_t i = 0; i < size; ++i) {
		switch(request->state) {
			case 0:
				if (buffer[i] == ' ' || request->index == METHOD_SIZE-1) {
					request->method[request->index] = '\0';
					RequestNull(request);
					continue;
				}
				request->method[request->index] = buffer[i];
                break;

            case 1:
				if (buffer[i] == ' ' || request->index == PATH_SIZE-1) {
					request->path[request->index] = '\0';
					RequestNull(request);
                    continue;
				}
				request->path[request->index] = buffer[i];
    			break;

            case 2:
				if (buffer[i] == '\n' || request->index == PROTO_SIZE-1) {
					request->protocol[request->index] = '\0';
					RequestNull(request);
					continue;
				}
				request->protocol[request->index] = buffer[i];
    			break;

            default:
                return;
		}
		request->index += 1;
	}
}


void RequestNull(HttpRequest *request) {
	request->state += 1;
	request->index = 0;
}


int HttpSwitch(Http *http, int conn, HttpRequest *request) {	
	List *tmp = http->files;
	
	while (tmp) {
		if(!strcmp(tmp->node->key, request->path)) {
            HttpParse(conn, tmp->node->value);
            return 0;
        }
		tmp = tmp->next;
	}

	Http404(conn);

	return -1;
}


static void Http404(int conn) {
    char *header = "HTTP/1.1 404 Not Found\n\nnot found";
    size_t headsize = strlen(header);

	Send(conn, header, headsize);
}
