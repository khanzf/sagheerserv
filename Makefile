OBJS = library.o httpd.o config.o modules.o parse.o response.o
HDRS = library.h httpd.h config.h modules.h parse.h response.h
CFLS = library.c httpd.c config.c modules.c parse.c response.c
#GCC  = /usr/bin/gcc 

CFLAGS = -Wall -Werror -ggdb
CLINKS = -rdynamic -ldl

httpd: $(OBJS) main.o
	gcc $(CFLAGS) $(CLINKS) $(OBJS) main.o -o httpd 
%.o: %.c %.h 
	gcc $(CFLAGS) -c $< 
main.o: main.c
	gcc $(CFLAGS) -c main.c

.PHONY : clean

clean:
	rm -f httpd *.o core *.core *.so
cgi:
	gcc -fPIC $(CFLAGS) -c mod_cgi.c 
	gcc -shared -Wl,-soname,mod_cgi.so -o mod_cgi.so mod_cgi.o -lc -rdynamic
userpath:
	gcc -fPIC $(CFLAGS) -c mod_userpath.c
	gcc -shared -Wl,-soname,mod_userpath.so -o mod_userpath.so mod_userpath.o -lc -rdynamic
blank:
	gcc -fPIC $(CFLAGS) -c mod_blank.c
	gcc -shared -Wl,-soname,mod_blank.so -o mod_blank.so mod_blank.o -lc -rdynamic
