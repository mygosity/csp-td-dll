PROJECT=$1
CMAKETOOLCHAIN=

rm -rf project
mkdir project
cd project
# -DCMAKE_TOOLCHAIN_FILE="$CMAKETOOLCHAIN"
# -G "MinGW Makefiles"
# https://stackoverflow.com/questions/46022210/how-to-configure-cmake-for-msys2
# cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug ../src
# cmake -A "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Debug ../src
cmake -DCMAKE_BUILD_TYPE=Debug ../src


# PROJECT=$1
# BASEPATH=$PWD

# rm -rf project
# mkdir project
# cd project
# if [ "${PROJECT}" == "r" ]; then
#     cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$BASEPATH/vcpkg/scripts/buildsystems/vcpkg.cmake ../src
# else
#     cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$BASEPATH/vcpkg/scripts/buildsystems/vcpkg.cmake ../src
# fi

start ./CrispySkiesNative.sln
# start ./project/CrispySkiesNative.sln