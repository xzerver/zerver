CPPS = request_module.cc \
       response_module.cc \
       tcp_module.cc \
       simple_module.cc \
       fork_module.cc \
       join_module.cc
 
default:$(CPPS)
	for CPP in $(CPPS) ; do \
		echo g++ -c -Wall -Werror -I../thirdparty/ $$CPP; \
		g++ -c -Wall -Werror -I../thirdparty/ $$CPP; \
	done
