#!/bin/bash

if [ -d "../bin-int/Debug-linux-x86_64/UnitTests" ] && [ -d "../bin-int/Debug-linux-x86_64/TRAP-UnitTests" ]; then
    lcov --directory ../bin-int/Debug-linux-x86_64/UnitTests --directory ../bin-int/Debug-linux-x86_64/TRAP-UnitTests --zerocounters
fi

ninja -C ../ UnitTests_Debug
# make -C ../ config=debug UnitTests -j$(nproc)

if [ -d "../Coverage" ]; then
    rm -Rf ../Coverage
fi

mkdir -p ../Coverage

lcov --capture --initial --directory ../ --output-file=../Coverage/coverage_baseline.info --exclude /usr --exclude Dependencies --parallel --branch-coverage

cd "../UnitTests"
#Execute UnitTest project (Generates gcov report)
../bin/Debug-linux-x86_64/UnitTests/./UnitTests

#Run lcov
lcov --capture --directory ../ --output-file=../Coverage/coverage.info --exclude /usr --exclude Dependencies --parallel --branch-coverage

#Combine
lcov -a ../Coverage/coverage_baseline.info -a ../Coverage/coverage.info --output-file=../Coverage/coverage_total.info --parallel --branch-coverage

#Generate HTML page
prefix=${PWD%/*}
genhtml --output-directory=../Coverage --demangle-cpp --num-spaces 4 ../Coverage/coverage_total.info --prefix $prefix --parallel --title "TRAP test coverage" --header-title "TRAP test coverage" --footer "© 2023-2024 TrappedGames. All Rights Reserved." -branch-coverage
