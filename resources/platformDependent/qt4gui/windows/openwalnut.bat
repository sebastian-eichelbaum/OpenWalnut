:: Add lib and bin to PATH to allow all the libs to be found properly
set PATH=./lib/;./bin/;%PATH%

:: Add externals to PATH
set PATH=./ext;%PATH%

:: Finally, run OpenWalnut
openwalnut-qt4.exe
