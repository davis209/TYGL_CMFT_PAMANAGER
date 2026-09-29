import socket

location = 'occ'
# location = 'js03'

# server_type = 'local'
# server_type = 'http'
# server_type = 'xmlrpc'
server_type = 'tcp'
db = r'D:\LocalTest\J155_TIP\database\J155_OCC.ste'

host, port = 'localhost', 10089
host = socket.gethostbyname(host)
timeout = 60
