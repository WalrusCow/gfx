require('final_box')

-- Name, KS, Phong, Alpha
wood = gr.texture_material("wood.png", {0, 0, 0}, 1, 1)
brown = gr.material({0.094, 0.05, 0.003}, {0.02, 0.02, 0.02}, 1000, 1, 1)

bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)

floor:set_material(brown)
back:set_material(bw)
left:set_material(bw)

-- Camera at middle of box opening
camera = {2, -2.5, -1}
cameraDirection = {-3, -2, -4}
-- Light from the top centre
topLight = gr.light({0, roomSize / 4, -2}, {1, 1, 1}, {1, 0, 0})
-- And light from the camera
cameraLight = gr.light(camera, {1, 1, 1}, {1, 0, 0})

SIZE = 128
gr.render(scene, 'final_scene.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
