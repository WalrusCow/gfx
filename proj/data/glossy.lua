require('readobj')

scene = gr.node('scene')

bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)
bluegreen = gr.function_material("blue_green_squares", {0, 0, 0}, 25, 1)
mirror = gr.material({.1, .1, .1}, {0.6, 0.6, 0.6}, 15, 1)

back = gr.mesh('back', readobj('mymesh.obj'))
back:translate(-10, -10, -20)
back:scale(20, 20, 1)
back:set_material(bluegreen)
scene:add_child(back)

floor = gr.mesh('floor', readobj('mymesh.obj'))
floor:translate(-10, 0, 0)
floor:rotate('X', -90)
floor:scale(20, 20, 1)
floor:set_material(bw)
scene:add_child(floor)

s = gr.sphere('sphere')
s:set_material(mirror)
s:translate(-1.6, 1, -15)
s:scale(0.75, 0.75, 0.75)
scene:add_child(s)

s2 = gr.sphere('sphere')
s2:set_material(mirror)
s2:translate(1.6, 1, -15)
s2:scale(0.75, 0.75, 0.75)
scene:add_child(s2)

s3 = gr.sphere('sphere')
s3:set_material(mirror)
s3:translate(0, 1, -15)
s3:scale(0.75, 0.75, 0.75)
scene:add_child(s3)


SIZE = 128
camera = {0, 2, -10}
cameraDirection = {0, 0, -1}

cameraLight = gr.light({0, 5, -15}, {0.7, 0.7, 0.7}, {1, 0, 0})

SIZE = 128
gr.render(scene, 'glossy.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {cameraLight})
