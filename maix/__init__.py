
__all__ = []

# try:
#   # from .video import camera
#   from . import display, rpycs
#   __all__.extend(['display', 'video', 'rpycs', 'camera'])
# except ModuleNotFoundError as e:
#   pass

# try:
#   from . import nn
#   __all__.extend(['nn'])
# except ModuleNotFoundError as e:
#   pass

try:
    import shutil
    from PIL import ImageShow
    # use fbviewer on linux
    # os.system('ln -s /usr/sbin/fbviewer /usr/sbin/display')
    if shutil.which("fbviewer"):
        class fbViewer(ImageShow.UnixViewer):
            def get_command_ex(self, file, **options):
                command = executable = "fbviewer"
                return command, executable
        ImageShow.register(fbViewer, 0)
except ModuleNotFoundError as e:
    pass
