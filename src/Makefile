#########################################
# rsidImpu - Makefile
#########################################

# Compiler
CXX = g++

# Flags
CXXFLAGS = -std=c++11 -O3 -fopenmp
LDFLAGS  = -lz

# All source files in current directory
SRC = \
	main.cpp \
	args.cpp \
	util.cpp \
	gadgets.cpp \
	linereader.cpp \
	dbsnp.cpp \
	gwas.cpp \
	allele.cpp \
	writer.cpp

# Convert .cpp → .o
OBJ = $(SRC:.cpp=.o)

# Output binary
TARGET = rsidImpu

#########################################
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

#########################################
clean:
	rm -f $(OBJ) $(TARGET)

#########################################
.PHONY: all clean
