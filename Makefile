current_dir = $(shell pwd)

all: jq.js

clean:
	rm jq.* libjq.o libjq.a libonig.a

libjq.a:
	cd jq && \
		emmake make LDFLAGS=-all-static CCFLAGS=-O3 -j4 && \
		cp .libs/libjq.a $(current_dir)/libjq.a

libonig.a:
	cd jq/modules/oniguruma && \
		emmake make && \
		cp src/.libs/libonig.a $(current_dir)/libonig.a

libjq.o: libjq_wrapper.c libjq.a libonig.a
	emcc -O3 libjq_wrapper.c libjq.a libonig.a -o libjq.o

jq.js: libjq.o pre.js
	 emcc -O3 -s MODULARIZE_INSTANCE=1 -s INVOKE_RUN=0 -s EXPORT_NAME='jq'\
	 -s EXPORTED_FUNCTIONS="['_execute']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall']" \
	 --pre-js pre.js libjq.o -o jq.js
