#!/bin/zsh

src_files=('./src/menu.cpp' './src/validateLog.cpp' './src/rabin_karp.cpp'
    './src/getEvidence.cpp' './src/saveEvidence.cpp' './src/utilities.cpp'
    './src/saveOptionalLog.cpp' './src/getTables.cpp' './src/global.cpp')

g++ ${src_files[@]} -o ../bin/menu
