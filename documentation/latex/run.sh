#/usr/bin/bash

bash clean.sh
pdflatex thesis
bibtex thesis
pdflatex thesis
pdflatex thesis
