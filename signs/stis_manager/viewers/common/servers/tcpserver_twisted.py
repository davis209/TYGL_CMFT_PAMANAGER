from twisted.internet import protocol, reactor
import json
import config
from transactive.location import Location
from transactive.pid import Pid
from transactive import message
from transactive import template

host = ('', config.port)


class MessageServer(protocol.Protocol):

    def connectionMade(self):
        print(self.transport.getPeer(), 'connected')

    def dataReceived(self, data: bytes):
        request, response = json.loads(data.decode()), ''
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
            case 'stop':
                response = 'stop'
        pass
        if response:
            self.transport.write(response.encode())


if __name__ == '__main__':
    factory = protocol.Factory()
    factory.protocol = MessageServer
    reactor.listenTCP(config.port, factory)
    reactor.run()
