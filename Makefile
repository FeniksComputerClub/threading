# List of all programs (that we want compiled).
PROGRAMS = start_thread use_lock condition_variable condition_variable_any

CXXFLAGS=-g -pthread
LDFLAGS=-pthread
LIBS=-lpthread

# The default target to make:
all: $(PROGRAMS)

.SUFFIXES:              # Delete the default suffixes
.SUFFIXES: .cxx .o .h	# Define our suffix list

%: %.o
	g++ $(LDFLAGS) $^ -o $@ $(LIBS)

%.o: %.cxx
	g++ -std=c++11 $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(PROGRAMS)
