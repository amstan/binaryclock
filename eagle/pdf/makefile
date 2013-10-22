EAGLEFILES+=$(wildcard ../*.sch)
EAGLEFILES+=$(wildcard ../*.brd)
PDFS=$(patsubst ../%,%.pdf,$(EAGLEFILES))
EAGLE=eagle

all: $(PDFS)

%.sch.pdf: ../%.sch
	-rm $@
	$(EAGLE) -C 'print 1 -1 caption file $(shell pwd)/output.pdf; quit;' $(shell pwd)/$<
	mv output.pdf $@

%.brd.pdf: ../%.brd
	-rm $@
	$(EAGLE) -C 'print 1 -1 paper 8.5x11.0inch caption file $(shell pwd)/output.pdf; quit;' $(shell pwd)/$<
	mv output.pdf $@

clean:
	-rm output.pdf
	-rm $(PDFS)

view: all
	okular $(PDFS)