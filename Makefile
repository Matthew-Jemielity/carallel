CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -fPIC -I inc
LDFLAGS = -pthread
SOURCES = $(shell find src -name "*.c")
OBJECTS = $(SOURCES:.c=.o)
LIBRARY = libcarallel.so
DEMO = demo

all: $(LIBRARY) $(DEMO)

$(LIBRARY): out $(OBJECTS)
	@$(CC) $(LDFLAGS) out/$(shell basename $(OBJECTS)) -shared -o $(shell basename $@)
	@echo "Created shared library: "$(LIBRARY)

.c.o: out
	@$(CC) $(CFLAGS) -c $< -o out/$(shell basename $@)

clean:
	@rm -rf $(DEMO) $(LIBRARY) out

$(DEMO): demo.c
	@$(CC) $(CFLAGS) $(LDFLAGS) -L . demo.c -o $@ -lcarallel
	@echo "To execute demo: LD_LIBRARY_PATH=$$""LD_LIBRARY_PATH:. ./"$(DEMO)

out:
	@mkdir -p $@

