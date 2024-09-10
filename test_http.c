#include <stdio.h>
#include "./source/http.h"


extern Http* HttpInit();
extern void HttpHandle();
extern int HttpListen();
extern void HttpFinalize();


int main(void) {
	Http *serve = HttpInit("127.0.0.1:11111");
	
	HttpHandle(serve);
	HttpListen(serve);
	HttpFinalize(serve);

	return 0;
}
