red = gr.material({0.5, 0.91, 0.78}, {0.1, 0.1, 0.1}, 10)

root = gr.node('root')

s1 = gr.nh_box('s1', {0, 0, 0}, 1)
--s1:scale(100, 100, 100)
--s1:scale(1, 3, 4)
s1:rotate('X', 48)
s1:rotate('Y', 48)
root:add_child(s1)
s1:set_material(red)

l1 = gr.light({-3, 0, -3}, {1, 1, 1}, {1, 0, 0})
l2 = gr.light({3, 0, 3}, {1, 1, 1}, {1, 0, 0})
l3 = gr.light({2, 2, 2}, {1, 1, 1}, {1, 0, 0})
--l4 = gr.light({-2, -2, -2}, {1, 1, 1}, {1, 0, 0})
--l5 = gr.light({0, 0, 4}, {1, 1, 1}, {1, 0, 0})
lights = {
  l1,
  l2,
  l3,
  l4,
  l5,
}

gr.render(root, 'ez.png', 256, 256,
	  {0, 0, 4}, {0, 0, -4}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, lights)
