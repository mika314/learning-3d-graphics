all: FORCE .bins/vs-hello-triangle.bin .bins/fs-hello-triangle.bin
	sed -i '/no-nan-infinity-disabled/d' bgfx/bx/scripts/toolchain.lua
	make -j 17 -C bgfx/GENie/build/gmake.linux
	cp bgfx/GENie/bin/linux/genie bgfx/bx/tools/bin/linux/
	make -j 17 -C bgfx/bgfx linux-clang-release64
	coddle

.bins/vs-hello-triangle.bin: vs-hello-triangle.sc varying.def.sc
	mkdir -p .bins
	bgfx/bgfx/.build/linux64_clang/bin/shadercRelease -f vs-hello-triangle.sc -o .bins/vs-hello-triangle.bin --type vertex --platform linux --profile 430 --varyingdef varying.def.sc

.bins/fs-hello-triangle.bin: fs-hello-triangle.sc varying.def.sc
	mkdir -p .bins
	bgfx/bgfx/.build/linux64_clang/bin/shadercRelease -f fs-hello-triangle.sc -o .bins/fs-hello-triangle.bin --type fragment --platform linux --profile 430 --varyingdef varying.def.sc

FORCE:
