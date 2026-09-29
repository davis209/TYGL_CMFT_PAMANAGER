import json
from http.server import HTTPServer, SimpleHTTPRequestHandler
from threading import Thread
import embedded_message as embeded


class Resquest(SimpleHTTPRequestHandler):

    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        print('GET', self.path)

        data = ''
        parts = self.path.split('/')
        match parts[1]:
            case 'get_messages':
                data = embeded.get_messages(parts[2])
            case 'get_locations':
                data = embeded.get_locations()
            case 'get_pids':
                data = embeded.get_pids()
        self.wfile.write(data.encode())

    def do_POST(self):
        data = self.rfile.read(int(self.headers["content-length"]))
        data = json.loads(data)
        print('POST', self.path, data)

        match self.path:
            case '/clear_messages':
                embeded.clear_messages(data)

        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def log_message(self, format, *args):
        pass


host = ('', embeded.get_port())
server = HTTPServer(host, Resquest)
t = Thread(target=server.serve_forever)
t.start()
