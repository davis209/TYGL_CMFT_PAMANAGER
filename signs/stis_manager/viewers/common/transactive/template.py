import random
import functools
from functools import total_ordering
from dataclasses import dataclass
import common.utils.json
from common.utils.datetime import make_random_datetime

if __name__ == '__main__':
    from common.transactive.pid import Pid
    from common.transactive.location import Location
else:
    from .pid import Pid
    from .location import Location

PID_TYPES = ['LCD']
TEMPLATE_TYPES = ['LCD Display Normal template', 'LCD Display Emergency template']


@dataclass
@total_ordering
@common.utils.json.customized_json
class Template:
    location: str = ''
    level: str = ''
    pid: str = ''
    display_template_type: str = ''
    id: str = ''
    template_start_datetime: str = ''
    template_end_datetime: str = ''

    def __eq__(self, other):
        return vars(self) == vars(other)

    def __lt__(self, other):
        return vars(self) < vars(other)


@dataclass
@total_ordering
@common.utils.json.customized_json
class PredefinedDisplayTemplate:
    display_template_type: str = ''
    display_template_id: str = ''
    start_time: str = ''
    end_time: str = ''

    def __eq__(self, other):
        return vars(self) == vars(other)

    def __lt__(self, other):
        return vars(self) <= vars(other)


def template_generator(location_filter=''):
    Location.load_from_db()
    Pid.load_from_db()

    location_filter = location_filter.strip().upper()
    if location_filter in ['', 'ALL']:
        pids = Pid.all()
    else:
        pids = Pid.pids_from_location(location_filter)

    while not pids:
        yield None

    while True:
        pid = random.choice(pids)
        t = Template()
        t.location = Location.get(pid.location).display_name
        t.level = pid.level
        t.pid = pid.name
        t.display_template_type = random.choice(TEMPLATE_TYPES)
        t.id = str(random.randint(1, 1000)).zfill(4)
        t.template_start_datetime = make_random_datetime()
        t.template_end_datetime = make_random_datetime()
        yield t


@common.utils.json.customized_json
class RemoveTemplateArg:
    def __init__(self):
        self.location: str = ''
        self.pids = []
        self.display_template_list = []


@functools.lru_cache()
def template_sequence_generator(location_filter='', max_templates=100):
    tpl_gen = template_generator(location_filter)
    while True:
        res = []
        for t in range(random.randint(1, max_templates)):
            tpl = next(tpl_gen)
            if tpl:
                res.append(tpl)
        yield Template.to_json(res)


def display_template_type_to_str(tt: str):
    if not tt.isdigit():
        return tt

    match tt:
        case '0':
            return 'Default'
        case '1':
            return 'LCDEmergency'
        case '2':
            return 'Spare'  # Reserved (no LED for TYGL CMFT)
        case '3':
            return 'LCDNormal'
        case '4':
            return 'Spare'  # Reserved for future


def display_template_type_to_number(tt: str):
    if tt.isdigit():
        return tt

    match tt:
        case 'Default':
            return '0'
        case 'LCDEmergency':
            return '1'
        case 'LCDNormal':
            return '3'
        case 'Spare':
            return '4'


if __name__ == '__main__':
    templates_generator = template_sequence_generator(max_templates=10)
    tpl = next(templates_generator)
    print(tpl)
    for tpl in Template.from_json(tpl):
        print(tpl)
