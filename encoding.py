import codecs
import os
for dirpath, dnames, fnames in os.walk("./"):
    for f in fnames:
        if f.endswith(".cpp") or f.endswith(".h") or f.endswith(".hpp"):
            f.encode('utf-8')
