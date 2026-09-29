import json
import threading
import time
from twisted.internet import protocol, reactor
import config
from transactive.location import Location
from transactive.pid import Pid
from transactive.message import Message
from transactive.template import Template

if __name__ == '__main__':
    from client import Client
else:
    from .client import Client


class TcpClient(Client, protocol.Protocol):

    def __init__(self):
        self.received_data = ''
        self.error = 'Failed'

    def wait_data(self, timeout=1):
        now = time.time()
        while True:
            if self.received_data:
                break
            time.sleep(0.0001)
            if timeout < time.time() - now:
                self.error = 'timedout'
                break

    def test(self):
        self.get_messages('OCC')
        self.get_locations()
        self.get_pids()
        self.stop()
        # reactor.callFromThread(self.stop_reactor)
        # reactor.stop()

    def connectionMade(self):
        print(self.transport.getPeer(), 'connected')
        t = threading.Thread(target=self.test)
        t.start()

    def dataReceived(self, data: bytes):
        self.received_data = data.decode()
        print(len(self.received_data), self.received_data)
        if self.received_data == 'stop':
            reactor.stop()

    def get_messages(self, location) -> (list[Message], str):
        return self.send('get_messages', location)

    def clear_messages(self, destinations) -> str:
        self.send('clear_messages', destinations)

    def get_templates(self, location) -> (list[Template], str):
        return self.send('get_templates', location)

    def remove_templates(self, destinations) -> str:
        self.send('remove_templates', destinations)

    def get_locations(self) -> (list[Location], str):
        return self.send('get_locations')

    def get_pids(self) -> (list[Pid], str):
        return self.send('get_pids')

    def stop(self):
        return self.send('stop')

    def send(self, function, args=''):
        self.received_data, self.error = '', ''
        request = self.make_request_data(function, args)
        self.transport.write(request)
        self.wait_data()
        return self.received_data, self.error

    @staticmethod
    def make_request_data(function, args=''):
        return bytes(json.dumps({'function': function, 'args': args}) + '\n', 'utf-8')


class MessageClientFactory(protocol.ClientFactory):
    protocol = TcpClient
    clientConnectionLost = clientConnectionFailed = lambda self, connector, reason: reactor.stop()


if __name__ == '__main__':
    reactor.connectTCP(config.host, config.port, MessageClientFactory())
    reactor.run()
