set history save on
set print pretty on
set pagination off

set logging file output.txt
set logging on

set args 0.0.0.0 30000
b main
b connect_client
r

refresh
