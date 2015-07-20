require('boxscene')

grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0)
yellow = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15)
mirror = gr.material({.1, .1, .1}, {0.8, 0.8, 0.8}, 15)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25)

sphere = gr.sphere('sphere')
sphere:set_material(yellow)
sphere:translate(0, -roomSize / 2 + 2, -4)
scene:add_child(sphere)

floor:set_material(mirror)
back:set_material(bw)
roof:set_material(grey)

gr.render(scene, 'reflScene.png', 128, 128,
    camera, cameraDirection, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {topLight, cameraLight})
