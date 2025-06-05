import numpy as np
import matplotlib
import matplotlib.pyplot as plt

matplotlib.use("svg")

angle = np.pi/2
phi = (1+np.sqrt(5))/2

x_cos = np.linspace(0, np.pi/2,100)
y_cos = np.cos(x_cos)
plt.plot(x_cos, y_cos,label='cos(x)')

x_interp = ([angle]+[angle:=angle/phi for _ in range(7)]+[0])[::-1]
y_interp = [np.cos(angle) for angle in x_interp]
plt.plot(x_interp, y_interp, label='interp')
plt.savefig("out")

cos_lerp_table=[]

for i in range(1, len(x_interp)):
  a = (y_interp[i]-y_interp[i-1])/(x_interp[i]-x_interp[i-1])
  b = y_interp[i] -a * x_interp[i]

  cos_lerp_table.append((a*(np.pi/180), b))

for (a, b) in cos_lerp_table:
  print(f"{a}, {b}")

def cos_approx(angle_deg):
  tgt_angle = (angle_deg * np.pi) / 180
  for i, angle in enumerate(x_interp[1:]):
    if tgt_angle < angle:
      a = (y_interp[i]-y_interp[i-1])/(x_interp[i]-x_interp[i-1])
      b = y_interp[i] -a * x_interp[i]

      a0, b0 = cos_lerp_table[i]

      return a0*angle_deg + b0, a * angle_deg*np.pi/180 +b

for angle in range(0,90):
  res1, res0 = cos_approx(angle)
  print(f"cos {np.cos(angle *(np.pi/180))}, approx0 {res0}, approx1 {res1}")