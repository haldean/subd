EXECUTABLE=subd

all:
	make -C src
	cp src/$(EXECUTABLE) .

clean:
	make -C src clean
	rm $(EXECUTABLE)

run: all
	./$(EXECUTABLE)
