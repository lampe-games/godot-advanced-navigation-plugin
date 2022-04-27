all: gd-lint gd-format-check

cloc:
	cloc .

gource:
	gource . --key -s 1.5 -a 0.1

gd-format:
	find -name '*.gd' | xargs gdformat

gd-format-check:
	find -name '*.gd' | xargs gdformat --check

gd-lint:
	find -name '*.gd' | xargs gdlint
