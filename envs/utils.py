
import os

def get_srcs(path, file_type=['c', 'cpp', 'cc', 'cxx', 's']):
    _sources = []
    setup_path = os.path.dirname(os.path.abspath(__file__)) + '/../' # envs/
    # print(setup_path)
    for root, dirs, files in os.walk(setup_path + path):
        for file in files:
            _type = file.split(".")
            if len(_type) > 1 and _type[1].lower() in file_type:
                abs_path = root.replace(setup_path, '')
                _sources.append(abs_path + '/' + file)
    # print(_sources)
    return _sources
