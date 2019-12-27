#!/bin/bash

SRCPATH=$1

if test -z "$SRCPATH"; then
	echo "Usage: $0 <path_to_universal_ctags_git_directory>"
	exit 0
fi

echo "Copying sources from $SRCPATH"


cp -v $SRCPATH/main/*.c src/main/
cp -v $SRCPATH/main/*.h src/main/
cp -v $SRCPATH/parsers/*.c src/parsers/
cp -v $SRCPATH/parsers/*.h src/parsers/
cp -v $SRCPATH/parsers/cxx/*.c src/parsers/cxx/
cp -v $SRCPATH/parsers/cxx/*.h src/parsers/cxx/
cp -v $SRCPATH/optlib/*.c src/optlib/

echo "Done"