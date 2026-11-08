import numpy as np
import matplotlib.pyplot as plt

nt = 3000   # time samples
nr = 1024 - 20 * 2    # receivers (traces)
dt = 0.001  # seconds

data = np.fromfile("gather.bin", dtype=np.float32)
data = data.reshape(nt, nr).T   # if it looks wrong, try reshape(nt, nr).T

print(data.size)

clip = np.percentile(np.abs(data), 99)
plt.imshow(data.T, cmap="gray", aspect="auto", vmin=-clip, vmax=clip,
           extent=[0, nr, nt*dt, 0])
plt.xlabel("Receiver")
plt.ylabel("Time (s)")
plt.colorbar(label="Amplitude")
plt.savefig("gather.png", dpi=150, bbox_inches="tight")
