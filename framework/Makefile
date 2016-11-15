CPPS = fsm.cc \
       data.cc \
       module.cc \
       tcp_server.cc 
 
default:$(CPPS)
	for CPP in $(CPPS) ; do \
		g++ -c -Wall -Werror $$CPP; \
	done
