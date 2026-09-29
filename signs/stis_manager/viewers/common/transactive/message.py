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

# LOCATIONS = 'JE01,JE02,JE03,JE04,JE05,JE06,JE07,JS01,JS02,JS03,JS04,JS05,JS06,JS07,JS08,JS09,JS10,JS11,JS12,JW01,JW02,JW03,JW04,JW05,OCC,PKF,TGD'.split(',')
LOCATIONS = Location.all_display_names()
PID_TYPES = ['LCD']
MESSAGES = ['No Eating and drinking', 'Mind the gap', 'Please enter normal message\n请输入普通消息\nஒரு பொதுவான செய்தியை உள்ளிடவும்']
TEMPLATE_TYPES = ['LCD Display Normal template', 'LCD Display Emergency template']


@dataclass
@total_ordering
@common.utils.json.customized_json
class Message:
    location: str = ''
    level: str = ''
    pid: str = ''
    display_message: str = ''
    tag: str = ''
    priority: int = 0
    start_datetime: str = ''
    end_datetime: str = ''
    display_template_type: str = ''
    id: str = ''
    template_start_datetime: str = ''
    template_end_datetime: str = ''

    def __eq__(self, other):
        return vars(self) == vars(other)

    def __lt__(self, other):
        return vars(self) == vars(other)


def message_generator(location_filter=''):
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
        m = Message()
        m.location = Location.get(pid.location).display_name
        m.level = pid.level
        m.pid = pid.name
        m.display_message = random.choice(MESSAGES)
        m.tag = str(random.randint(0, 4000)).zfill(4)
        m.priority = random.randint(1, 8)
        m.start_datetime = make_random_datetime()
        m.end_datetime = make_random_datetime()
        m.display_template_type = random.choice(TEMPLATE_TYPES)
        m.id = str(random.randint(1, 1000)).zfill(4)
        m.template_start_datetime = make_random_datetime()
        m.template_end_datetime = make_random_datetime()
        yield m


@functools.lru_cache()
def message_sequence_generator(location_filter='', max_messages=100):
    msg_gen = message_generator(location_filter)
    while True:
        res = []
        for m in range(random.randint(1, max_messages)):
            msg = next(msg_gen)
            if msg:
                res.append(msg)
        yield Message.to_json(res)


if __name__ == '__main__':
    messages_generator = message_sequence_generator(max_messages=10)
    msg = next(messages_generator)
    print(msg)
    for msg in Message.from_json(msg):
        print(msg)
