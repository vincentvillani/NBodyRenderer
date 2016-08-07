ifdef VERBOSE
	Q =
	E = @true 
else
	Q = @
	E = @echo 
endif

CXXFILES := $(wildcard source/*.cpp)

INFILES := $(CXXFILES)

INCLUDES = include

OBJFILES := $(CXXFILES:source/%.cpp=%)
DEPFILES := $(CXXFILES:source/%.cpp=%)
OFILES := $(OBJFILES:%=obj/%.o)

BINFILE = nbody_renderer

COMMONFLAGS = -Wall -Wextra -pedantic -I$(INCLUDES)
LDFLAGS = -Llib/ -L../../Trent/lex_engine -L../../Trent/lex_math -l:lex_engine.a -l:lex_math.a -lopengl32 -l:libglew32.dll.a -l:glfw3dll.a

# Add the -g command for debugging
ifdef DEBUG
	COMMONFLAGS := $(COMMONFLAGS) -g3
endif

CXXFLAGS = $(COMMONFLAGS) --std=c++11
DEPDIR = deps
all: $(BINFILE)
ifeq ($(MAKECMDGOALS),)
-include Makefile.dep
endif
ifneq ($(filter-out clean, $(MAKECMDGOALS)),)
-include Makefile.dep
endif

CXX = g++

-include Makefile.local

.PHONY: clean all depend
.SUFFIXES:
obj/%.o: source/%.cpp
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(CXXFLAGS)
Makefile.dep: $(CXXFILES)
	$(E)Depend
	$(Q)for i in $(^); do $(CXX) $(CXXFLAGS) -MM "$${i}" -MT obj/`basename $${i%.*}`.o; done > $@

$(BINFILE): $(OFILES)
	$(E)Linking $@
	$(CXX) -o $@ $(OFILES) $(LDFLAGS) $(AFILES)

clean:
	$(E)Removing files
	$(Q)rm -f $(BINFILE) obj/* Makefile.dep
	
	
#This is a debug target for printing a variable
print:
	@$(foreach var, $(OBJFILES), echo $(var);)

