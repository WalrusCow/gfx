require('readobj')

scene = gr.node('scene')

red = gr.material({1, 0, 0}, {0, 0, 0}, 1)
green = gr.material({0, 1, 0}, {0, 0, 0}, 1)
blue = gr.material({0, 0, 1}, {0, 0, 0}, 1)
white = gr.material({1, 1, 1}, {0, 0, 0}, 1)
black = gr.material({0, 0, 0}, {0, 0, 0}, 1)

roomSize = 10

left = gr.mesh('left', readobj('mymesh.obj'))
scene:add_child(left)
left:set_material(green)
left:rotate('Y', 90)
left:scale(roomSize, roomSize, 1)

right = gr.mesh('right', readobj('mymesh.obj'))
scene:add_child(right)
right:set_material(blue)
right:translate(roomSize, 0, -roomSize)
right:rotate('Y', -90)
right:scale(roomSize, roomSize, 1)

back = gr.mesh('back', readobj('mymesh.obj'))
scene:add_child(back)
back:set_material(red)
back:translate(0, 0, -roomSize)
back:scale(roomSize, roomSize, 1)

floor = gr.mesh('floor', readobj('mymesh.obj'))
scene:add_child(floor)
floor:set_material(white)
floor:rotate('X', -90)
floor:scale(roomSize, roomSize, 1)


roof = gr.mesh('roof', readobj('mymesh.obj'))
scene:add_child(roof)
roof:set_material(black)
roof:translate(0, roomSize, -roomSize)
roof:rotate('X', 90)
roof:scale(roomSize, roomSize, 1)

--roof = gr.mesh('roof', readobj('mymesh.obj'))
--scene:add_child(roof)
--roof:set_material(black)
--roof:rotate('X', 90)
--roof:translate(0, roomSize, 0)
--roof:scale(roomSize, roomSize, 1)

-- Light from the top centre
topLight = gr.light({roomSize / 2, roomSize - 0.01, 0}, {1, 1, 1}, {1, 0, 0})
-- And light from the camera
cameraLight = gr.light({roomSize / 2, roomSize / 2, roomSize}, {0.6, 0.6, 0.6}, {1, 0, 0})

camera = {roomSize / 2, roomSize / 2, roomSize}
--gr.render(scene, 'boxscene.png', 128, 128,
--    camera, {0, 0, -1}, {0, 1, 0}, 50,
--    {0.4, 0.4, 0.4}, {l1})
