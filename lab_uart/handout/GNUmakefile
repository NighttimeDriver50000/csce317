# Copyright 2020 Charles Daniels, Jason Bakos, Philip Conrad

include ../../../opinionated.mk

IPEPDF=$(shell for f in *.ipe ; do echo "$$(basename "$$f" .ipe).pdf" ; done)

all: README.pdf
> cp ./README.pdf ../README.pdf
.PHONY: all

%.pdf: %.tex figures
> latexmk -pdf "$<"
> latexmk -c "$<"
> rm -f *.fls

figures: $(IPEPDF)
.PHONY: figures

%.pdf: %.ipe
> iperender -pdf "$<" "$@"



clean:
> for f in *.tex ; do latexmk -C "$$f" ; done
> for f in *.ipe ; do rm -f "$$(basename "$$f" .ipe).pdf" ; done
> rm -f *.fls
> rm -f ../README.pdf
.PHONY: clean
