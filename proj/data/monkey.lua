require('readobj')

scene = gr.node('scene')

blue = gr.material({0.25, 0.33, 0.88}, {0.5, 0.4, 0.8}, 25)

suzanne = gr.mesh('suzanne', readobj('suzanne.obj'))
scene:add_child(suzanne)
suzanne:translate(1, -1, 0)
suzanne:set_material(blue)

l1 = gr.light({0, 0, 10}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'suzanne.png', 512, 512,
    {0, 0, 8}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {l1})
