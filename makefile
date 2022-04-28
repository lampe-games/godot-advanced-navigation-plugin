all: gd-lint gd-format-check cpp-format-check

init:
	git submodule update --init --recursive

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

setup-linux:
	cd thirdparty/godot-cpp && scons -j$(shell nproc) platform=linux target=release generate_bindings=yes && cd -

setup-windows:
	cd thirdparty/godot-cpp && scons  -j$(shell nproc) platform=windows target=release generate_bindings=yes && cd -

build-linux:
	mkdir -p bin/
	scons -j$(shell nproc) platform=linux target=release

build-windows:
	scons -j$(shell nproc) platform=windows target=release
	mv bin/win64/libadvancednavigation.so bin/win64/libadvancednavigation.dll

cpp-format-check:
	clang-format --style=file --dry-run -Werror src/*.[ch]pp

clean:
	scons platform=linux target=release -c
	scons platform=windows target=release -c
