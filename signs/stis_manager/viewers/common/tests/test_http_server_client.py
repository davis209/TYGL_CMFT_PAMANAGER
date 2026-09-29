import threading
from http.server import HTTPServer, ThreadingHTTPServer
from .. import config

from ..servers.httpserver import Resquest
from ..clients.httpclient_v1 import HttpClient


class TestHttpServerClient:

    def test_1(self):
        server = HTTPServer(('', config.port), Resquest)
        t = threading.Thread(target=server.serve_forever)
        t.start()

        try:
            client = HttpClient()
            data, error = client.get_messages('ALL')
            assert len(data)

            client.clear_messages('OCC')
            assert 1

            data, error = client.get_locations()
            assert len(data)

            data, error = client.get_pids()
            assert len(data)
        except:
            assert 0
        finally:
            server.shutdown()
            t.join()
