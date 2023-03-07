CC=g++
CFLAGS=-std=c++17 -Wall -Wextra -pedantic
LIBS=-lSDL2main -lSDL2 -lSDL2_image
SRCDIR=src
OBJDIR=obj
INCDIR=include
SRCS=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
INCLUDE=$(wildcard $(INCDIR)/*.h)
TARGET=chess.out

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCS)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -I$(INCDIR) $(OBJS) -o $@ $(LIBS)