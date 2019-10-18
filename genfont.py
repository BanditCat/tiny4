from PIL import Image

img = Image.open("font.png")
max = 95

mstr = 'GLuint font[95*2] = {'
for l in range(max):
	i = 0;
	for x in range(4):
		c = 1
		i *= 256
		for y in range(7):
			r, g, b = img.getpixel((3-x,l*8+6-y))
			if b == 0:
				i += c
			c *= 2
	mstr += str(i) + ","
	c = 1
	i = 0
	for y in range(7):
		r, g, b = img.getpixel((4,l*8+6-y))
		if b == 0:
			i += c
		c *= 2
	if l == max-1:
		mstr += str(i) + "};"
	else:
		mstr += str(i) + ","

print(mstr)
	

