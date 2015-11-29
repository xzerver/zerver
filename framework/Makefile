CPPS = fsm.cc \
       module.cc \
       tcp_server.cc
 
default:$(CPPS)
	for CPP in $(CPPS) ; do \
		echo g++ -c $$CPP; \
		g++ -c $$CPP; \
	done
