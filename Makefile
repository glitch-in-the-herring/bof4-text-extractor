all: extractor jpextractor
extractor: src/extractor.c src/utils.c
	gcc -o extractor src/extractor.c src/utils.c -O2
jpextractor: src/jpextractor.c src/jputils.c
	gcc -o jpextractor src/jpextractor.c src/jputils.c -O2