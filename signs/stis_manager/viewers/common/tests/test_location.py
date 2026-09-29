import random
from .. import config
from ..transactive.location import Location


class TestLocation:

    def setup_class(self):
        Location.load_from_db()

    def test_occ(self):
        occ = Location.occ()
        assert occ.is_occ()
        assert occ.name == 'OCC'

    def test_this(self):
        config.location = 'js01'
        this = Location.this()
        assert this.name == 'JS01'

    def test_stations(self):
        stations = Location.stations()
        assert len(stations)
        occ = Location.get('occ')
        js01 = Location.get('js01')
        assert occ not in stations
        assert js01 in stations

    def test_stations_and_depots(self):
        stations = Location.stations_and_depots()
        assert len(stations)
        occ = Location.get('occ')
        js01 = Location.get('js01')
        tgd = Location.get('tgd')
        assert occ not in stations
        assert js01 in stations
        assert tgd in stations

    def test_get(self):
        js01 = Location.get('js01')
        assert js01.name == 'JS01'

        assert Location.get('invalid') is None

    def test_eq(self):
        x = Location()
        y = Location()
        assert x == y

        x = Location.get(1)
        y = Location.get(1)
        assert x == y

        x = Location.get(1)
        y = Location.get(2)
        assert x != y

        x = Location.get('invalid')
        y = Location.get(1)
        assert x != y

    def test_lt(self):
        x = Location(1)
        y = Location()
        x.order_id = 1
        y.order_id = 2
        assert x < y

        all = Location.all()
        pre = all[0]
        for x in all[1:]:
            assert pre.order_id <= x.order_id
            pre = x

    def test_sort(self):
        all = Location.all().copy()
        random.shuffle(all)
        all = sorted(all)
        pre = all[0]
        for x in all[1:]:
            assert pre.order_id <= x.order_id
            pre = x

    def test_json(self):
        all = Location.all().copy()
        s = Location.to_json(all)
        all2 = Location.from_json(s)
        assert all == all2

        one = all[0]
        s = Location.to_json(one)
        one2 = Location.from_json(s)
        assert one == one2
