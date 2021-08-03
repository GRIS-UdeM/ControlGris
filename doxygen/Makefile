SHELL := /bin/bash

SOURCE_FILES := $(shell find ../Source -type f -name "cg_*.h*" | sed 's/ /\\ /g')

.PHONEY: clean

doc/doxygen.css: doc/index.html
	cp extra/tabs.css doc/tabs.css -f
	cp extra/doxygen.css doc/doxygen.css -f
	cp extra/dynsections.js doc/dynsections.js -f

doc/index.html: Doxyfile
	doxygen

clean:
	rm -rf build doc
