from ... import utils


class TestUtil:

    def test_flatten(self):
        x = ['hello', ['world'], (42,)]
        # print(list(utils.util.flatten(x)))
        assert list(utils.util.flatten(x)) == ['hello', 'world', 42]
