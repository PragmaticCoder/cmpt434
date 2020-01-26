set history save on
set print pretty on
set pagination off

set logging file output.txt
set logging on

set args 127.0.0.1 30050

# For client
# b main
# b connect_client

# For server
b main
b setup_server

r

refresh
