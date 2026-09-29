import json
from http.server import HTTPServer, SimpleHTTPRequestHandler
from .. import config
from ..transactive import message
from ..transactive import template
from ..transactive.location import Location
from ..transactive.pid import Pid

host = ('', config.port)


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
                location = parts[2]
                data = next(message.message_sequence_generator(location))
            case 'get_templates':
                location = parts[2]
                data = next(template.template_sequence_generator(location))
            case 'get_locations':
                all = Location.load_from_db()
                data = Location.to_json(all)
            case 'get_pids':
                all = Pid.load_from_db()
                data = Pid.to_json(all)
        self.wfile.write(data.encode())

    def do_POST(self):
        data = self.rfile.read(int(self.headers["content-length"]))
        data = json.loads(data)
        print('POST', self.path, data)

        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    # def log_message(self, format, *args):
    #     pass


if __name__ == '__main__':
    server = HTTPServer(host, Resquest)
    print("Starting http server, listen at: %s:%s" % host)
    server.serve_forever()
