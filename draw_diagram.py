import matplotlib.pyplot as plt
import matplotlib.patches as patches

fig, ax = plt.subplots()

# Draw rectangles representing each component
host_rect = patches.Rectangle((0.1, 0.7), 0.3, 0.2, linewidth=1, edgecolor='r', facecolor='none')
ssd_rect = patches.Rectangle((0.5, 0.7), 0.3, 0.2, linewidth=1, edgecolor='g', facecolor='none')
ecc_rect = patches.Rectangle((0.1, 0.4), 0.3, 0.2, linewidth=1, edgecolor='b', facecolor='none')
cpu_rect = patches.Rectangle((0.5, 0.4), 0.3, 0.2, linewidth=1, edgecolor='y', facecolor='none')

# Add rectangles to plot
ax.add_patch(host_rect)
ax.add_patch(ssd_rect)
ax.add_patch(ecc_rect)
ax.add_patch(cpu_rect)

# Label rectangles
plt.text(0.15, 0.75, 'ISDF Library\n(Host Layer)', fontsize=12)
plt.text(0.55, 0.75, 'SSD Controller\n& ISDF Engine', fontsize=12)
plt.text(0.2, 0.45, 'ECC Engine', fontsize=12)
plt.text(0.6, 0.45, 'CPU', fontsize=12)

# Draw arrows
arrowprops = dict(facecolor='black', edgecolor='black', arrowstyle='->')
plt.annotate('', xy=(0.4, 0.8), xytext=(0.5, 0.8), arrowprops=arrowprops)
plt.annotate('', xy=(0.5, 0.6), xytext=(0.4, 0.6), arrowprops=arrowprops)
plt.annotate('', xy=(0.4, 0.5), xytext=(0.5, 0.5), arrowprops=arrowprops)
plt.annotate('', xy=(0.5, 0.3), xytext=(0.4, 0.3), arrowprops=arrowprops)

# Label arrows
plt.text(0.42, 0.81, 'Query', fontsize=12)
plt.text(0.42, 0.61, 'Control Signal', fontsize=12)
plt.text(0.42, 0.51, 'Metadata', fontsize=12)
plt.text(0.42, 0.31, 'Data Retrieval', fontsize=12)

plt.xlim(0, 1)
plt.ylim(0, 1)
plt.axis('off')
plt.show()

