#! /bin/bash
# Bash Script that builds project

RED='\033[0;31m' # Red
BB='\033[0;34m'  # Blue
NC='\033[0m' # No Color
BG='\033[0;32m' # Green

error() { >&2 echo -e "${RED}$1${NC}"; }
showinfo() { echo -e "${BG}$1${NC}"; }
workingprocess() { echo -e "${BB}$1${NC}"; }
allert () { echo -e "${RED}$1${NC}"; }

# Building project
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make -j8
# Checks if last comand didn't output 0
# $? checks what last command outputed
# If output is 0 then command is succesfuly executed
# If command fails it outputs number between 0 to 255
if [ $? -ne 0 ]; then
    error "Error: there are compile errors!"
	# Terminate script and outputs 3
    exit 3
fi

./luabz_tests
workingprocess "All tests compile and pass."

#Uncomment if you want to run cppcheck
#workingprocess "Running cppcheck"
#cppcheck --version
#cd ..
#cppcheck  --suppress=missingIncludeSystem --force --enable=all --std=c++11 --language=c++ -i gtest --quiet --error-exitcode=1  . 

#Uncomment if you want to run clang-tidy
#workingprocess "Running clang-tidy"
#cd build
#make clang-tidy > output.txt
#if [[ -n $(grep "warning: " output.txt) ]] || [[ -n $(grep "error: " output.txt) ]]; then
#      echo "You must pass the clang tidy checks before submitting a pull request"
#      echo ""
#      grep --color -E '^|warning: |error: ' output.txt
#      exit -1;
#  else
#      echo -e "\033[1;32m\xE2\x9C\x93 passed:\033[0m $1";
#fi