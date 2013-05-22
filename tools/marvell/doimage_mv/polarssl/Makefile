
DESTDIR=/usr/local
PREFIX=polarssl_

.SILENT:

all:
	$(MAKE) all -C library
	$(MAKE) all -C programs

install:
	mkdir -p $(DESTDIR)/include/polarssl
	cp -r include/polarssl $(DESTDIR)/include

	mkdir -p $(DESTDIR)/lib
	cp library/libpolarssl.* $(DESTDIR)/lib

	mkdir -p $(DESTDIR)/bin
	for p in programs/*/* ; do              \
	    if [ -x $$p ] && [ ! -d $$p ] ;     \
	    then                                \
	        f=$(PREFIX)`basename $$p` ;     \
	        cp $$p $(DESTDIR)/bin/$$f ;     \
	    fi                                  \
	done

clean:
	$(MAKE) clean -s -C library
	$(MAKE) clean -s -C programs

