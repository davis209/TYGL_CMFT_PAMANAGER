from ... import utils


class TestString:

    def test_iequals(self):
        assert utils.string.iequals('a', 'A')
        assert utils.string.iequals('Hello', 'hello')

    def test_iequals_to(self):
        x = utils.string.iequals_to('hello')
        assert x('Hello')
        assert x('World') == False

    def test_icontains(self):
        assert utils.string.icontains(['hello', 'world'], 'Hello')
        assert utils.string.icontains(['hello', 'world'], 'hello')
        assert utils.string.icontains([], 'hello') == False

    def test_to_lower(self):
        assert utils.string.to_lower('Hello') == 'hello'
        assert utils.string.to_lower(['Hello', 'world']) == ['hello', 'world']

    def test_to_upper(self):
        assert utils.string.to_upper('Hello') == 'HELLO'
        assert utils.string.to_upper(['Hello', 'world']) == ['HELLO', 'WORLD']

    def test_to_casefold(self):
        assert utils.string.to_casefold('Hello') == 'HELLO'.casefold()
        assert utils.string.to_casefold(['Hello', 'world']) == ['HELLO'.casefold(), 'WORLD'.casefold()]

    def test_endswith_any(self):
        assert utils.string.endswith_any('hello', ['llo'])
        assert utils.string.endswith_any('hello', []) == False

    def test_iendswith_any(self):
        assert utils.string.iendswith_any('hello', ['LLO'])
        assert utils.string.iendswith_any('hello', ['llo'])
        assert utils.string.iendswith_any('hello', []) == False

    def test_startswith_any(self):
        assert utils.string.startswith_any('hello', ['hello'])
        assert utils.string.startswith_any('hello', []) == False

    def test_istartswith_any(self):
        assert utils.string.istartswith_any('hello', ['Hello'])
        assert utils.string.istartswith_any('hello', ['hello'])
        assert utils.string.istartswith_any('hello', []) == False

    def test_remove_all(self):
        assert utils.string.remove_all('hello, world', ['hello', 'world']) == ', '

    def test_replace(self):
        assert utils.string.replace('hello, world', {'hello': 'Hello', 'world': 'World'}) == 'Hello, World'

    def test_escape(self):
        assert utils.string.escape(r'hello\world') == r'hello\\world'
        assert utils.string.escape(r'hello\\world') == r'hello\\\\world'
