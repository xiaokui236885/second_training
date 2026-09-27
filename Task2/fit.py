import numpy as np
import pandas as pd
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt

# ---- 1. 读数据 ----
df = pd.read_csv('result/angles.csv')
t = df['frame'].values / 60.0
angle = df['angle'].values

# ---- 2. 算角速度 ----
angle_smooth = np.convolve(angle, np.ones(5)/5, mode='valid')
t_smooth = t[2:-2]

dt = np.diff(t_smooth)
omega_raw = np.diff(angle_smooth) / dt
t_omega = t_smooth[1:]

# ---- 滑动平均滤波（窗口 10 帧）----
window = 10
omega = np.convolve(omega_raw, np.ones(window)/window, mode='valid')
t_omega_smooth = t_omega[window//2 : window//2 + len(omega)]

# ---- 过滤异常值 ----
mask = (omega > 0.5) & (omega < 3.0)
omega = omega[mask]
t_omega_smooth = t_omega_smooth[mask]

# ---- 3. 拟合模型 ----
def model(t, b, A, Omega, phi):
    return b + A * np.sin(Omega * t + phi)

# 更好的初值
b0 = np.mean(omega)
A0 = (np.max(omega) - np.min(omega)) / 2
Omega0 = 2 * np.pi / 4
phi0 = 0.0

p0 = [b0, A0, Omega0, phi0]
popt, _ = curve_fit(model, t_omega_smooth, omega, p0=p0, maxfev=20000)
b, A, Omega, phi = popt

# ---- 4. 算 RMSE ----
omega_fit = model(t_omega_smooth, *popt)
rmse = np.sqrt(np.mean((omega - omega_fit) ** 2))
print(f"b={b:.4f}, A={A:.4f}, Omega={Omega:.4f}, phi={phi:.4f}")
print(f"RMSE={rmse:.4f}")

# ---- 5. 生成图表 ----
fig, ax = plt.subplots()
ax.scatter(t_omega_smooth, omega, s=1, label='观测值(平滑后)')
ax.plot(t_omega_smooth, omega_fit, 'r', label='拟合曲线')
ax.set_xlabel('Time (s)'); ax.set_ylabel('Angular Velocity (rad/s)')
ax.legend(); plt.savefig('result/task2_fit/fit_comparison.png')

plt.figure(); plt.plot(t_omega_smooth, omega_fit)
plt.xlabel('Time (s)'); plt.ylabel('Angular Velocity (rad/s)')
plt.savefig('result/task2_fit/angular_velocity.png')

plt.figure(); plt.scatter(t_omega_smooth, omega - omega_fit, s=1)
plt.xlabel('Time (s)'); plt.ylabel('Residual (rad/s)')
plt.savefig('result/task2_fit/residuals.png')