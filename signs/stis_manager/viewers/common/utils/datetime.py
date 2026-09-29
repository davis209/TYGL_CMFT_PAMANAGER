import random
from datetime import datetime, timedelta


def make_random_datetime():
    one_day = 1 * 24 * 60 * 60
    one_month = 30 * one_day
    t = datetime.now() + timedelta(seconds=random.randint(0, one_month))
    # return t.strftime('%Y/%m/%d %H:%M:%S')
    return t.strftime('%Y%m%d%H%M%S')


def make_datetime_n(count):
    times = []
    for i in range(count):
        times.append(make_random_datetime())
    return times
