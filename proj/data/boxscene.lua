require('readobj')

scene = gr.node('scene')

red = gr.material({1, 0, 0}, {0, 0, 0}, 1, 1, 1)
green = gr.material({0, 1, 0}, {0, 0, 0}, 1, 1)
blue = gr.material({0, 0, 1}, {0, 0, 0}, 1, 1)
white = gr.material({1, 1, 1}, {0, 0, 0}, 1, 1)
black = gr.material({0, 0, 0}, {0, 0, 0}, 1, 1)
purple = gr.material({0.6, 0, 0.6}, {0, 0, 0}, 15, 1)

-- But the depth is 4 times this
roomSize = 10

boxParent = gr.node('boxContainer')
scene:add_child(boxParent)
boxParent:translate(-roomSize / 2, -roomSize / 2, 0)

left = gr.mesh('left', readobj('mymesh.obj'))
boxParent:add_child(left)
left:set_material(green)
left:rotate('Y', 90)
left:scale(roomSize, roomSize, 1)
left:scale(4, 1, 1)

right = gr.mesh('right', readobj('mymesh.obj'))
boxParent:add_child(right)
right:set_material(blue)
right:translate(roomSize, 0, -4 * roomSize)
right:rotate('Y', -90)
right:scale(roomSize, roomSize, 1)
right:scale(4, 1, 1)

back = gr.mesh('back', readobj('mymesh.obj'))
boxParent:add_child(back)
back:set_material(red)
back:translate(0, 0, -4 * roomSize)
back:scale(roomSize, roomSize, 1)

floor = gr.mesh('floor', readobj('mymesh.obj'))
boxParent:add_child(floor)
floor:set_material(white)
floor:rotate('X', -90)
floor:scale(roomSize, roomSize, 1)
floor:scale(1, 4, 1)

roof = gr.mesh('roof', readobj('mymesh.obj'))
boxParent:add_child(roof)
roof:set_material(black)
roof:translate(0, roomSize, -4 * roomSize)
roof:rotate('X', 90)
roof:scale(roomSize, roomSize, 1)
roof:scale(1, 4, 1)

front = gr.mesh('front', readobj('mymesh.obj'))
boxParent:add_child(front)
front:set_material(purple)
front:translate(roomSize, 0, 0)
front:rotate('Y', 180)
front:scale(roomSize, roomSize, 1)

-- Camera at middle of box opening
camera = {0, -roomSize / 4, -1}
cameraDirection = {0, 0, -1}
-- Light from the top centre
topLight = gr.light({0, roomSize / 4, -2}, {1, 1, 1}, {1, 0, 0})
-- And light from the camera
--cameraLight = gr.light(camera, {0.3, 0.3, 0.3}, {1, 0, 0})
cameraLight = gr.light(camera, {1, 1, 1}, {1, 0, 0})

EP = 1
LEFT = -roomSize / 2 + EP
RIGHT = roomSize / 2 - EP
TOP = roomSize / 2 - EP
BACK = -4 * roomSize + EP
FRONT = -EP

backTopLeft = gr.light({LEFT, TOP, BACK}, {1, 1, 1}, {1, 0, 0})
backTopRight = gr.light({RIGHT, TOP, BACK}, {1, 1, 1}, {1, 0, 0})
frontTopLeft = gr.light({LEFT, TOP, FRONT}, {1, 1, 1}, {1, 0, 0})
frontTopRight = gr.light({RIGHT, TOP, FRONT}, {1, 1, 1}, {1, 0, 0})

roomLights = {backTopLeft, backTopRight, frontTopLeft, frontTopRight}
