CFLAGS += -Wall
LDFLAGS =
target = fis_test

.PHONY: all
test_app:
	$(CC) $(CFLAGS) $(LDFLAGS) fis_post_proc.c fis.c testing/fis_test.c -o $(target)
dll: 
	$(CC) -c fis_post_proc.c -o fis_post_proc.o
	$(CC) -c -DBUILD_DLL fis.c -o build/fis.o
	$(CC) -shared -o fis.dll -Wl,--out-implib,libtstdll.a fis_post_proc.o fis.o
#Build an EXE that references the DLL:
#gcc -o hello.exe hello.o -L./ -ltstdll

.PHONY: clean
clean:
	$(RM) *.o $(target)
