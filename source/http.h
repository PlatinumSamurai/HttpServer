#ifndef HTTP_H_
#define HTTP_H_

#include <string.h>
#include "network.h"
#include "list.h"


typedef struct Http Http;
typedef struct HttpRequest HttpRequest;


Http* HttpInit(char *address);
void HttpFinalize(Http *http);
void HttpHandle(Http *http);
int HttpListen(Http *http);
void HttpParse(int conn, char *filename);


#endif  // HTTP_H_
