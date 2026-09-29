import json
import os
from functools import partial


def customized_json(Class):
    def customized_to_json(cls, x):
        if not x:  # [], {}, ''
            return json.dumps(x)

        if isinstance(x, cls):
            return json.dumps(x, default=lambda obj: obj.__dict__)
        elif isinstance(x, list):
            return json.dumps(list(map(partial(customized_to_json, cls), x)))
        else:
            raise TypeError()

    def customized_from_json(cls, x: str):
        if not x:
            return x

        x = json.loads(x)
        if isinstance(x, dict):
            # o = cls(**x)
            # o = cls()
            o = cls.__new__(cls)
            o.__dict__ = x
            return o
        elif isinstance(x, list):
            return list(map(partial(customized_from_json, cls), x))
        else:
            raise TypeError()

    Class.to_json = partial(customized_to_json, Class)
    Class.from_json = partial(customized_from_json, Class)
    return Class
