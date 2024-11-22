CFLAGS = -I. -g

relocatable = mybc.o lexer.o parser.o

mybc: $(relocatable)
	$(CC) -o mybc $(relocatable)
clean:
	$(RM) *.o
mostlyclean: clean 
	$(RM) *- mybc