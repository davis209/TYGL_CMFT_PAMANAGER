# import utils.json
from ... import utils
from ...utils import json


class TestJson:

    def test_1(self):
        @utils.json.customized_json
        class X:
            def __init__(self):
                self.a = 'hello'
                self.b = 'world'
                self.c = 42

            def __eq__(self, other):
                return self.a, self.b, self.c == other.a, other.b, other.c

        x = X()
        s = X.to_json(x)
        # print(s)
        x2 = X.from_json(s)

        xx = [x, x]
        s = X.to_json(xx)
        xx2 = X.from_json(s)
        assert xx == xx2

    # def test_2(self):
    #     @utils.json.customized_json
    #     class X:
    #         def __init__(self, a, b, c):
    #             self.a = a
    #             self.b = b
    #             self.c = c
    #
    #         def __eq__(self, other):
    #             return self.a, self.b, self.c == other.a, other.b, other.c
    #
    #     x = X('hello', 'world', 42)
    #     s = X.to_json(x)
    #     print(s)
    #     x2 = X.from_json(s)
    #
    #     xx = [x, x]
    #     s = X.to_json(xx)
    #     xx2 = X.from_json(s)
    #     assert xx == xx2
