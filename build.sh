#!/bin/bash

cc=gcc
src='src/*.c src/scripts/*.c src/UI/*.c src/client/*.c src/server/packet.c'
exe=Tree2D
server=T2Dserver
std='-std=c99'

if echo "$OSTYPE" | grep -q "linux"; then
    rflag="-Wl,--whole-archive"
    lflag="-Wl,--no-whole-archive"
fi

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
    $rflag
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
    $lflag
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
        echo "$cc ${flags[*]} ${inc[*]} ${lib[*]} ${mac[*]} $src -o $exe"
        $cc ${flags[*]} ${inc[*]} ${lib[*]} ${mac[*]} $src -o $exe
    elif echo "$OSTYPE" | grep -q "linux"; then
        echo "$cc ${flags[*]} ${inc[*]} ${lib[*]} ${linux[*]} $src -o $exe"
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

build_app() {
    echo "building MacOS app..."
    mkdir $exe.app/
    mkdir $exe.app/Contents/
    mkdir $exe.app/Contents/Resources/
    mkdir $exe.app/Contents/MacOS/

    cp $exe $exe.app/Contents/MacOS/
    cp assets/MacOS/Info.plist $exe.app/Contents/
    cp assets/MacOS/icon.icns $exe.app/Contents/Resources/
    cp -r assets $exe.app/Contents/Resources/assets/
    tar -xzvf ../../Assets/lib.tar.gz -C $exe.app/Contents/MacOS/

    echo "MacOS app built succesfully" 
}

clean() {
    rm -r lib && rm $exe
}

fail() {
    echo "Use 'comp' to compile game and 'server' to build server." && exit
}

case "$1" in
    "server")
        src=src/server/*.c
        exe=$server
        compile;;
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
    "app")
        build_app;;
    "clean")
        clean;;
    *)
        fail;;
esac
