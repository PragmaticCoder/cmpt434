# This script runs on Python 3
import socket
import time

MAX_PORTS = 65535


def scan_tcp_ports(target_ip='localhost'):
    startTime = time.time()

    target = input('Enter the host to be scanned: ')
    target_ip = socket.gethostbyname(target)

    print('Starting scan on host: ', target_ip)

    for i in range(1, MAX_PORTS):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        sock.settimeout(0.05)
        conn = sock.connect_ex((target_ip, i))

        if(conn == 0):
            print('Port %d: OPEN' % (i,))
        sock.close()

    print('Time taken:', time.time() - startTime)


def main():
    scan_tcp_ports()


if __name__ == "__main__":
    main()
