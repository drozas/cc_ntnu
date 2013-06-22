SOURCE=src
OBJECT=obj
BINARY=bin
WORK=work
INCLUDE=include
CC=cc
vslc: parser.tab.h scanner.yy.c main.o
	${CC} ${CFLAGS} -I${INCLUDE} -I${WORK} -o ${BINARY}/vslc ${WORK}/*.c ${OBJECT}/*.o
%.o:
	${CC} -std=gnu99 ${CFLAGS} -I${INCLUDE} -I${WORK} -o ${OBJECT}/$*.o -c ${SOURCE}/$*.c
%.tab.h:
	bison -d ${SOURCE}/$*.y
	mv $*.tab.* ${WORK}/
%.yy.c:
	lex -o${WORK}/$*.yy.c ${SOURCE}/$*.l
clean:
	rm -f ${BINARY}/* ${OBJECT}/* ${WORK}/*
