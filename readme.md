# crts

A real-time-simulation game written in c!

[website](https://mochiro.moe/crts)

# build

## dependencies

+ [meson](https://mesonbuild.com/Getting-meson.html) / a [backend](https://mesonbuild.com/Running-Meson.html)
+ A curses implementation if you want the ncurses ui
+ [cygwin](https://www.cygwin.com/) if you are on windows

## compile

```
meson build
ninja -C build
```

Build options can be displayed using `meson configure`.

## install

```
meson install
```

# usage

If you installed crts, run `crts`, otherwise run `bin/crts` from the source
root.
