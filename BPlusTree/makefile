# !!!=== cross compile...
CROSS_COMPILE ?= 

CC  = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR  = $(CROSS_COMPILE)ar

ARFLAGS = -cr
RM     = -rm -rf
MAKE   = make

CFLAGS  = 
LDFLAGS = 
DEFS    =
LIBS    =
 
# !!!===
# target executable file or .a or .so
target = a.out

# !!!===
# compile flags
CFLAGS += -Wall -Wfatal-errors

#****************************************************************************
# debug can be set to y to include debugging info, or n otherwise
debug  = y

#****************************************************************************

ifeq ($(debug), y)
    CFLAGS += -ggdb -rdynamic
else
    CFLAGS += -O2 -s
endif

# !!!===
DEFS    += -DFUCK

CFLAGS  += $(DEFS)

LIBS    += 

LDFLAGS += $(LIBS)

# !!!===
INC1 = ./
INC2 = ./inc
INC3 = 
INCDIRS := -I$(INC1) -I$(INC2)

# !!!===
CFLAGS += $(INCDIRS)

# !!!===
#LDFLAGS += -lpthread -lrt

DYNC_FLAGS += -fpic -shared

# !!!===
# source file(s), including c file(s) or cpp file(s)
# you can also use $(wildcard *.c), etc.
SRC_DIR = .
SRC_DIR1 = 
SRC_DIR2 = 
SRC_DIR3 = 

# ok for c/c++
SRC = $(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp)
SRC+=$(wildcard $(SRC_DIR1)/*.c $(SRC_DIR1)/*.cpp)
SRC+=$(wildcard $(SRC_DIR2)/*.c $(SRC_DIR2)/*.cpp)
SRC+=$(wildcard $(SRC_DIR3)/*.c $(SRC_DIR3)/*.cpp)

# ok for c/c++
OBJ = $(patsubst %.c,%.o, $(patsubst %.cpp,%.o, $(SRC))) 


# !!!===
# in case all .c/.cpp need g++...
# CC = $(CXX)

ifeq ($(V),1)
Q=
NQ=true
else
Q=
NQ=echo
endif

###############################################################################

all: $(target)

$(target): $(OBJ)

ifeq ($(suffix $(target)), .so)
	@$(NQ) "Generating dynamic lib file..." $(notdir $(target))
	$(Q)$(CXX) $(CFLAGS) $^ -o $(target) $(LDFLAGS) $(DYNC_FLAGS)
else ifeq ($(suffix $(target)), .a)
	@$(NQ) "Generating static lib file..." $(notdir $(target))
	$(Q)$(AR) $(ARFLAGS) -o $(target) $^
else
	@$(NQ) "Generating executable file..." $(notdir $(target))
	$(Q)$(CXX) $(CFLAGS) $^ -o $(target) $(LDFLAGS)
endif

# make all .c or .cpp
%.o: %.c
	@$(NQ) "Compiling: " $(addsuffix .c, $(basename $(notdir $@)))
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@$(NQ) "Compiling: " $(addsuffix .cpp, $(basename $(notdir $@)))
	$(Q)$(CXX) $(CFLAGS) -c $< -o $@

clean:
	@$(NQ) "Cleaning..."
	$(Q)$(RM) $(target)
	@find . -iname '*.o' -o -iname '*.bak' -o -iname '*.d' | xargs rm -f

.PHONY: all clean

