import re


def iequals(x: str, y: str):
    return x.casefold() == y.casefold()


def iequals_to(x: str):
    return lambda y: x.casefold() == y.casefold()


def icontains(container: list, x: str):
    return x.casefold() in to_casefold(container)


def to_lower(x: str | list):
    if isinstance(x, list):
        return [i.lower() for i in x]
    return x.lower()


def to_upper(x: str | list):
    if isinstance(x, list):
        return [i.upper() for i in x]
    return x.upper()


def to_casefold(x: str | list):
    if isinstance(x, list):
        return [i.casefold() for i in x]
    return x.casefold()


def endswith_any_v1(text: str, text_list):
    for x in text_list:
        if text.endswith(x):
            return True
    return False


def endswith_any(text: str, text_list):
    return any(map(lambda x: text.endswith(x), text_list))


def iendswith_any(text: str, text_list: list):
    return endswith_any(to_casefold(text), to_casefold(text_list))


def startswith_any_v1(text: str, text_list):
    for x in text_list:
        if text.startswith(x):
            return True
    return False


def startswith_any(text: str, text_list):
    return any(map(lambda x: text.startswith(x), text_list))


def istartswith_any(text: str, text_list):
    return startswith_any(to_casefold(text), to_casefold(text_list))


def remove_all(text: str, remove_list: list):
    for x in remove_list:
        text = text.replace(x, '')
    return text


def replace(text: str, table: dict):
    for k, v in table.items():
        text = text.replace(k, v)
    return text


def escape(x: str | dict):
    if isinstance(x, str):
        return x.replace('\\', '\\\\')
    if isinstance(x, dict):
        res = {}
        for k, v in x.items():
            k = k.replace('\\', '\\\\')
            v = v.replace('\\', '\\\\')
            res[k] = v
        return res
