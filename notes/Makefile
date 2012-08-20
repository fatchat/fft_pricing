2d-fft : 2d-fft.pdf

%.pdf : FORCE
	latex $(subst pdf,tex,$@)
	dvipdf $(subst pdf,dvi, $@)


FORCE:

.PHONY : all clean

all : 2d-fft

clean:
	@echo removing .aux, .dvi and .log files
	@rm -f *.aux *.dvi *.log
