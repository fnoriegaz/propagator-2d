import sys
import numpy as np
import tifffile

fname = sys.argv[1]
nr = int(sys.argv[2])     # receivers -> width
nt = int(sys.argv[3])     # time samples -> height

data = np.memmap(fname, dtype=np.float32, mode="r", shape=(nt, nr))
CHUNK = 256

# clip value from a strided subsample (affects contrast only)
sample = np.concatenate([
    np.abs(data[i:i + CHUNK].ravel()[::50]) for i in range(0, nt, CHUNK)
])
clip = float(np.percentile(sample, 99))
del sample

# file-backed output: written to disk, not held in RAM
out = tifffile.memmap("gather.tif", shape=(nt, nr), dtype=np.uint8,
                      photometric="minisblack")
scale = 255.0 / (2.0 * clip)
for i in range(0, nt, CHUNK):
    blk = data[i:i + CHUNK].astype(np.float32)
    blk += clip
    blk *= scale
    np.clip(blk, 0, 255, out=blk)
    out[i:i + CHUNK] = blk.astype(np.uint8)
out.flush()
print("saved gather.tif", (nr, nt))
