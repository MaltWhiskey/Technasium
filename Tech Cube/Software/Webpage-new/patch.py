from __future__ import print_function
from os import getcwd, scandir
from os.path import join, isfile

import difflib
import re

patch = """
@@ -66 +66 @@
-#define WEBSOCKETS_MAX_DATA_SIZE (15 * 1024)
+#define WEBSOCKETS_MAX_DATA_SIZE 40969
"""[1::]
folder = join(getcwd(), ".pio", "libdeps")

_no_eol = "\ No newline at end of file"
_hdr_pat = re.compile("^@@ -(\d+),?(\d+)? \+(\d+),?(\d+)? @@$")


def apply_patch(s, patch, revert=False):
    """
    Apply patch to string s to recover newer string.
    If revert is True, treat s as the newer string, recover older string.
    """
    modifier = '\n'+f'// modifid using {patch}'.replace("\n", " \\n ")
    if (s.endswith(modifier)):
        print("not modified")
        return s
    print("modified")
    s = s.splitlines(True)
    p = patch.splitlines(True)
    t = ''
    i = sl = 0
    (midx, sign) = (1, '+') if not revert else (3, '-')
    while i < len(p) and p[i].startswith(("---", "+++")):
        i += 1  # skip header lines
    while i < len(p):
        m = _hdr_pat.match(p[i])
        if not m:
            raise Exception("Bad patch -- regex mismatch [line "+str(i)+"]")
        l = int(m.group(midx))-1 + (m.group(midx+1) == '0')
        if sl > l or l > len(s):
            raise Exception("Bad patch -- bad line num [line "+str(i)+"]")
        t += ''.join(s[sl:l])
        sl = l
        i += 1
        while i < len(p) and p[i][0] != '@':
            if i+1 < len(p) and p[i+1][0] == '\\':
                line = p[i][:-1]
                i += 2
            else:
                line = p[i]
                i += 1
            if len(line) > 0:
                if line[0] == sign or line[0] == ' ':
                    t += line[1:]
                sl += (line[0] != sign)
    t += ''.join(s[sl:])
    return t+modifier


def make_patch(a, b):
    """
    Get unified string diff between two strings. Trims top two lines.
    Returns empty string if strings are identical.
    """
    diffs = difflib.unified_diff(a.splitlines(True), b.splitlines(True), n=0)
    try:
        _, _ = next(diffs), next(diffs)
    except StopIteration:
        pass
    # diffs = list(diffs); print(diffs)
    return ''.join([d if d[-1] == '\n' else d+'\n'+_no_eol+'\n' for d in diffs])
# 
for dirs in [ f.path for f in scandir(folder) if f.is_dir() ]:
    filename = (join(dirs, "WebSockets", "src", "WebSockets.h"))
    with open(filename) as f:
        file = f.read()
    with open(filename, "w") as f:
        file = apply_patch(file, patch)
        f.write(file)
# with open(filename[0:-1]+"m") as f:
    # filem = f.read()
# print(make_patch(file,filem))
