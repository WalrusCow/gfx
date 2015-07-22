require('boxscene')

grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0, 1)
yellow = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)
monet = gr.texture_material("monet.png", {0, 0, 0}, 25, 1)
t1 = gr.texture_material("t2.png", {0, 0, 0}, 25, 1)

sphere = gr.sphere('sphere')
sphere:set_material(monet)
sphere:translate(0, -roomSize / 2 + 2, -10)
sphere:scale(2, 2, 2)
sphere:rotate('X', 90)
scene:add_child(sphere)

left:set_material(bw)
right:set_material(t1)

roof:set_material(grey)

l1 = gr.light({0, 0, -4}, {1, 1, 1}, {1, 0, 0})

SIZE = 512
gr.render(scene, 'texture.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
