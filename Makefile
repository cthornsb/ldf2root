#####################################################################

# Installer for ldf2root
# Cory R. Thornsberry
# updated: Feb. 25th, 2017

#####################################################################

# Set the binary install directory.
INSTALL_DIR = $(HOME)/bin

#####################################################################

CC = g++

CFLAGS = -g -fPIC -Wall -std=c++0x -Iinclude -Idict `root-config --cflags`
#CFLAGS = -fPIC -Wall -std=c++0x -Iinclude -Idict `root-config --cflags`
LDLIBS = -lstdc++ `root-config --libs`
LDFLAGS = `root-config --glibs`

# Directories
TOP_LEVEL = $(shell pwd)
INCLUDE_DIR = $(TOP_LEVEL)/include
SOURCE_DIR = $(TOP_LEVEL)/source
DICT_DIR = $(TOP_LEVEL)/dict
EXEC_DIR = $(TOP_LEVEL)/exec
OBJ_DIR = $(TOP_LEVEL)/obj

# Local root dictionary
DICTIONARY = $(DICT_DIR)/libRootDict.a

# Local shared libraries
LDLIBS += $(DICTIONARY)

# Root dictionary generator script
DICT_GENERATOR = $(TOP_LEVEL)/macros/rootdict.sh

# Tools
ALL_TOOLS = ldf2root tplotter
EXE_NAMES = $(addprefix $(EXEC_DIR)/, $(addsuffix .a, $(ALL_TOOLS)))
INSTALLED = $(addprefix $(INSTALL_DIR)/, $(ALL_TOOLS))

# List of directories to generate if they do not exist.
DIRECTORIES = $(OBJ_DIR) $(EXEC_DIR)

# C++ CORE
SOURCES = caen.cpp

# C++ object files
OBJECTS = $(addprefix $(OBJ_DIR)/,$(SOURCES:.cpp=.o))
OBJECTS += $(DICT_DIR)/Structures.o

#####################################################################

all: $(DIRECTORIES) $(DICTIONARY) $(EXE_NAMES)
#	Create all directories, make all objects, and link executable

.PHONY: $(ALL_TOOLS) $(INSTALLED) $(DIRECTORIES)

.PRECIOUS: $(OBJ_DIR)/%.o

#####################################################################

$(DIRECTORIES):
#	Make the default configuration directory
	@if [ ! -d $@ ]; then \
		echo "Making directory: "$@; \
		mkdir -p $@; \
	fi

#####################################################################

$(DICTIONARY): 
	@echo "Entering directory \"$(DICT_DIR)\""
	@cd $(DICT_DIR) && $(DICT_GENERATOR) -n RootDict -c

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
#	Compile C++ source files
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC_DIR)/%.a: $(SOURCE_DIR)/%.cpp $(OBJECTS)
#	Compile C++ source files
	$(CC) $(CFLAGS) $< $(OBJECTS) -o $@ $(LDLIBS)

#####################################################################

$(ALL_TOOLS):
	@echo " Installing "$(INSTALL_DIR)/$@
	@rm -f $(INSTALL_DIR)/$@
	@ln -s $(EXEC_DIR)/$@.a $(INSTALL_DIR)/$@

install: all $(ALL_TOOLS)
	@echo "Finished installing tools to "$(INSTALL_DIR)

#####################################################################

$(INSTALLED):
	@rm -f $@

uninstall: clean $(INSTALLED)
	@rm -f $(EXE_NAMES)
	@echo "Finished uninstalling"

clean:
	@rm -f $(OBJ_DIR)/*
	@rm -f $(DICT_DIR)/Structures.o $(DICT_DIR)/libRootDict*
