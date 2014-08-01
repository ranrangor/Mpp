CC=gcc 
CFLAGS= -I ./include  -g -Wall
LDFLAGS= 

SOURCE=./source
INCLUDE=./include


EXEC=./app/mpp
SRCS=./source/main.c ./source/console.c ./source/file.c\
	 ./source/playlist.c ./source/play.c ./source/parsecmd.c\
	 ./source/utils.c ./source/linenoise.c

OBJS=${SRCS:.c=.o}

#ALLSRCS=main.c console.c file.c\
	 playlist.c play.c parsecmd.c


#SRCS=$(addpreffix ${SOURCE}/,${ALLSRCS})




.PHONY:all
.PHONY:clean
.PHONY:player
.PHONY:install
.PHONY:uninstall


all:${OBJS}
	$(CC) $(CFLAGS) $^ -o $(EXEC) $(LDFLAGS)


#${OBJS}:${INCLUDE}/config.h
#	${CC} -c ${CFLAGS} -o $@ $<  


#${SOURCE}/main.o:${SOURCE}/main.c ${INCLUDE}/config.h
${SOURCE}/main.o:${SOURCE}/main.c ${INCLUDE}/config.h
	${CC} -c ${CFLAGS} -o $@ $<  

${SOURCE}/console.o:${SOURCE}/console.c ${INCLUDE}/console.h ${INCLUDE}/config.h
	${CC} -c ${CFLAGS} -o $@ $<  

${SOURCE}/file.o:${SOURCE}/file.c ${INCLUDE}/file.h ${INCLUDE}/config.h
	${CC} -c ${CFLAGS} -o $@ $<  

${SOURCE}/playlist.o:${SOURCE}/playlist.c ${INCLUDE}/playlist.h ${INCLUDE}/config.h
	${CC} -c ${CFLAGS} -o $@ $<  


${SOURCE}/play.o:${SOURCE}/play.c ${INCLUDE}/play.h ${INCLUDE}/config.h
	${CC} -c ${CFLAGS} -o $@ $<  

${SOURCE}/parsecmd.o:${SOURCE}/parsecmd.c ${INCLUDE}/parsecmd.h ${INCLUDE}/config.h
	${CC} -c ${CFLAGS} -o $@ $<  

${SOURCE}/utils.o:${SOURCE}/utils.c ${INCLUDE}/utils.h
	${CC} -c ${CFLAGS} -o $@ $<  




LDFLAGS+= -lmad -lasound


player:
	${CC} ${CFLAGS} ${SOURCE}/player.c -o ${SOURCE}/player  ${LDFLAGS}


install:
	cp ${SOURCE}/player /usr/local/bin
	cp ${EXEC} /usr/local/bin


uninstall:
	-rm /usr/local/bin/player
	-rm /usr/local/bin/mpp

clean:
	-rm  ${OBJS} ${EXEC}
