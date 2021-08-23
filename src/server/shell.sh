#!/bin/bash

comp=gcc
src='*.c'
exe=tree_server
std=-std=c99
dir=../../../../

flags=(
    $std
    -Wall
    -Wextra
    -O2
)

inc=(
    '-I.'
    '-I'$dir
    '-I'$dir'src/'
    '-I'$dir'include/'
    '-L'$dir'lib/'
)

lib=(
    -lenet
    -lutopia
    -lnano
    -lzbug
    -lfract
    -limagine
)


compile() {
    if echo "$OSTYPE" | grep -q "darwin"; then 
        $comp ${flags[*]} ${inc[*]} ${lib[*]} $src -o $dir'bin/'$exe
    elif echo "$OSTYPE" | grep -q "linux"; then
        $comp ${flags[*]} ${inc[*]} ${lib[*]} -lm $src -o $dir'bin/'$exe
    else
        echo "OS not supported yet..."
        exit
    fi
}

execute() {
    cd $dir
    ./shell.sh exe $exe "$@"
}

fail() {
    echo "Use 'comp' to compile, 'exe' to execute or 'run' to do both."
    exit
}

if [[ $# < 1 ]]
then
    fail
fi

if [[ "$1" == "run" ]]
then
    shift
    compile
    execute "$@"
elif [[ "$1" == "comp" ]]
then
    compile
elif [[ "$1" == "exe" ]]
then
    shift
    execute "$@"
else
    fail
fi
