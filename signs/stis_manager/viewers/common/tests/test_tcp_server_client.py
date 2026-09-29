import threading
from socketserver import ThreadingTCPServer, TCPServer
from .. import config

from ..servers.tcpserver import MyTCPHandler
from ..clients.tcpclient import TcpClient


class TestTcpServerClient:

    def test_1(self):
        # server = socketserver.TCPServer((config.host, config.port), MyTCPHandler)
        server = ThreadingTCPServer(('', config.port), MyTCPHandler)
        t = threading.Thread(target=server.serve_forever, daemon=True)
        t.start()

        try:
            client = TcpClient()
            data = client.get_messages('ALL')
            assert len(data)

            client.clear_messages('ALL')
            assert 1

            data = client.get_locations()
            assert len(data)

            data = client.get_pids()
            assert len(data)
        except:
            assert 0
        finally:
            server.shutdown()
            t.join()
