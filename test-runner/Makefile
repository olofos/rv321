# For a verbose build set V to an empty string when calling make: "V= make ..."
V?=@

SOURCES := util.c tokenizer.c expr-parser.c stmt-parser.c data-parser.c header-parser.c parser.c mt19937-64.c main.c

TARGET := parser

SRCDIR := src/
OBJDIR := obj/
BINDIR := ./
DEPDIR := .deps/
TSTDIR := test/
TSTOBJDIR := test/obj/
TSTBINDIR := test/bin/
TSTDEPDIR := test/.deps/
RESULTDIR := test/results/
GCOVDIR := gcov/

BUILD_DIRS = $(BINDIR) $(OBJDIR) $(DEPDIR) $(RESULTDIR) $(TSTOBJDIR) $(TSTBINDIR) $(TSTDEPDIR)

OBJ := $(SOURCES:%.c=$(OBJDIR)%.o)
DEPS := $(SOURCES:%.c=$(DEPDIR)%.d)

SOURCES_TST = $(wildcard $(TSTDIR)test_*.c)

CC = gcc
CFLAGS = -Wall -pedantic -g -I$(SRCDIR)

TST_CC = gcc
TST_WRAP = -Wl,--wrap=fopen,--wrap=fclose
TST_CFLAGS = -DUNIT_TESTING -Wall -I$(SRCDIR) -g --coverage $(TST_WRAP)

TST_RESULTS = $(patsubst $(TSTDIR)test_%.c,$(RESULTDIR)test_%.txt,$(SOURCES_TST))
TST_DEPS = $(TSTDEPDIR)*.d

TST_RESULT_FORMATTER = ./test/result-formatter.py

.PHONY: all bin clean erase test build_dirs coverage

all: $(BINDIR)$(TARGET)

$(TSTBINDIR)test_expr-parser: $(TSTOBJDIR)expr-parser.o $(TSTOBJDIR)util.o $(TSTOBJDIR)tokenizer.o $(TSTOBJDIR)test-util.o
$(TSTBINDIR)test_data-parser: $(TSTOBJDIR)data-parser.o $(TSTOBJDIR)expr-parser.o $(TSTOBJDIR)util.o $(TSTOBJDIR)tokenizer.o $(TSTOBJDIR)test-util.o
$(TSTBINDIR)test_stmt-parser: $(TSTOBJDIR)stmt-parser.o $(TSTOBJDIR)data-parser.o $(TSTOBJDIR)expr-parser.o $(TSTOBJDIR)util.o $(TSTOBJDIR)tokenizer.o $(TSTOBJDIR)test-util.o
$(TSTBINDIR)test_header-parser: $(TSTOBJDIR)header-parser.o $(TSTOBJDIR)util.o $(TSTOBJDIR)tokenizer.o $(TSTOBJDIR)test-util.o
$(TSTBINDIR)test_parser: $(TSTOBJDIR)parser.o $(TSTOBJDIR)header-parser.o $(TSTOBJDIR)stmt-parser.o $(TSTOBJDIR)data-parser.o $(TSTOBJDIR)expr-parser.o $(TSTOBJDIR)util.o $(TSTOBJDIR)tokenizer.o $(TSTOBJDIR)test-util.o

-include $(DEPS)
-include $(TST_DEPS)

$(BINDIR)$(TARGET): build_dirs $(OBJ)
	@echo LD $@
	$(V)$(CC) $(CFLAGS) $(OBJ) -o $@

$(OBJDIR)%.o : $(SRCDIR)%.c
	@echo CC $<
	$(V)$(CC) $(CFLAGS)  $(INCLUDES) -c $< -o $@
	$(V)$(CC) -MM -MT $@ $(CFLAGS)  $(INCLUDES) $< > $(DEPDIR)$*.d

test: build_dirs $(TST_RESULTS)
	$(V)$(TST_RESULT_FORMATTER) $(TST_RESULTS)

build_dirs:
	$(V)mkdir -p $(BUILD_DIRS)


$(RESULTDIR)%.txt: $(TSTBINDIR)%
	@echo Running $<
	@echo
	$(V) CMOCKA_MESSAGE_OUTPUT=subunit ./$< > $@ 2> $(RESULTDIR)$(*F).err || true

$(TSTOBJDIR)%.o : $(TSTDIR)%.c
	@echo CC $@
	$(V)$(TST_CC) $(TST_CFLAGS)  $(INCLUDES) -c $< -o $@
	$(V)$(TST_CC) -MM -MT $@ $(TST_CFLAGS) $(INCLUDES) $< > $(TSTDEPDIR)$*.d

$(TSTOBJDIR)%.o : $(SRCDIR)%.c
	@echo CC $@
	$(V)$(TST_CC) $(TST_CFLAGS)  $(INCLUDES) -c $< -o $@
	$(V)$(TST_CC) -MM -MT $@ $(TST_CFLAGS) $(INCLUDES) $< > $(TSTDEPDIR)$*.d

$(TSTBINDIR)test_%: $(TSTOBJDIR)test_%.o
	@echo LD $@
	$(V)$(TST_CC) -o $@ $(TST_CFLAGS) $^ -lcmocka

coverage: test
	@echo Collecting coverage data
	$(V)mkdir -p $(GCOVDIR)html
	$(V)gcov src/*.c -o $(TSTOBJDIR) > /dev/null
	$(V)mv *.gcov $(GCOVDIR)
	$(V)lcov --quiet --capture --directory $(TSTOBJDIR) --output-file $(GCOVDIR)coverage.info
	$(V)genhtml --quiet gcov/coverage.info --output-directory $(GCOVDIR)html/
	$(V)lcov --quiet --remove $(GCOVDIR)coverage.info 'test/*' '/usr/*' --output-file $(GCOVDIR)coverage-src.info
	@echo
	$(V)lcov --summary $(GCOVDIR)coverage-src.info 2>&1 | grep -v 'Reading\|branches'

coverage-open: coverage
	$(V)xdg-open $(GCOVDIR)html/index.html

clean:
	@echo Cleaning
	$(V)-rm -f $(OBJ) $(DEPS) $(TST_DEPS) $(TSTOBJDIR)*.o $(TSTOBJDIR)*.gcda $(TSTOBJDIR)*.gcno $(TSTBINDIR)test_* $(RESULTDIR)*.txt $(BINDIR)$(TARGET) $(LIBDIR)$(LIBTARGET)
	$(V)-rm -rf $(GCOVDIR)

.PRECIOUS: $(TSTBINDIR)test_%
.PRECIOUS: $(DEPDIR)%.d
.PRECIOUS: $(OBJDIR)%.o
.PRECIOUS: $(RESULTDIR)%.txt
.PRECIOUS: $(TSTOBJDIR)%.o
