require('readobj')

scene = gr.node('scene')

blue = gr.material({0.25, 0.33, 0.88}, {0.5, 0.4, 0.8}, 25, 1)

suzanne = gr.mesh('suzanne', readobj('suzanne.obj'))
scene:add_child(suzanne)
suzanne:translate(0, 0, 0)
suzanne:set_material(blue)
suzanne:rotate('X', 25)

l1 = gr.light({0, 0, 10}, {0.8, 0.8, 0.8}, {1, 0, 0})

SIZE=512
gr.render(scene, 'suzanne.png', SIZE, SIZE,
    {0, 0, 8}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {l1})
