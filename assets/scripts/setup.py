import modus          as ml
import modus.memory   as mem

# messages
print(f'# {ml.info.name} | {ml.info.arch!r}-bit | {ml.info.config}')
print(f'# {ml.info.url}')
print('# type \'help\' for a list of commands')
print()

# memory
print('# TEST MEMORY')
s = 'Hello, World!'
p = mem.strcpy(mem.malloc(len(s) + 1), s)
r = mem.record(p)
print(r)
print(mem.strget(p))
print(mem.strget(p, p + r.size))
print(chr(mem.memget(p + r.size - 2)))
print(chr(mem.memget(p, 1)))
print(mem.memget(p, 1, r.size))
mem.free(p)
print(mem.record(p))
print()

# testing
if 0:
    ml.event('key_event', ml.key_code.w, 0, ml.key_state.press, ml.key_mods.none)

    s = ml.getscene()
    e = s.create(ml.entity('My Entity', enabled=True, l=[ 1, 2, 3 ], d={ 'a': 1 }))
    xfm = e.attach(ml.transform())
    xfm.position = [ 1, 2, 3 ]
    spr = e.attach(ml.sprite_renderer())
    spr.set_sprite(ml.get_sprite('My Sprite'))
