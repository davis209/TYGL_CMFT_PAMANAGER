from ..transactive import message
from ..transactive.message import Message
from ..transactive import template
from ..transactive.template import Template


class TestMessage:

    def test_json_one(self):
        m = Message()
        m.location = 'OCC'
        s = Message.to_json(m)
        # print(s)
        assert r'"location": "OCC"' in s

        m2 = Message.from_json(s)
        # print(m2)
        assert isinstance(m2, Message)
        assert m2.location == 'OCC'

    def test_json_list(self):
        m1 = Message('OCC')
        m2 = Message('CCK')
        s = Message.to_json([m1, m2])
        # print(s)
        # assert r'"location": "OCC"' in s

        ms = Message.from_json(s)
        assert isinstance(ms, list)
        x1, x2 = ms
        assert isinstance(x1, Message)
        assert isinstance(x2, Message)
        assert x1.location == 'OCC'
        assert x2.location == 'CCK'

    def test_message_sequence_generator(self):
        g1 = message.message_sequence_generator('all')
        g2 = message.message_sequence_generator('all')
        g3 = message.message_sequence_generator('js01')
        g4 = message.message_sequence_generator('invalid')
        assert g1 is g2
        assert g1 is not g3
        assert len(next(g1))
        assert next(g4) == '[]'
