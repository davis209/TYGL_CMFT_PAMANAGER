from ..transactive.pid import Pid
from ..transactive.location import Location


class TestPid:

    def setup_class(self):
        Pid.load_from_db()
        Location.load_from_db()

    def test_all(self):
        assert len(Pid.all())

    def test_pid_from_location_id(self):
        pid = Pid.pid_from_location_id('js01', 101)
        assert (pid.location == 'JS01')
        assert (pid.id == '101')

        pid = Pid.pid_from_location_id('cck', 101)
        assert (pid.location == 'JS01')
        assert (pid.id == '101')

        pid1 = Pid.pid_from_location_id('cck', 101)
        pid2 = Pid.pid_from_location_id('cck', 'lcd101')
        assert pid1 == pid2

        assert Pid.pid_from_location_id('invalid', 101) is None
        assert Pid.pid_from_location_id('cck', 999) is None

    def test_pid_from_location(self):
        pids = Pid.pids_from_location('js01')
        assert len(pids)
        assert (pids[0].is_location('JS01'))

        assert len(Pid.pids_from_location('')) == 0
        assert len(Pid.pids_from_location('invalid')) == 0

    def test_pid_from_location_level(self):
        pids = Pid.pids_from_location_level('js01', 'pltd')
        assert len(pids)
        assert (pids[0].location == 'JS01')
        assert (pids[0].level == 'PLTD')

        assert len(Pid.pids_from_location_level('js01', 'invalid')) == 0
        assert len(Pid.pids_from_location_level('invalid', 'pltd')) == 0

    def test_json(self):
        all = Pid.all().copy()
        s = Pid.to_json(all)
        all2 = Pid.from_json(s)
        assert all == all2

        one = all[0]
        s = Pid.to_json(one)
        one2 = Pid.from_json(s)
        assert one == one2
