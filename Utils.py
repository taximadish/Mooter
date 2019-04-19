import os, os.path
import subprocess
import sys
import urllib.request
from urllib.error import URLError, HTTPError
import re
from version import __version__
import random
import itertools
import bisect
import logging
import SamCustom

def is_bundled():
    return getattr(sys, 'frozen', False)

def local_path(path=''):
    if local_path.cached_path is not None:
        return os.path.join(local_path.cached_path, path)

    if is_bundled():
        # we are running in a bundle
        local_path.cached_path = os.path.dirname(os.path.realpath(sys.executable))
    else:
        # we are running in a normal Python environment
        local_path.cached_path = os.path.dirname(os.path.realpath(__file__))

    return os.path.join(local_path.cached_path, path)

local_path.cached_path = None


def data_path(path=''):
    if data_path.cached_path is not None:
        return os.path.join(data_path.cached_path, path)

    # Even if it's bundled we use __file__
    # if it's not bundled, then we want to use the source.py dir + Data
    # if it's bundled, then we want to use the extraction dir + Data
    data_path.cached_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "data")

    return os.path.join(data_path.cached_path, path)

data_path.cached_path = None


def default_output_path(path):
    if path == '':
        path = local_path('Output')

    if not os.path.exists(path):
        os.mkdir(path)
    return path


def open_file(filename):
    if sys.platform == 'win32':
        os.startfile(filename)
    else:
        open_command = 'open' if sys.platform == 'darwin' else 'xdg-open'
        subprocess.call([open_command, filename])

def close_console():
    if sys.platform == 'win32':
        #windows
        import win32gui, win32con
        try:
            win32gui.ShowWindow(win32gui.GetForegroundWindow(), win32con.SW_HIDE)
        except Exception:
            pass

def compare_version(a, b):
    if not a and not b:
        return 0
    elif a and not b:
        return 1
    elif not a and b:
        return -1

    sa = a.replace('v', '').replace(' ', '.').split('.')
    sb = b.replace('v', '').replace(' ', '.').split('.')

    for i in range(0,3):
        if int(sa[i]) > int(sb[i]):
            return 1
        if int(sa[i]) < int(sb[i]):
            return -1
    return 0

class VersionError(Exception):
    pass

def check_version(checked_version):
    if SamCustom.IGNORE_VERSION: return
    if compare_version(checked_version, __version__) < 0:
        try:
            with urllib.request.urlopen('http://raw.githubusercontent.com/TestRunnerSRL/OoT-Randomizer/Dev/version.py') as versionurl:
                version = versionurl.read()
                version = re.search(".__version__ = '(.+)'", str(version)).group(1)

                if compare_version(version, __version__) > 0:
                    raise VersionError("You are on version " + __version__ + ", and the latest is version " + version + ".")
        except (URLError, HTTPError) as e:
            logger = logging.getLogger('')
            logger.warning("Could not fetch latest version: " + str(e))

# Shim for the sole purpose of maintaining compatibility with older versions of Python 3.
def random_choices(population, weights=None, k=1):
    pop_size = len(population)
    if (weights is None):
        weights = [1] * pop_size
    else:
        assert (pop_size == len(weights)), "population and weights mismatch"

    CDF = list(itertools.accumulate(weights))

    result = []
    for i in range(k):
        x = random.random() * CDF[-1]
        result.append(population[bisect.bisect(CDF, x)])

    return result


# From the pyinstaller Wiki: https://github.com/pyinstaller/pyinstaller/wiki/Recipe-subprocess
# Create a set of arguments which make a ``subprocess.Popen`` (and
# variants) call work with or without Pyinstaller, ``--noconsole`` or
# not, on Windows and Linux. Typical use::
#   subprocess.call(['program_to_run', 'arg_1'], **subprocess_args())
def subprocess_args(include_stdout=True):
    # The following is true only on Windows.
    if hasattr(subprocess, 'STARTUPINFO'):
        # On Windows, subprocess calls will pop up a command window by default
        # when run from Pyinstaller with the ``--noconsole`` option. Avoid this
        # distraction.
        si = subprocess.STARTUPINFO()
        si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        # Windows doesn't search the path by default. Pass it an environment so
        # it will.
        env = os.environ
    else:
        si = None
        env = None

    # ``subprocess.check_output`` doesn't allow specifying ``stdout``::
    # So, add it only if it's needed.
    if include_stdout:
        ret = {'stdout': subprocess.PIPE}
    else:
        ret = {}

    # On Windows, running this from the binary produced by Pyinstaller
    # with the ``--noconsole`` option requires redirecting everything
    # (stdin, stdout, stderr) to avoid an OSError exception
    # "[Error 6] the handle is invalid."
    ret.update({'stdin': subprocess.PIPE,
                'stderr': subprocess.PIPE,
                'startupinfo': si,
                'env': env })
    return ret


def check_python_version():
    python_version = '.'.join([str(num) for num in sys.version_info[0:3]])
    if compare_version(python_version, '3.6.0') < 0:
        raise Exception('Randomizer requires at least version 3.6 and you are using %s' % python_version)
