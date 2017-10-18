PREFIX=${PARSECDIR}/pkgs/kernels/streamcluster/inst/${PARSECPLAT}

TARGET_C = sc_cpu

TARGET_O = sc_omp

ifdef version
  ifeq "$(version)" "parallel"
    CXXFLAGS :=	$(CXXFLAGS) -DENABLE_THREADS -pthread
  endif
endif

all: cpu omp

cpu:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -lnuma streamcluster_original.cpp -o $(TARGET_C) $(LIBS) -DENABLE_THREADS -pthread

omp:
	#g++ -O3 -fopenmp -o $(TARGET_O) streamcluster_omp.cpp -lnuma
	xlc++ -O3 -fopenmp -lnuma -o $(TARGET_O) streamcluster_omp.cpp -qhot -qsmp=omp -qarch=pwr8 -qtune=pwr8

clean:
	rm -f *.o *~ *.txt sc_cpu sc_omp
	
