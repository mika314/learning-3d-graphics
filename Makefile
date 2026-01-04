all: FORCE
	sed -i '/no-nan-infinity-disabled/d' bgfx/bx/scripts/toolchain.lua
	make -j 17 -C bgfx/GENie/build/gmake.linux
	cp bgfx/GENie/bin/linux/genie bgfx/bx/tools/bin/linux/
	make -j 17 -C bgfx/bgfx linux-clang-release64
	coddle

FORCE:
