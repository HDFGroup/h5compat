#! /bin/sh

while [ $# -gt 0 ]; do
    argstring=$1
    shift
    echo "Found $argstring"
done


if [ -r ${HOME}/snapshots-h5compat/.h5compatrc ]; then
   cp ${HOME}/snapshots-h5compat/.h5compatrc ./api
   cp ${HOME}/snapshots-h5compat/.h5compatrc ./format
fi

if (cd ./api &&\
    ./check_api.sh &&\
    cd ../format &&\
    ./check_format.sh &&\
    cd ..); then
    exit 0
else
    exit 1
fi

