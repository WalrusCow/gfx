require('final_box')

-- Name, KS, Phong, Alpha, refr
brown = gr.material({0.094, 0.05, 0.003}, {0.02, 0.02, 0.02}, 1000, 1, 1)
lensM = gr.material({0.2, 0.2, 0.2}, {0.02, 0.02, 0.02}, 1e10, .11, 1.55)

wood = gr.texture_material("wood.png", {0, 0, 0}, 1, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)
bluegreen = gr.function_material("blue_green_squares", {0, 0, 0}, 25, 1)

floor:set_material(brown)
back:set_material(bw)
left:set_material(bluegreen)

camera = {2, -3, -1}

mag = gr.node('mag')
scene:add_child(mag)
mag:translate(unpack(camera))
mag:translate(-1.6, -.5, -2)
mag:rotate('Y', 45)
mag:rotate('X', -5)

lens = gr.sphere('lens')
mag:add_child(lens)
lens:set_material(lensM)
lens:scale(0.6, 0.6, 0.04)

cameraDirection = {-2, -0.8, -2}

SIZE = 128
gr.render(scene, 'final_scene.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
