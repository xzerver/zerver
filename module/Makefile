CPPS = request_module.cc \
       response_module.cc \
       tcp_module.cc \
       simple_module.cc \
       fork_module.cc \
       join_module.cc
 
default:$(CPPS)
	for CPP in $(CPPS) ; do \
		echo g++ -c $$CPP; \
		g++ -c $$CPP; \
	done
