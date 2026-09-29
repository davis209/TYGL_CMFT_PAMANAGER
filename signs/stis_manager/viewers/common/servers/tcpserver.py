import socketserver
from socketserver import ThreadingTCPServer
import json
import threading
from .. import config
from ..transactive.location import Location
from ..transactive.pid import Pid
from ..transactive import message
from ..transactive import template

host = ('', config.port)


class MyTCPHandler(socketserver.StreamRequestHandler):

    def setup(self):
        super().setup()
        print(self.client_address, 'connected')

    def handle(self):
        try:
            for data in iter(self.rfile.readline, ''):
                print(self.client_address, data)
                request, response = json.loads(data), ''
                match request['function']:
                    case 'get_messages':
                        location = request['args']
                        response = next(message.message_sequence_generator(location))
                    case 'get_templates':
                        location = request['args']
                        response = next(template.template_sequence_generator(location))
                    case 'get_locations':
                        all = Location.load_from_db()
                        response = Location.to_json(all)
                    case 'get_pids':
                        all = Pid.load_from_db()
                        response = Pid.to_json(all)
                    case 'clear_messages':
                        pass
                    case 'remove_templates':
                        pass
                if response:
                    self.wfile.write(response.encode())
        except:
            pass

    def finish(self):
        print(self.client_address, 'closed')
        super().finish()


if __name__ == '__main__':
    print("Starting tcp server, listen at: %s:%s" % host)
    server = socketserver.ThreadingTCPServer(host, MyTCPHandler)
    server.serve_forever()
