EAGLEFILES+=$(wildcard ../*.sch)
EAGLEFILES+=$(wildcard ../*.brd)
PDFS=$(patsubst ../%,%.pdf,$(EAGLEFILES))
EAGLE=eagle

all: $(PDFS)

%.pdf: ../%
	-rm $@
	$(EAGLE) -C 'print 10 -1 caption file $(shell pwd)/output.pdf; quit;' $(shell pwd)/$<
	mv output.pdf $@

clean:
	-rm output.pdf
	-rm $(PDFS)

view: all
	okular $(PDFS)