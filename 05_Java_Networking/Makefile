
JAVASRC      = chatter.java client.java gui.java server.java
SOURCES      = ${JAVASRC} Makefile README
MAINCLASS    = chatter
CLASSES      = ${JAVASRC:.java=.class}
INNERCLASSES = chatter\$$options.class \
               client\$$receiver.class client\$$sender.class \
               client\$$listener.class client\$$client_start.class \
               gui\$$confirm.class gui\$$gui_start.class \
               gui\$$copyfield.class gui\$$gui_listen.class \
               server\$$receiver.class server\$$sender.class \
               server\$$reader.class server\$$writer.class \
			   server\$$server_listen.class \
               server\$$userManager.class \
               server\$$queueManager.class server\$$verify_user.class \
               server\$$client_listener.class \
               server\$$client_writer.class
JARCLASSES   = ${CLASSES} ${INNERCLASSES}
JARFILE      = chatter
LISTING      = Listing.ps

all : ${JARFILE}

${JARFILE} : ${CLASSES}
	echo Main-class: ${MAINCLASS} >Manifest
	jar cvfm ${JARFILE} Manifest ${JARCLASSES}
	- rm Manifest
	chmod +x ${JARFILE}

%.class : %.java
	javac $<

lis : ${SOURCES} all
	mkpspdf ${LISTING} ${SOURCES}

clean :
	- rm ${JARCLASSES}

spotless : clean
	- rm ${JARFILE}

ci : ${SOURCES}
	checksource ${SOURCES}
	cid + ${SOURCES}

submit : ${SOURCES}
	submit cmps109-wm.s12 asg5 ${SOURCES}

again :
	${MAKE} spotless ci all lis

	

