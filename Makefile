all:
	gcc -o two two.c
	gcc -o schedule mySched.c

clean:
	rm -rf *.o
