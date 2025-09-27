CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinc
LDLIBS = -lpcap -lssl -lcrypto
SRCDIR = src
INCDIR = inc
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.cpp)
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
zip:
	zip -r xrepcim00.zip . -x ".*" "build/*" "test/*" "secret" "test_env/*"

clean:
	rm -rf $(BUILDDIR) $(TARGET)

.PHONY: all run clean