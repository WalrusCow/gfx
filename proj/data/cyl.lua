scene = gr.node('scene')

blue = gr.material({0.25, 0.33, 0.88}, {0.5, 0.4, 0.8}, 25)

cylinder = gr.cylinder('cylinder')
scene:add_child(cylinder)
cylinder:set_material(blue)
cylinder:rotate('X', 90)
cylinder:translate(-2.5, 0, 0)
--scene:rotate('Y', 45)

require('cylinder')
scene:add_child(cyl)
cyl:set_material(blue)
cyl:translate(2.5, -2, 0)
cyl:scale(1, 4, 1)

l1 = gr.light({0, 0, 10}, {0.8, 0.8, 0.8}, {1, 0, 0})

cylinder:scale(1, 1, 2)
for i = 1, 36 do
  --cylinder:scale(1, 1, 0.5)
  --cylinder:rotate('X', 10) -- scale has to be last or else things get fckd
  --cylinder:scale(1, 1, 2)
  scene:rotate('X', 10)
  gr.render(scene, i..'cyl.png', 256, 256,
      {0, 0, 10}, {0, 0, -1}, {0, 1, 0}, 50,
      {0.4, 0.4, 0.4}, {l1})
end
