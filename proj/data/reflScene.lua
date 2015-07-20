require('boxscene')

mat1 = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15)

sphere = gr.sphere('sphere')
sphere:set_material(mat1)
sphere:translate(0, 0, -2)
scene:add_child(sphere)

lights = {topLight, cameraLight}
gr.render(scene, 'reflScene.png', 128, 128,
    camera, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {topLight, cameraLight})
