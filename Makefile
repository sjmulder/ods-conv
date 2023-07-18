CFLAGS.xml!=	xml2-config --cflags
LDLIBS.xml!=	xml2-config --libs

CFLAGS+=	-Wall -Wextra ${CFLAGS.xml}
LDLIBS+=	-lzip ${LDLIBS.xml}

all: ods-conv

clean:
	rm -f ods-conv

.PHONY: all clean
