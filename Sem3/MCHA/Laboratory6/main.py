import numpy as np
from lagrange_interpolation import lagrange_poly
from newton_interpolation import newton_poly
import test
import matplotlib.pyplot as plt
import task
from sympy import symbols

x = test.x_1
y = test.y_1
point = test.point_1
x_var = symbols('x')

newton = newton_poly(x, y)
lagrange = lagrange_poly(x, y)

print(f"\tData:\n"
      f"x = {x}\n"
      f"y = {y}")

print('-' * 80)

print(f'\tLagrange interpolation polynomial:\nL(x) = {lagrange}')
print(f'Interpolation point: x = {point}')
print(f'Lagrange poly result on x = {point}\nL({point}) = {float(lagrange.subs(x_var, point)):.4f}')

print('-' * 80)

print(f'\tNewton interpolation polynomial:\nN(x) = {newton}')
print(f'Interpolation point: x = {point}')
print(f'Newton poly result on x = {point}\nN({point}) = {float(newton.subs(x_var, point)):.4f}')

