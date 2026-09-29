from common.transactive.location import Location
from common.transactive.pid import Pid
from common.transactive import message
from common.transactive.message import Message
from common.transactive import template
from common.transactive.template import Template

if __name__ == '__main__':
    from client import Client
else:
    from .client import Client


class LocalClient(Client):

    def __init__(self):
        Location.load_from_db()
        Pid.load_from_db()

    def get_messages(self, location) -> (list[Message], str):
        content = next(message.message_sequence_generator(location))
        return Message.from_json(content), None

    def clear_messages(self, destinations) -> str:
        print('CLEAR', destinations)
        return ''

    def get_templates(self, location) -> (list[Template], str):
        content = next(template.template_sequence_generator(location))
        return Template.from_json(content), None

    def remove_templates(self, destinations) -> str:
        print('CLEAR', destinations)
        return ''

    def get_locations(self) -> (list[Location], str):
        return Location.all(), None

    def get_pids(self) -> (list[Pid], str):
        return Pid.all(), None


if __name__ == '__main__':
    c = LocalClient()
    msg, _ = c.get_messages('')
    print(msg)
