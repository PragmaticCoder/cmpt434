set history save on
set print pretty on
set pagination off

set logging file output.txt
set logging on

set args 0.0.0.0 31416

b main

# For client
b connect_client
b chat
b 54

# For server

r

refresh
