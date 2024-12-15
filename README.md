# Problem

There is list of rectangles with different (known) widths and heights.

How to pack those rectangles around center point - like this:

![image](https://github.com/user-attachments/assets/3b1540d1-af9e-4201-b260-be5246ea5e75)

# Algorithm

Divide the area around the central point into 4 traingles of equal area, like this

![image](https://github.com/user-attachments/assets/3424ea8d-c402-4261-9ab3-e6592dbc6b74)

The total area of the four triangles should equal the total area of of the rectangles plus, say, 10%.

Divide the rectangles into 4 subsets of equal total area.

Use a standard bin packing algorithm to pack each triangle with its assigned rectangles.

# Result

Here is how to looks with 50 boxes of random sizes

![image](https://github.com/user-attachments/assets/926e7808-bbc3-4b26-85f3-1f22ad02ff40)



