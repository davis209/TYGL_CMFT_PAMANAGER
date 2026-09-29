import os


def split_dir_stem_ext(p):
    dir_name = os.path.dirname(p)
    basename = os.path.basename(p)
    stem, ext = os.path.splitext(basename)
    return dir_name, stem, ext


def replace_path(p, search, replace):
    if isinstance(p, list):
        res = []
        for i in p:
            res.append(replace_path(i, search, replace))
        return res
    if os.path.isabs(p):
        search = os.path.abspath(search)
        replace = os.path.abspath(replace)
    return os.path.join(replace, os.path.relpath(p, search))


def add_postfix_for_files(p, postfix):
    if isinstance(p, list):
        res = []
        for i in p:
            res.append(add_postfix_for_files(i, postfix))
        return res
    dir, stem, ext = split_dir_stem_ext(p)
    return os.path.join(dir, stem + postfix + ext)
