from xmlrpc.server import SimpleXMLRPCServer
from socketserver import ThreadingMixIn
from threading import Thread
import embedded_message as embeded


class MySimpleXMLRPCServer(ThreadingMixIn, SimpleXMLRPCServer):

    def __init__(self, host):
        super().__init__(host, allow_none=True, logRequests=False)
        self.register_introspection_functions()
        self.register_function(self.get_messages, 'get_messages')
        self.register_function(self.clear_messages, 'clear_messages')
        self.register_function(self.get_locations, 'get_locations')
        self.register_function(self.get_pids, 'get_pids')

    def get_messages(self, location):
        print('Location: ' + location)
        return embeded.get_messages(location)

    def clear_messages(self, data):
        print('CLEAR:', data)
        embeded.clear_messages(data)

    def get_locations(slef):
        return embeded.get_locations()

    def get_pids(self):
        return embeded.get_pids()


host = ('', embeded.get_port())
server = MySimpleXMLRPCServer(host)
t = Thread(target=server.serve_forever)
t.start()
