import json
import socket
from .. import config
from common.transactive.location import Location
from common.transactive.pid import Pid
from common.transactive.message import Message
from common.transactive.template import Template

if __name__ == '__main__':
    from client import Client
else:
    from .client import Client


class TcpClient(Client):

    def __init__(self):
        self.sock = None

    def get_messages(self, location) -> (list[Message], str):
        content, error = self.send_and_receive('get_messages', location)
        return Message.from_json(content), error

    def clear_messages(self, destinations) -> str:
        return self.send_only('clear_messages', destinations)

    def get_templates(self, location) -> (list[Template], str):
        content, error = self.send_and_receive('get_templates', location)
        return Template.from_json(content), error

    def remove_templates(self, destinations) -> str:
        return self.send_only('remove_templates', destinations)

    def get_locations(self) -> (list[Location], str):
        content, error = self.send_and_receive('get_locations')
        return Location.from_json(content), error

    def get_included_locations(self) -> (list[Location], str):
        content, error = self.send_and_receive('get_included_locations')
        return Location.from_json(content), error

    def get_pids(self) -> (list[Pid], str):
        content, error = self.send_and_receive('get_pids')
        return Pid.from_json(content), error

    def get_included_pids(self) -> (list[Pid], str):
        content, error = self.send_and_receive('get_included_pids')
        return Pid.from_json(content), error

    def get_socket(self):
        if not self.sock:
            try:
                self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.sock.connect((config.host, config.port))
            except:
                self.sock = None
        return self.sock

    def send_and_receive(self, function, args=''):
        try:
            sock = self.get_socket()
            if not sock:
                return None, "Failed"

            # discard last timedout messages
            try:
                sock.settimeout(0.1)
                sock.recv(1024 * 1024 * 100)
            except:
                pass

            request = self.make_request_data(function, args)
            sock.sendall(request)
            sock.settimeout(config.timeout)
            content = str(sock.recv(1024 * 1024 * 100), "utf-8").strip()
            print(content[:8 * 1024])

            if content.lower().startswith('error:'):
                return None, content

            return content, None
        except Exception as e:
            self.sock = None
            return None, str(e)

    def send_only(self, function, parameter):
        try:
            sock = self.get_socket()
            if not sock:
                return "Failed"
            request = self.make_request_data(function, parameter)
            sock.settimeout(None)
            sock.sendall(request)
            return None
        except Exception as e:
            self.sock = None
            return str(e)

    @staticmethod
    def make_request_data(function, args=''):
        return bytes(json.dumps({'function': function, 'args': args}) + '\n', 'utf-8')


if __name__ == '__main__':
    c = TcpClient()
    c.get_messages('')
    c.clear_messages('OCC')
    c.get_locations()
    c.get_pids()
