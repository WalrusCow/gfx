require('boxscene')

grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0, 1)
yellow = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15, 1)
mirror = gr.material({.1, .1, .1}, {0.6, 0.6, 0.6}, 15, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)
pink = gr.material({0, 0.75, 0.75}, {0, 0, 0}, 0, 1)

sphere = gr.sphere('sphere')
sphere:set_material(pink)
sphere:translate(0, -roomSize / 2 + 2, -10)
sphere:scale(0.8, 1.3, 0.8)
scene:add_child(sphere)

cube = gr.cube('cube')
cube:set_material(yellow)
cube:translate(-2, -roomSize / 2 + 2, -6)
scene:add_child(cube)

torus = gr.torus('torus', 0.3)
torus:set_material(mirror)
torus:translate(0, -roomSize / 2 + 2, -10)
torus:scale(2.2, 2.2, 1)
torus:rotate('X', 70)
scene:add_child(torus)

--floor:set_material(mirror)
roof:set_material(grey)

l1 = gr.light({0, 0, -4}, {1, 1, 1}, {1, 0, 0})

SIZE = 128
gr.render(scene, 'reflScene2.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
