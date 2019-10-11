# C++ Makefile Template
# Copyright (C) 2018  Manuel Weitzman
# You may use/distribute this template under the terms of the MIT LICENSE

# HowTo:
#	Create a src/ dir for all .cpp files
#	Create a head/ dir for all .h files
#	In .cpp files import .h files as if they were in the same dir
#	You have available:
#		make			Compile binaries
#		make install		Install final exec to /usr/bin
#		make uninstall		Remove final exec from /usr/bin
#		make clean		Remove intermediate .o files
#		make distclean		Remove final executable
#		make cleanall		clean+distclean

# Final executable name
EXEC = build/s18_example

# Directories for sourcefiles, headers and object files
BUILDDIR = build
SRCDIR = src
HEADDIR = head
OBJDIR = obj

# Files will be detected automatically (they shall not be in subdirectories
# though)
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

# Compiler options
CXX ?= /usr/bin/g++
CPPFLAGS = $(addprefix -I, $(HEADDIR)) -isystem../sdsl/build/include -isystem../s9/include/sdsl -MMD -MP
CFLAGS = -O9 -std=gnu++17 -Wall -Weffc++ -Wextra \
	-Wcast-align -Wcast-qual -Wchar-subscripts -Wcomment -Wconversion \
	-Wdisabled-optimization -Wfloat-equal -Wformat -Wformat=2 \
	-Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimport \
	-Winit-self -Winvalid-pch -Wunsafe-loop-optimizations \
	-Wlong-long -Wmissing-braces -Wmissing-field-initializers \
	-Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn \
	-Wpacked -Wno-padded -Wparentheses -Wpointer-arith -Wredundant-decls \
	-Wreturn-type -Wsequence-point -Wshadow -Wsign-compare \
	-Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch \
	-Wswitch-default -Wswitch-enum -Wtrigraphs -Wuninitialized \
	-Wunknown-pragmas -Wunreachable-code -Wunused -Wunused-function \
	-Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable \
	-Wvariadic-macros -Wvolatile-register-var -Wwrite-strings \
	-mtune=native -DNDEBUG
LDFLAGS = -L../sdsl/build/lib
LDLIBS = -lsdsl -ldivsufsort -ldivsufsort64
DEBUG =

# Utilities used for output and others
ECHO = echo
RM = rm -rf
MKDIR = mkdir -p
INSTALL = install
FIND = find
CP = cp

# Makefile rules
.PHONY: all
all: $(OBJDIR) $(BUILDDIR) $(HEADDIR) $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(DEBUG) -o $@ $^ $(LDLIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(DEBUG) -c $< -o $@

$(OBJDIR):
	@$(MKDIR) -p $@

$(BUILDDIR):
	@$(MKDIR) -p $@

$(HEADDIR):
	@$(MKDIR) -p $@

.PHONY: install
install:
	$(INSTALL) $(EXEC) /usr/bin/$(EXEC)

.PHONY: uninstall
uninstall:
	$(RM) /usr/bin/$(EXEC)

.PHONY: cleanall
cleanall: clean distclean

.PHONY: clean
clean:
	$(FIND) obj/ head/ src/ build/ -iname '*.d'            -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.o'            -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.ko'           -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.obj'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.elf'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.ilk'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.map'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.exp'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.gch'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.pch'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.lib'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.a'            -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.la'           -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.lo'           -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.dll'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.so'           -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.so.*'         -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.dylib'        -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.exe'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.out'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.app'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.i*86'         -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.x86_64'       -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.hex'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.su'           -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.idb'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.pdb'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.mod*'         -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.cmd'          -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname 'modules.order'  -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname 'Module.symvers' -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname 'Mkfile.old'     -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname 'dkms.conf'      -type f -delete
	$(FIND) obj/ head/ src/ build/ -iname '*.dSYM'        -type d -empty -delete
	$(FIND) obj/ head/ src/ build/ -iname '.tmp_versions' -type d -empty -delete
	$(FIND) obj/ head/ src/ build/ -iname 'obj'           -type d -empty -delete

.PHONY: distclean
distclean:
	$(RM) $(EXEC)

-include $(wildcard $(OBJDIR)/*.d)
