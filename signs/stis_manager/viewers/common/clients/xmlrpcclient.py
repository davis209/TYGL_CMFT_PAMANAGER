import socket
import xmlrpc.client
from .. import config
from common.transactive.location import Location
from common.transactive.pid import Pid
from common.transactive.message import Message
from common.transactive.template import Template

if __name__ == '__main__':
    from client import Client
else:
    from .client import Client


class XmlrpcClient(Client):

    def __init__(self):
        self.proxy = xmlrpc.client.ServerProxy(f"http://{config.host}:{config.port}/")

    def get_messages(self, location) -> (list[Message], str):
        try:
            socket.setdefaulttimeout(config.timeout)
            content = self.proxy.get_messages(location)
            print(content)
            return Message.from_json(content), None
        except Exception as e:
            return None, str(e)

    def clear_messages(self, destinations) -> str:
        try:
            socket.setdefaulttimeout(None)
            self.proxy.clear_messages(destinations)
            return ''
        except:
            return "Failed"

    def get_templates(self, location) -> (list[Template], str):
        try:
            content = self.proxy.get_templates(location)
            print(content)
            return Template.from_json(content), None
        except Exception as e:
            return None, str(e)

    def remove_templates(self, destinations) -> str:
        try:
            self.proxy.remove_templates(destinations)
            return ''
        except:
            return "Failed"

    def get_locations(self) -> (list[Location], str):
        try:
            content = self.proxy.get_locations()
            print(content)
            return Location.from_json(content), None
        except Exception as e:
            return None, str(e)

    def get_pids(self) -> (list[Pid], str):
        try:
            content = self.proxy.get_pids()
            print(content)
            return Pid.from_json(content), None
        except Exception as e:
            return None, str(e)


if __name__ == '__main__':
    c = XmlrpcClient()
    c.get_messages('')
