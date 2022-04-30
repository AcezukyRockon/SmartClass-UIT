REM This is to set up executable path for python (i.e. where python.exe is located)
REM The "." means "here in this directory"
PATH=.;^
C:\Users\Trang\AppData\Local\Programs\Python\Python37\;^
%PATH%

REM This is to make sure "import" of local .py scripts within this directory
set PYTHONPATH=.

python.exe HiveMQ-python.py
pause