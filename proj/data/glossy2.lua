require('boxscene')

grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0, 1)
yellow = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)
mirror = gr.material({.1, .1, .1}, {0.6, 0.6, 0.6}, 1e10, 1)
lessmirror = gr.material({.1, .1, .1}, {0.6, 0.6, 0.6}, 25, 1)
nomirror = gr.material({.1, .1, .1}, {0.6, 0.6, 0.6}, 5, 1)
wood = gr.texture_material("wood.png", {0,0,0},1,1)

s = gr.sphere('sphere')
s:set_material(mirror)
s:translate(-3, -roomSize / 2 + 2, -10)
scene:add_child(s)

s2= gr.sphere('s2')
s2:set_material(lessmirror)
s2:translate(0, -roomSize / 2 + 2, -10)
scene:add_child(s2)

s3 =gr.sphere('s2')
s3:set_material(nomirror)
s3:translate(3, -roomSize / 2 + 2, -10)
scene:add_child(s3)

floor:set_material(wood)
back:set_material(grey)
left:set_material(bw)
right:set_material(bw)
roof:set_material(grey)

SIZE = 512
gr.render(scene, 'glossy.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
