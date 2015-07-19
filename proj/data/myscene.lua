require('readobj')

scene = gr.node('scene')

blue = gr.material({0.25, 0.33, 0.88}, {0.5, 0.4, 0.8}, 25)
bw = gr.function_material("bw_squares", {0.6, 0.6, 0.6}, 25)

require('mymesh')
scene:add_child(mymesh)
mymesh:scale(2, 2, 2)
mymesh:set_material(bw)

l1 = gr.light({0, 0, 6}, {0.8, 0.8, 0.8}, {1, 0, 0})

SIZE = 128

for i = 1, 36 do
  mymesh:scale(0.5, 0.5, 0.5)
  mymesh:rotate('X', 10) -- scale has to be last or else things get fckd
  mymesh:scale(2, 2, 2)
  --scene:rotate('X', 10)
  gr.render(scene, i..'myscene.png', SIZE, SIZE,
      {0, 0, 6}, {0, 0, -1}, {0, 1, 0}, 50,
      {0.4, 0.4, 0.4}, {l1})
end
