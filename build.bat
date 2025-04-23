@echo off
IF NOT EXIST bin mkdir bin
IF NOT EXIST bin-int mkdir bin-int


pushd bin-int

cl ..\main.cpp ..\log.cpp /c /WAall /wd4189
cl main.obj Log.obj /link /out:main.exe

popd

move bin-int\main.exe bin\main.exe

