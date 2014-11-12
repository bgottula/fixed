# object files used to link all binaries
OBJ_COMMON:=obj/ComplexFixedPoint.o obj/FixedPoint.o
# object files used to link bin/test
OBJ_TEST:=$(OBJ_COMMON) obj/unit/unit.o obj/unit/FixedPointTest.o obj/unit/ComplexFixedPointTest.o

# libraries used to link bin/test
LINK_TEST:=

# all header files
HEADERS:=include/*.h

# g++ options
GCC_FLAGS:=-std=gnu++0x -Wall -Wextra -g -Og -I obj -I include


# how to link against boost unit test framework: dynamic, static, or header
BOOST_UTF_MODE:=dynamic

ifeq ($(BOOST_UTF_MODE), dynamic)
GCC_FLAGS+=-DUSE_BOOST_UTF_DYNAMIC
LINK_TEST+=-l:libboost_unit_test_framework.so
else
ifeq ($(BOOST_UTF_MODE), static)
GCC_FLAGS+=-DUSE_BOOST_UTF_STATIC
LINK_TEST+=-l:libboost_unit_test_framework.a
else
GCC_FLAGS+=-DUSE_BOOST_UTF_HEADER
endif
endif


# phony targets: these rules don't generate the files they name
.PHONY: all test clean

# 'make' or 'make all' -> build all binaries
all: test
# 'make test' -> build bin/test and run it
test: bin/test
	bin/test

# clean up build products
clean:
	-rm -vrf bin obj


# binary linking
bin/test: $(OBJ_TEST) Makefile
	-mkdir -p $(@D)
	g++ $(GCC_FLAGS) -o $@ $(OBJ_TEST) $(LINK_TEST)

# source compilation
obj/%.o: src/%.cpp $(HEADERS) Makefile
	-mkdir -p $(@D)
	g++ $(GCC_FLAGS) -c -o $@ $<
