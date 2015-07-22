scene = gr.node('scene')

mat = gr.material({0.25, 0.33, 0.88}, {0.5, 0.4, 0.8}, 25, 1)

d = 19
start = -(d + 1) / 2
-- Generate a 3d structure of spheres
for i=0,d do
  for j=0,d do
    for k=0,d do
      local s = gr.sphere(i..j..k..'s')
      s:translate(start+i, start+j, start+k)
      s:scale(0.25, 0.25, 0.25)
      s:set_material(mat)
      scene:add_child(s)
    end
  end
end


l = gr.light({0,0,20},{1,1,1},{1,0,0})

SIZE = 128
gr.render(scene, 'grid.png', SIZE, SIZE,
          {0.1, 0.1, 20}, {0, 0, -1}, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {l})
