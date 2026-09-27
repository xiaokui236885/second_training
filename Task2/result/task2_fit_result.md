# Task 2 参数拟合结果

## 模型
ω(t) = b + A·sin(Ωt + φ)

## 估计参数
| 参数 | 数值 | 单位 |
| :--- | :--- | :--- |
| A（振幅） | 0.5478 | rad/s |
| b（平均角速度） | 1.3502 | rad/s |
| Ω（频率参数） | 1.6498 | rad/s |
| φ（相位） | 0.6753 | rad |
| RMSE | 0.0043 | rad/s |

## 方法
- 提取：HSV 掩膜 + 轮廓重心
- 角度：atan2 公式，逐帧展开
- 拟合：scipy.optimize.curve_fit（最小二乘）
- 初值：b0=mean(omega), A0=(max-min)/2, Ω0=2π/4, φ0=0

## 误差指标
- 角速度 RMSE = 0.0043 rad/s
- 有效样本数 = 1440（排除异常后略少）
- 参与计算的帧范围 = 0 ~ 1439

## 交付文件
- tracking_overlay.mp4
- fit_comparison.png
- angular_velocity.png
- residuals.png

## 作者和日期
作者：薛家瑧
学号：2266114524
完成日期：2026年9月27日