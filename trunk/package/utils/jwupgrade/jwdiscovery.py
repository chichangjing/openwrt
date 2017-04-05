import time
import socket

SENDERIP = '192.168.9.9'
MYPORT = 3721
MYGROUP = '228.228.228.228'

def receiver():
    #create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    #allow multiple sockets to use the same PORT number
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
    #Bind to the port that we know will receive multicast data
    sock.bind((SENDERIP, MYPORT))
    #tell the kernel that we are a multicast socket
    #sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 255)
    #Tell the kernel that we want to add ourselves to a multicast group
    #The address for the multicast group is the third param
    status = sock.setsockopt(socket.IPPROTO_IP,
        socket.IP_ADD_MEMBERSHIP,
        socket.inet_aton(MYGROUP) + socket.inet_aton(SENDERIP));

    sock.setblocking(0)
    while True:
        try:
            data, addr = sock.recvfrom(1024)
        except socket.error, e:
            pass
        else:
            print "Receive data!"

            print "TIME:" , time.time()
            print "FROM: ", addr
            print "DATA: ", data


if __name__ == "__main__":

    receiver()


