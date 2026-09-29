import operator
import json
from dataclasses import dataclass
from functools import total_ordering, partial, partialmethod
import sqlite3
from .. import config
import common.utils
from common.utils.string import icontains, iequals

_all_locations = []


@dataclass
@total_ordering
@common.utils.json.customized_json
class Location:
    key: int = 0
    name: str = ''
    description: str = ''
    order_id: int = 0
    display_name: str = ''
    type_name: str = ''

    def is_occ(self):
        return iequals(self.type_name, 'OCC')

    def is_station(self):
        return iequals(self.type_name, 'STATION')

    def is_depot(self):
        return iequals(self.type_name, 'DEPOT')

    def is_station_or_depot(self):
        return self.is_station() or self.is_depot()

    def is_name_or_display_name(self, x: str):
        return icontains([self.name, self.display_name], x)

    def is_type(self, type_name):
        return iequals(self.type_name, type_name)

    def __eq__(self, other):
        if isinstance(other, Location):
            return self.key == other.key
        elif isinstance(other, (int, str)):
            loc = Location.get(other)
            return loc and loc.key == self.key

    def __lt__(self, other):
        return self.order_id < Location.get(other).order_id

    @staticmethod
    def this():
        return Location.get(config.location)

    @staticmethod
    def set_this(location):
        config.location = location

    @staticmethod
    def occ():
        return Location.get('occ')

    @staticmethod
    def tgd():
        return Location.get('tgd')

    @staticmethod
    def set(locations):
        global _all_locations
        _all_locations = locations

    @staticmethod
    def all():
        return _all_locations

    @staticmethod
    def all_names():
        return [x.name for x in _all_locations]

    @staticmethod
    def all_display_names():
        return [x.display_name for x in _all_locations]

    @staticmethod
    def station_and_depot_names():
        return [x.name for x in Location.stations_and_depots()]

    @staticmethod
    def station_and_depot_display_names():
        return [x.display_name for x in Location.stations_and_depots()]

    @staticmethod
    def get(x):
        res = None
        if isinstance(x, int):
            res = [loc for loc in _all_locations if loc.key == x]
        elif isinstance(x, str):
            if x.isdigit():
                return Location.get(int(x))
            else:
                res = [loc for loc in _all_locations if loc.is_name_or_display_name(x)]
        elif isinstance(x, Location):
            return x
        return res[0] if res else None

    @staticmethod
    def get_by_type(type_name: str):
        return [x for x in _all_locations if x.is_type(type_name)]

    @staticmethod
    def stations():
        return Location.get_by_type('STATION')

    @staticmethod
    def depots():
        return Location.get_by_type('DEPOT')

    @staticmethod
    def stations_and_depots():
        return [x for x in _all_locations if x.is_station_or_depot()]

    @staticmethod
    def all_json():
        return json.dumps(_all_locations)

    @staticmethod
    def load_from_db(*, db=None, force=False):

        if not force and _all_locations:
            return _all_locations

        locations = []
        conn = sqlite3.connect(db or config.db)
        c = conn.cursor()
        rows = c.execute('SELECT pkey, name, description, order_id, display_name, type_name FROM location')
        for row in rows:
            location = Location(*row)
            if location.key == 0:
                continue
            locations.append(location)
        # locations = sorted(locations, key=lambda x: x.order_id)
        # locations = sorted(locations, key=operator.attrgetter('order_id'))
        locations.sort(key=operator.attrgetter('order_id'))
        Location.set(locations)
        return locations


if __name__ == '__main__':
    all = Location.load_from_db()
    # print(all)
    print(Location.to_json(all))
