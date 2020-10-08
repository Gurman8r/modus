import libmeme as ml

# setup window
wnd = ml.render_window()
assert(wnd.open(
    "libmeme",                      # title
    ml.video_mode(
        [ 1280, 720 ],              # resolution
        [ 8, 8, 8, 8 ],             # bits per pixel
        -1),                        # refresh rate
    ml.context_settings(
        ml.context_api.opengl,      # api
        4, 6,                       # version
        ml.context_profile.compat,  # profile
        24,                         # depth bits
        8,                          # stencil bits
        True,                       # multisample
        False),                     # sRGB capable
    ml.window_hints.default_max     # hints
    & ~ml.window_hints.doublebuffer
    ))