set history save on
set print pretty on
set pagination off

set logging file output.txt
set logging on

set args localhost 8000
b main
r

refresh
