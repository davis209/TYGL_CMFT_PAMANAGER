from functools import total_ordering, partial, partialmethod
from dataclasses import dataclass
import sqlite3
from first import first
from .. import config
import common.utils
from common.utils.string import iequals, icontains

if __name__ == '__main__':
    from common.transactive.location import Location
else:
    from .location import Location

_all_pids = []


@dataclass
@total_ordering
@common.utils.json.customized_json
class Pid:
    asset: str = ''  # entity-name
    location: str = ''  # JS01
    level: str = ''  # CONC
    name: str = ''  # LCD101
    id: str = ''  # 101

    def is_location(self, location):
        Location.load_from_db()
        return Location.get(self.location) == Location.get(location)

    def is_level(self, level):
        return iequals(self.level, level)

    def is_id(self, id_):
        return icontains([self.id, self.name], str(id_))

    def is_location_level(self, location, level):
        return self.is_location(location) and self.is_level(level)

    def is_location_id(self, location, id_):
        return self.is_location(location) and self.is_id(id_)

    def __eq__(self, other):
        return vars(self) == vars(other)

    def __lt__(self, other):
        return self.asset < other.asset

    @staticmethod
    def load_from_db(*, db=None, force=False):
        global _all_pids

        if not force and _all_pids:
            return _all_pids

        pids = []
        conn = sqlite3.connect(db or config.db)
        c = conn.cursor()
        rows = c.execute(r"SELECT name, locationname FROM entity_v WHERE name like '%.TIS.%'")
        for row in rows:
            asset = row[0]
            parts = asset.split('.')

            if len(parts) != 4:
                continue

            location = parts[0]
            level = parts[2]
            name = parts[3]

            if location.startswith('OCC_'):
                continue

            if len(name) != 6:
                continue

            type_ = name[0:3]
            if type_ not in ['LCD', 'LED', 'PDP']:
                continue

            assert location == row[1]

            id = name[3:]

            pid = Pid()
            pid.asset = asset
            pid.location = location
            pid.name = name
            pid.level = level
            pid.id = id

            pids.append(pid)

        Pid.set(pids)
        return pids

    @staticmethod
    def set(pids):
        global _all_pids
        _all_pids = pids

    @staticmethod
    def all():
        return _all_pids

    @staticmethod
    def pids_from_location(location):
        return [x for x in _all_pids if x.is_location(location)]

    @staticmethod
    def pids_from_location_level(location, level):
        return [x for x in _all_pids if x.is_location_level(location, level)]

    @staticmethod
    def pid_from_location_id(location, id_):
        return first(_all_pids, key=lambda x: x.is_location_id(location, id_))


if __name__ == '__main__':
    Location.load_from_db()
    all_ = Pid.load_from_db()
    print(all_)
    pid = Pid.pid_from_location_id('JS01', '102')
    pids = Pid.pids_from_location('JS01')

    for pid in pids:
        print(Pid.to_json(pid))

    s = Pid.to_json(all_)
    all2 = Pid.from_json(s)
