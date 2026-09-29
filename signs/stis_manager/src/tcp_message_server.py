import json
import socketserver
from socketserver import ThreadingTCPServer, TCPServer
from threading import Thread
import embedded_message as embeded


class TCPHandler(socketserver.StreamRequestHandler):

    def setup(self):
        super().setup()

    def handle(self):
        try:
            while True:
                data = self.rfile.readline().strip().decode()
                if not data:
                    break
                request = json.loads(data)
                match request['function']:
                    case 'get_messages':
                        data = embeded.get_messages(request['args'])
                        self.wfile.write(bytes(data, "utf-8"))
                    case 'get_locations':
                        data = embeded.get_locations()
                        self.wfile.write(bytes(data, "utf-8"))
                    case 'get_pids':
                        data = embeded.get_pids()
                        self.wfile.write(bytes(data, "utf-8"))
                    case 'clear_messages':
                        embeded.clear_messages(request['args'])
        except:
            pass

    def finish(self):
        super().finish()


host = ('', embeded.get_port())
server = ThreadingTCPServer(host, TCPHandler)
t = Thread(target=server.serve_forever)
t.start()
