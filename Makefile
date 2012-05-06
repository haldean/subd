EXECUTABLE=display_geom

all:
	make -C src
	cp src/$(EXECUTABLE) .

clean:
	make -C src clean
	rm $(EXECUTABLE)

run: all
	./$(EXECUTABLE)
