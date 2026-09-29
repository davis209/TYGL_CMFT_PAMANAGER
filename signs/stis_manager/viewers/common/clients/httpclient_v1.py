import json
import urllib.request
from .. import config
from common.transactive.location import Location
from common.transactive.pid import Pid
from common.transactive.message import Message
from common.transactive.template import Template

if __name__ == '__main__':
    from client import Client
else:
    from .client import Client


class HttpClient(Client):

    def __init__(self):
        pass

    def get_messages(self, location) -> (list[Message], str):
        url = f'http://{config.host}:{config.port}/get_messages/{location}'
        data, error = self.get(url)
        return Message.from_json(data), error

    def clear_messages(self, destinations) -> str:
        url = f'http://{config.host}:{config.port}/clear_messages'
        _, error = self.post(url, destinations)
        return error

    def get_templates(self, location) -> (list[Template], str):
        url = f'http://{config.host}:{config.port}/get_templates/{location}'
        data, error = self.get(url)
        return Template.from_json(data), error

    def remove_templates(self, destinations) -> str:
        url = f'http://{config.host}:{config.port}/remove_templates'
        _, error = self.post(url, destinations)
        return error

    def get_locations(self) -> (list[Location], str):
        Location.load_from_db()
        url = f'http://{config.host}:{config.port}/get_locations'
        data, error = self.get(url)
        return Location.from_json(data), error

    def get_pids(self) -> (list[Pid], str):
        Pid.load_from_db()
        url = f'http://{config.host}:{config.port}/get_pids'
        data, error = self.get(url)
        return Pid.from_json(data), error

    def get(self, url):
        response = urllib.request.urlopen(url)
        status = response.getcode()
        reason = response.msg
        data = response.read().decode()
        print(status, reason, data)
        if status == 200:
            return data, None
        else:
            return None, reason

    def post(self, url, data):
        data = json.dumps(data).encode()
        response = urllib.request.urlopen(url, data=data)
        status = response.getcode()
        reason = response.msg
        data = response.read().decode()
        print(status, reason, data)
        if status == 200:
            return data, None
        else:
            return None, reason


if __name__ == '__main__':
    c = HttpClient()
    c.get_messages('OCC')
    c.clear_messages('OCC')
    c.get_locations()
    c.get_pids()
