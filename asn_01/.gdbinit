set history save on
set print pretty on
set pagination off

set logging file output.txt
set logging on

set args 127.0.0.1 35875

b main

# For client
# b connect_client
b func
b 29

# For server
# b main

r

refresh
