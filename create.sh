PROJECT=$1
CMAKETOOLCHAIN=

rm -rf project
mkdir project
cd project
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug "$CMAKETOOLCHAIN" ../src