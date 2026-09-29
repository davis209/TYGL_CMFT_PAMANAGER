import json
import requests
import config
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
        try:
            res = requests.get(url, timeout=config.timeout)
            data = res.text
            print(res.status_code, res.reason, data)
            if res.status_code == requests.codes.ok:
                return data, None
            else:
                return None, res.reason
        except Exception as e:
            return None, str(e)

    def post(self, url, data):
        try:
            data = json.dumps(data).encode()
            res = requests.post(url, data=data, timeout=config.timeout)
            data = res.text
            print(res.status_code, res.reason, data)
            if res.status_code == requests.codes.ok:
                return data, None
            else:
                return None, res.reason
        except Exception as e:
            return None, str(e)


if __name__ == '__main__':
    c = HttpClient()
    c.get_messages('')
    c.clear_messages('OCC')
    c.get_locations()
    c.get_pids()
