import threading
from .. import config
from ..servers.xmlrpcserver import MySimpleXMLRPCServer
from ..clients.xmlrpcclient import XmlrpcClient


class TestTcpServerClient:

    def test_1(self):
        pass
        server = MySimpleXMLRPCServer(('', config.port))
        t = threading.Thread(target=server.serve_forever, daemon=True)
        t.start()

        try:
            client = XmlrpcClient()
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
            pass
