import sys
import numpy as np
from PIL import Image

Image.MAX_IMAGE_PIXELS = None

fname = sys.argv[1]
nr = int(sys.argv[2])     # receivers (traces)  -> image width
nt = int(sys.argv[3])     # time samples        -> image height
dt = float(sys.argv[4])   # seconds (unused for the raw image)

# memmap: pages are loaded on demand and can be evicted by the OS
data = np.memmap(fname, dtype=np.float32, mode="r", shape=(nt, nr))

CHUNK = 256  # rows per block (~50 MB for 50000 columns)

# Clip value: 99th percentile of |amplitude|, estimated from a strided
# subsample (every 50th sample) so no big temporary arrays are created.
# This only affects the contrast, not the image resolution.
sample = np.concatenate([
    np.abs(data[i:i + CHUNK].ravel()[::50]) for i in range(0, nt, CHUNK)
])
clip = float(np.percentile(sample, 99))
del sample
print("clip =", clip)

# Full-resolution 8-bit grayscale image (nt x nr bytes)
img = np.empty((nt, nr), dtype=np.uint8)
scale = 255.0 / (2.0 * clip)
for i in range(0, nt, CHUNK):
    blk = data[i:i + CHUNK].astype(np.float32)   # small copy
    blk += clip
    blk *= scale
    np.clip(blk, 0, 255, out=blk)
    img[i:i + CHUNK] = blk.astype(np.uint8)

Image.fromarray(img, mode="L").save("gather.png", compress_level=1)
print("saved gather.png", img.shape[::-1])
