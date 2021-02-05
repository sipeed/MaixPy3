#!/usr/bin/env python
#coding=utf-8

import os,sys
from stat import *

# Declares a collection of file suffix names(.xxx) to be deleted
DelFiles=['pyc', 'log', 'o']

# Declares a collection of folder names that you want to delete
DelFolders=['__pycache__', 'dist', 'build', 'MaixPy3.egg-info']

def PurgeCatalog(path):
    count = 0
    for item in os.listdir(path):
        subpath = os.path.join(path, item)
        mode = os.stat(subpath).st_mode
        if S_ISDIR(mode):
            count += PurgeCatalog(subpath)
        else:
            os.chmod(subpath, S_IREAD|S_IWRITE)
            os.unlink(subpath)
            count += 1
    os.rmdir(path)
    count += 1
    return count
    
def ClearCatalog(path):
    for catalog in os.walk(path):
        print("find files : %s." % catalog[2])
        for file_name in catalog[2]:
            if '.' in file_name:
                file_type  = file_name.rsplit('.', 1)[1]
                if file_type in DelFiles:
                    print('file %s deleted.' % (os.path.join(catalog[0], file_name)))
                    os.remove(os.path.join(catalog[0], file_name))
        print("find folders : %s." % catalog[1])
        for file_folder in catalog[1]:
            print("into file_folder %s." % file_folder)
            if file_folder in DelFolders:
                print("file_folder %s deleted." % os.path.join(catalog[0], file_folder))
                print("files %d deleted." % PurgeCatalog(os.path.join(catalog[0], file_folder)))

if __name__ == '__main__':
    # python %cd%/Clear.py %cd%
    if len(sys.argv) != 2:
        print( 'please add argv path : %s ' % sys.argv[0]) # %cd%
        sys.exit(1)
    ClearCatalog(sys.argv[1])