#!/bin/bash

cc=gcc
src='src/*.c src/scripts/*.c src/UI/*.c src/client/*.c src/server/packet.c'
exe=Tree2D
std=-'std=c99'

flags=(
    $std
    -Wall
    -Wextra
    -O2
)

inc=(
    -I.
    -Iinclude/
)

lib=(
    -Llib/
    -lglfw
    -lfreetype
    -lz
    -lpng
    -ljpeg
    -lenet
    -lfract
    -lutopia
    -lglee
    -lgleex
    -lglui
    -lnano
    -limgtool
    -lphoton
    -lethnic
    -lmass
)

linux=(
    -lm
    -lGL
    -lGLEW
)

mac=(
    -framework OpenGL
    -mmacos-version-min=10.9
)


compile() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        $cc ${flags[*]} ${inc[*]} ${lib[*]} ${mac[*]} $src -o $exe
    elif echo "$OSTYPE" | grep -q "linux"; then
        $cc ${flags[*]} ${inc[*]} ${lib[*]} ${linux[*]} $src -o $exe
    else
        echo "OS is not supported yet..." && exit
    fi
}

build_lib() {
    pushd $1/ && ./build.sh $2 && mv *.a ../lib/ && popd
}

build() {
    mkdir lib/
    build_lib fract -s
    build_lib utopia -slib
    build_lib imgtool -slib
    build_lib mass -s
    build_lib photon -s
    build_lib glee -s
    build_lib gleex -s
    build_lib glui -s
    build_lib ethnic -s
    build_lib nano -s
}

clean() {
    rm -r lib && rm $exe
}

fail() {
    echo "Use 'comp' to compile game and 'server' to build server." && exit
}

case "$1" in
    "server")
        shift
        pushd src/server/ && ./build.sh "$@" && popd;;
    "run")
        shift
        compile && ./$exe "$@";;
    "comp")
        compile;;
    "build")
        build;;
    "all")
        shift
        build && compile && ./$exe "$@";;
    "clean")
        clean;;
    *)
        fail;;
esac
