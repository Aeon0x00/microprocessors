import time

import matplotlib.gridspec as gridspec
import matplotlib.pyplot as plt
import numpy as np
import serial
from mpl_toolkits.mplot3d import Axes3D

PORT = "/dev/ttyUSB0"
BAUD = 115200

min_fi = 0
max_fi = 180
min_tetta = 5
max_tetta = 30
steps = 20
speed = 9
MAXVAL = 2000  

cmd = f"{min_fi} {max_fi} {min_tetta} {max_tetta} {steps} {speed}\n"
total_points = steps * steps
data = []

print("Подключаемся к лидару...")
try:
    with serial.Serial(PORT, BAUD, timeout=15) as ser:
        time.sleep(2)
        ser.reset_input_buffer()
        ser.write(cmd.encode("utf-8"))
        print(f"Сканирование {steps}x{steps} точек...")

        while len(data) < total_points:
            line = ser.readline().decode("utf-8").strip()
            if line.isdigit():
                val = int(line)
                data.append(val)
                pct = len(data) / total_points * 100
                bar = "█" * int(pct // 5) + "░" * (20 - int(pct // 5))
                print(f"[{bar}] {pct:.0f}% | {val} мм", end="\r")
    print("\nГотово!")
except Exception as e:
    print(f"\nОшибка: {e}")
    exit()

grid = np.array(data).reshape((steps, steps)).astype(float)

phi_angles = np.linspace(min_fi, max_fi, steps)
tetta_angles = np.linspace(min_tetta, max_tetta, steps)

X = np.zeros((steps, steps))
Y = np.zeros((steps, steps))
Z = np.zeros((steps, steps))
mask = np.zeros((steps, steps), dtype=bool)
xs, ys, zs, rs = [], [], [], []

for i, phi_deg in enumerate(phi_angles):
    for j, tetta_deg in enumerate(tetta_angles):
        r = grid[i, j]
        phi_rad = np.radians(phi_deg)
        tetta_rad = np.radians(tetta_deg)

        x = r * np.sin(tetta_rad) * np.cos(phi_rad)
        y = r * np.sin(tetta_rad) * np.sin(phi_rad)
        z = r * np.cos(tetta_rad)

        X[i, j] = x
        Y[i, j] = y
        Z[i, j] = z

        if r < MAXVAL:
            xs.append(x)
            ys.append(y)
            zs.append(z)
            rs.append(r)
        else:
            mask[i, j] = True

xs, ys, zs, rs = np.array(xs), np.array(ys), np.array(zs), np.array(rs)

fig = plt.figure(figsize=(22, 10), facecolor="#080818")
gs = gridspec.GridSpec(2, 3, figure=fig, hspace=0.35, wspace=0.3)


def style_3d(ax, x_data, y_data, z_data):
    ax.set_facecolor("#080818")
    ax.xaxis.pane.fill = ax.yaxis.pane.fill = ax.zaxis.pane.fill = False
    ax.tick_params(colors="#aaaaaa", labelsize=8)
    if len(x_data) > 0:
        ax.set_xlim(x_data.min() - 10, x_data.max() + 10)
        ax.set_ylim(y_data.min() - 10, y_data.max() + 10)
        ax.set_zlim(z_data.min() - 10, z_data.max() + 10)


ax1 = fig.add_subplot(gs[0, 0])
color_map_data = np.where(grid > MAXVAL, MAXVAL, grid)
im = ax1.imshow(color_map_data, cmap="turbo", interpolation="nearest", aspect="auto")
for i in range(steps):
    for j in range(steps):
        val = grid[i, j]
        lbl = int(val) if val < MAXVAL else 8191
        ax1.text(j, i, str(lbl), ha="center", va="center", color="white", fontsize=7)
plt.colorbar(im, ax=ax1, label="мм")
ax1.set_title("Карта глубин", color="white")

ax2 = fig.add_subplot(gs[0, 1], projection="3d")
if len(xs) > 0:
    sc = ax2.scatter(
        xs, ys, zs, c=rs, cmap="turbo", s=50, alpha=1, edgecolors="white", linewidth=0.2
    )
    style_3d(ax2, xs, ys, zs)
    plt.colorbar(sc, ax=ax2, shrink=0.6)
ax2.set_title("3D Облако точек (Зум)", color="white")

ax3 = fig.add_subplot(gs[0, 2], projection="3d")
if len(xs) > 0:
    Xm = np.ma.array(X, mask=mask)
    Ym = np.ma.array(Y, mask=mask)
    Zm = np.ma.array(Z, mask=mask)
    surf = ax3.plot_surface(Xm, Ym, Zm, cmap="turbo", edgecolor="none", alpha=0.8)
    style_3d(ax3, xs, ys, zs)
ax3.set_title("3D Поверхность", color="white")

ax4 = fig.add_subplot(gs[1, 0])
if len(xs) > 0:
    ax4.scatter(xs, ys, c=rs, cmap="turbo", s=30)
    ax4.set_facecolor("#0d0d1a")
ax4.set_title("Вид сверху (X-Y)", color="white")

ax5 = fig.add_subplot(gs[1, 1])
if len(xs) > 0:
    ax5.scatter(ys, zs, c=rs, cmap="turbo", s=30)
    ax5.set_facecolor("#0d0d1a")
ax5.set_title("Вид сбоку (Y-Z)", color="white")

ax6 = fig.add_subplot(gs[1, 2])
ax6.set_facecolor("#0d0d1a")
ax6.axis("off")
if len(rs) > 0:
    stats = f"Точек: {len(rs)}\nМин: {rs.min():.0f} мм\nМакс: {rs.max():.0f} мм\nСреднее: {rs.mean():.0f} мм"
    ax6.text(
        0.1,
        0.5,
        stats,
        color="white",
        fontsize=12,
        bbox=dict(facecolor="#1a1a2e", alpha=0.5),
    )

plt.suptitle("Лидар Сканирование", color="white", fontsize=16)
plt.show()
