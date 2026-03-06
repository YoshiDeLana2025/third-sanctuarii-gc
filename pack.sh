#!/usr/bin/env sh

name=$(basename "$PWD")

mkdir "$name"

make
cp "$name.dol" "$name/boot.dol"
cp icon.png "$name/icon.png"
cp meta.xml "$name/meta.xml"

zip -r "$name.zip" "$name"
rm -r "$name"