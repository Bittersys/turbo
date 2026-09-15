# ====================================================================
# Makefile for the Turbo Development Ecosystem
# ====================================================================

# 1. Compiler and configuration flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = turbo

# 2. Object files to be generated
OBJS = turbo.o lexico.o sintactico.o maquina.o gencode.o

# 3. Headers affecting compilation (forces re-compilation if changed)
HEADERS = turbo.h maquina.h

# 4. Default target (runs when typing 'make')
all: $(TARGET)

# 5. Final executable linking step
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# 6. Pattern rule for compiling object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# 7. Clean target (runs 'make clean' to wipe binaries and object files)
clean:
	rm -f $(OBJS) $(TARGET)

# 8. Declare phony targets to prevent filename conflicts
.PHONY: all clean
