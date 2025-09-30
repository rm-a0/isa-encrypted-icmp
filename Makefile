CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinc
LDLIBS = -lpcap -lssl -lcrypto
SRCDIR = src
INCDIR = inc
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.cpp)
HDRS = $(wildcard $(INCDIR)/*.hpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

TARGET = secret

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)	$(LDLIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

run: $(TARGET)
	./$(TARGET)
tar:
	tar -cvf xrepcim00.tar $(SRCS) $(HDRS) Makefile secret.1 manual.pdf
clean:
	rm -rf $(BUILDDIR) $(TARGET)

.PHONY: all run clean