SOURCE=src
OBJECT=obj
BINARY=bin
WORK=work
INCLUDE=include
CC=cc
CFLAGS= -g
vslc: parser.tab.h scanner.yy.c main.o scope.o tree.o symtab.o
	${CC} ${CFLAGS} -I${INCLUDE} -I${WORK} -o ${BINARY}/vslc ${WORK}/*.c ${OBJECT}/*.o
%.o:
	${CC} -std=gnu99 ${CFLAGS} -I${INCLUDE} -I${WORK} -o ${OBJECT}/$*.o -c ${SOURCE}/$*.c
%.tab.h:
	bison -d ${SOURCE}/$*.y
	mv $*.tab.* ${WORK}/
%.yy.c:
	lex -o${WORK}/$*.yy.c ${SOURCE}/$*.l
clean:
	rm -f ${BINARY}/* ${OBJECT}/* ${WORK}/* dummy
dummy: clean scope.o
	${CC} -std=gnu99 -o dummy ${CFLAGS} -I${INCLUDE} dummy.c ${OBJECT}/scope.o
