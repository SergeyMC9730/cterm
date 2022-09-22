FILELIST = main.c cJSON/cJSON.c cJSON/cJSON_Utils.c
OUTPUT = cterm
CURDIR = $(shell pwd)

all:
	@rm -rf build
	@mkdir build
	@mkdir build/applications
	@gcc -c -fPIC -O2 -g cterm_generic.c -o cterm_generic.o
	@gcc -c -fPIC -O2 -g cterm_tests.c -o cterm_tests.o
	@gcc -c -fPIC -O2 -g cterm_extensions.c -o cterm_extensions.o
	@gcc -o cterm_generic.so -shared -g -fPIC -O2 cterm_generic.o
	@gcc -o cterm_tests.so -shared -g -fPIC -O2 cterm_tests.o
	@gcc -o cterm_extensions.so -shared -g -fPIC -O2 cterm_extensions.o
	@rm -rf *.o
	@cp *.so build/applications
	@rm -rf *.so
	@gcc ${FILELIST} -lncurses -ldl -Wl,-rpath,${CURDIR}/build/applications -O2 -g -o ${OUTPUT}
	@mv ${OUTPUT} build/
	@cp config.json build/
