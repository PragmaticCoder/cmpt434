set history save on
set print pretty on
set pagination off

set logging file output.txt
set logging on

set args 0.0.0.0 48868

b main

# For client
b connect_client
b chat

# For server

r

refresh
