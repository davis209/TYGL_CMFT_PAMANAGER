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

column_width = [65, 65, 65, 700, 65, 65, 150, 150, 150, 65, 150, 150]
auto_resize_columns = True

auto_refresh_after_clear = True

left = None
top = 155
width = 1920
height = 860

# version
component = 'STIS Message Viewer'
version = '0.0.0.0'
build_data = ''
copyright = 'Copyright © Copyright(C) 2023 ST Engineering.'
