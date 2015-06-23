red = gr.material({0.91, 0.5, 0.78}, {0.1, 0.1, 0.1}, 10)

root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 100, -500}, 100)
--s1:scale(100, 100, 100)
--s1:scale(1, 3, 4)
root:add_child(s1)
s1:set_material(red)

l1 = gr.light({-3, 0, -3}, {1, 1, 1}, {1, 1, 1})
l2 = gr.light({3, 0, 3}, {1, 1, 1}, {1, 1, 1})
l3 = gr.light({2, 2, 2}, {1, 1, 1}, {1, 1, 1})
lights = {
  l1,
  --l2,
  --l3,
}

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(root, 'ez.png', 256, 256,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
