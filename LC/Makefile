CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
CC_FLAGS  := --std=c++14 -I./src -I../lib/PEGTL/include -I../lib -g3 -DDEBUG -pedantic -pedantic-errors -Werror=pedantic
LD_FLAGS  := 
CC        := g++
CC_CLASS  := LCc
PL_CLASS  := LC
EXT_CLASS := c

all: dirs $(PL_CLASS)

dirs: obj bin

obj:
	mkdir -p $@

bin:
	mkdir -p $@

$(PL_CLASS): $(OBJ_FILES)
	$(CC) $(LD_FLAGS) -o ./bin/$@ $^

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) -c -o $@ $<

oracle: $(PL_CLASS)
	../scripts/generateOutput.sh $(EXT_CLASS) $(CC_CLASS)

rm_tests_without_oracle:
	../scripts/rm_tests_without_oracle.sh $(EXT_CLASS)

test: dirs $(PL_CLASS)
	../scripts/test.sh $(EXT_CLASS) $(CC_CLASS)

performance: dirs $(PL_CLASS)
	./$(CC_CLASS) tests/competition.$(EXT_CLASS) ; time ./a.out

clean:
	rm -fr bin obj *.out prog.* *.o core.* `find tests -iname *.tmp`
