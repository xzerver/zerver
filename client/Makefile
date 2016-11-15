CPPS = tcp_client_pool.cc \
			 load_balancer.cc
 
default:$(CPPS)
	for CPP in $(CPPS) ; do \
		echo g++ -c $$CPP; \
		g++ -c -I../thirdparty/ $$CPP; \
	done
