include Make.defines

PROGS =	 client server

OPTIONS = -DUNIX  -DANSI


COBJECTS =	DieWithError.o 
CSOURCES =	DieWithError.c 

CPLUSOBJECTS = 

COMMONSOURCES =

CPLUSSOURCES =

all:	${PROGS}


client:		UDPEchoClient2.o $(CPLUSOBJECTS) $(COBJECTS) $(LIBS) $(COMMONSOURCES) $(SOURCES)
		${CC} ${LINKOPTIONS}  $@ UDPEchoClient2.o $(CPLUSOBJECTS) $(COBJECTS) $(LIBS) $(LINKFLAGS)

server:		UDPEchoServer.o $(CPLUSOBJECTS) $(COBJECTS)
		${CC} ${LINKOPTIONS} $@ UDPEchoServer.o $(CPLUSOBJECTS) $(COBJECTS) $(LIBS) $(LINKFLAGS)


.cc.o:	$(HEADERS)
	$(CPLUS) $(CPLUSFLAGS) $(OPTIONS) $<

.c.o:	$(HEADERS)
	$(CC) $(CFLAGS) $(OPTIONS) $<



backup:
	rm -f UDPEcho2.tar
	rm -f UDPEcho2.tar.gz
	tar -cf UDPEcho2.tar *
	gzip -f UDPEcho2.tar

clean:
		rm -f ${PROGS} ${CLEANFILES}

depend:
		makedepend UDPEchoClient1.c UDPEchoClient2.c UDPEchoServer.c $(CFLAGS) $(HEADERS) $(SOURCES) $(COMMONSOURCES) $(CSOURCES)
#		mkdep $(CFLAGS) $(HEADERS) $(SOURCES) $(COMMONSOURCES) $(CSOURCES)

# DO NOT DELETE
