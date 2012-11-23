# Copyright 2012 Eugen Sawin <esawin@me73.com>
SRCDIR:=src
TSTDIR:=src/test
BINDIR:=bin
OBJDIR:=bin/obj
GTESTLIBS:=-lgtest -lgtest_main
GFLAGSDIR:=deps/gflags-2.0/.libs
CXX:=g++ -std=c++0x
# CXX:=g++ -std=c++0x -Ilibs/gflags-2.0/src
CFLAGS:=-Wall -O3
LIBS:=-lgflags -lpthread -lrt -llpsolve55 -lcolamd -ldl -lm
# LIBS:=$(GFLAGSDIR)/libgflags.a -lpthread -lrt -llpsolve55 -lcolamd -ldl -lm
TSTFLAGS:=
TSTLIBS:=$(GTESTLIBS) $(LIBS)
BINS:=ash

TSTBINS:=$(notdir $(basename $(wildcard $(TSTDIR)/*.cc)))
TSTOBJS:=$(addsuffix .o, $(notdir $(basename $(wildcard $(TSTDIR)/*.cc))))
OBJS:=$(notdir $(basename $(wildcard $(SRCDIR)/*.cc)))
OBJS:=$(addsuffix .o, $(filter-out $(BINS), $(OBJS)))
OBJS:=$(addprefix $(OBJDIR)/, $(OBJS))
BINS:=$(addprefix $(BINDIR)/, $(BINS))
TSTBINS:=$(addprefix $(BINDIR)/, $(TSTBINS))

compile: makedirs $(BINS)
	@echo "compiled all"

profile: CFLAGS=-Wall -O3 -DPROFILE
profile: LIBS+=-lprofiler
profile: clean compile

opt: CFLAGS=-Ofast -flto -mtune=native -DNDEBUG
opt: clean compile

debug: CLFAGS=-O0 -g
debug: compile

ARGS:=-verbose=false -brief=false
LOG:=perf-results.txt
perftest: opt
	@mkdir -p log; rm -f log/$(LOG);
	@echo "test parameters: LOG=$(LOG) ARGS=$(ARGS)";
	@for i in examples/*.nfg;\
		do echo "testing $$i";\
		./bin/ash $(ARGS) $$i >> log/$(LOG);\
		echo " " >> log/$(LOG);\
	done
	@echo "tested all (results in log/$(LOG))";

GAMBITLOG:=gambit-perf-results.txt
gambittest:
	@mkdir -p log; rm -f log/$(GAMBITLOG);
	@echo "test parameters: GAMBITLOG=$(GAMBITLOG)";
	@for i in examples/*.nfg;\
		do echo "testing $$i";\
		echo "File: $$i\nPure:" >> log/$(GAMBITLOG);\
		gambit-enumpure -q < $$i >> log/$(GAMBITLOG);\
		echo "Pure & Mixed:" >> log/$(GAMBITLOG);\
		gambit-enummixed -q < $$i >> log/$(GAMBITLOG);\
		echo " " >> log/$(GAMBITLOG);\
	done
	@echo "tested all (results in log/$(GAMBITLOG))";

depend: gflags cpplint

makedirs:
	@mkdir -p bin/obj

gflags:
	@tar xf deps/gflags-2.0.tar.gz -C deps/;
	@cd deps/gflags-2.0/; ./configure; make;
	@echo "compiled gflags"

cpplint: 
	@if [ -f tools/cpplint/cpplint.py ];\
	then\
		echo "updating cpplint";\
		cd tools/cpplint; git pull; cd ../..;\
	else\
		echo "cloning cpplint";\
		mkdir tools && cd tools;\
		git clone git@github.com:eamsen/cpplint.git; cd ..;\
	fi

check: makedirs $(TSTBINS)
	@for t in $(TSTBINS); do ./$$t; done
	@echo "completed tests"

checkstyle:
	@python tools/cpplint/cpplint.py --filter=-readability/streams\
		$(SRCDIR)/*.h $(SRCDIR)/*.cc

clean:
	@rm -f $(OBJDIR)/*.o
	@rm -f $(BINS)
	@rm -f $(TSTBINS)
	@echo cleaned

.PRECIOUS: $(OBJS) $(TSTOBJS)
.PHONY: compile profile opt perftest depend makedirs gflags check cpplint\
	checkstyle clean

$(BINDIR)/%: $(OBJS) $(SRCDIR)/%.cc
	@$(CXX) $(CFLAGS) -o $(OBJDIR)/$(@F).o -c $(SRCDIR)/$(@F).cc
	@$(CXX) $(CFLAGS) -o $(BINDIR)/$(@F) $(OBJDIR)/$(@F).o $(OBJS) $(LIBS)
	@echo compiled $(BINDIR)/$(@F)

$(BINDIR)/%-test: $(OBJDIR)/%-test.o $(OBJS)
	@$(CXX) $(TSTFLAGS) -o $(BINDIR)/$(@F) $(OBJS) $< $(TSTLIBS)
	@echo compiled $(BINDIR)/$(@F)

$(OBJDIR)/%-test.o: $(TSTDIR)/%-test.cc
	@$(CXX) $(TSTFLAGS) -o $(OBJDIR)/$(@F) -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cc $(SRCDIR)/%.h
	@$(CXX) $(CFLAGS) -o $(OBJDIR)/$(@F) -c $<
