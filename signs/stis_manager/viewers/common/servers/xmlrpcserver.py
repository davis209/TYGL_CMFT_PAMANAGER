from xmlrpc.server import SimpleXMLRPCServer
from .. import config
from ..transactive import message
from ..transactive import template
from ..transactive.location import Location
from ..transactive.pid import Pid

host = ('', config.port)


class MySimpleXMLRPCServer(SimpleXMLRPCServer):

    def __init__(self, host):
        super().__init__(host)
        self.register_function(self.get_messages, "get_messages")
        self.register_function(self.clear_messages, "clear_messages")
        self.register_function(self.get_templates, "get_templates")
        self.register_function(self.remove_templates, "remove_templates")
        self.register_function(self.get_locations, "get_locations")
        self.register_function(self.get_pids, "get_pids")

    def get_messages(self, location):
        print('Location: ' + location)
        return next(message.message_sequence_generator(location))

    def clear_messages(self, data):
        print('CLEAR:', data)
        return 0

    def get_templates(self, location):
        print('Location: ' + location)
        return next(template.template_sequence_generator(location))

    def remove_templates(self, data):
        print('CLEAR:', data)
        return 0

    def get_locations(slef):
        all = Location.load_from_db()
        return Location.to_json(all)

    def get_pids(self):
        all = Pid.load_from_db()
        return Pid.to_json(all)


if __name__ == '__main__':
    server = MySimpleXMLRPCServer(host)
    print("Starting xmlrpc server, listen at: %s:%s" % host)
    server.serve_forever()
