letter = [[ 0,1,1,1,0 ],
	    [ 0,0,0,0,1 ],
          [ 0,1,1,1,1 ],
          [ 1,0,0,0,1 ],
          [ 0,1,1,1,1 ],
          [ 0,0,0,0,0 ],
          [ 0,0,0,0,0 ]]

f = open( "font.txt", "w" )
f.write( "test" )
f.close()
i = 0;
for x in range(4):
	c = 1
	i *= 256
	for y in range(7):
		if letter[6-y][3-x] == 1:
			i += c
		c *= 2
print(i)
c = 1
i = 0
for y in range(7):
	if letter[6-y][4] == 1:
		i += c
	c *= 2
print(i)

	

