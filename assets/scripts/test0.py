# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * #

import libmeme          as ml
import libmeme_engine   as engine

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * #

class my_script(ml.script_object):
    def __init__(self, **kwargs):
        return super(my_script, self).__init__(self, **kwargs)
    def on_activate(self): return
    def on_deactivate(self): return
    def on_tick(self): return
    def on_timer(self): return

test = my_script(enabled = True)
test.call("on_tick")

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * #

engine.assets.load("image", "my_image", "../foo.png")
engine.assets.load("texture", "my_texture", "my_image")

# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * #