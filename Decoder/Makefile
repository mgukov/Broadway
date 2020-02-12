INPUT_DIR=./src
OUTPUT_DIR=./dist

EMCC_OPTS=-O3 --llvm-lto 1 --memory-init-file 1 -s NO_DYNAMIC_EXECUTION=1 -s NO_FILESYSTEM=1
DEFAULT_EXPORTS:='_free','_malloc', 'ccall', 'cwrap'

AVC_DIR=avc
AVC_JS=$(OUTPUT_DIR)/avc.js
AVC_OBJ=$(AVC_DIR)/build/libavc.a
AVC_EXPORTS:='_broadwayGetMajorVersion', '_broadwayGetMinorVersion', '_broadwayInit', '_broadwayExit', '_broadwayCreateStream', '_broadwayPlayStream', '_broadwayOnHeadersDecoded', '_broadwayOnPictureDecoded'

#'HEAP8', 'HEAP16', 'HEAP32', 

AVC_PUCK=avc.wasm.js

LIB_BROADCASTER_DIR=/home/mgukov/dev/vyu/lib_broadcaster

default: avc_puck

avc_puck: avc_build
	cat before.js $(AVC_JS) after.js > dist/$(AVC_PUCK)

clean:
	rm -rf $(OUTPUT_DIR)
	mkdir $(OUTPUT_DIR)

avc_obj: clean
	cd $(AVC_DIR)/build; emconfigure /usr/bin/cmake ..
	cd $(AVC_DIR)/build; emmake make


avc_build: avc_obj
	emcc $(EMCC_OPTS) -s EXPORT_NAME='"AvcModule"' -s EXPORTED_FUNCTIONS="[$(AVC_EXPORTS)]" -s USE_SDL=2 -s EXTRA_EXPORTED_RUNTIME_METHODS="[$(DEFAULT_EXPORTS)]" $(AVC_OBJ) -Iinc --js-library $(AVC_DIR)/lib.js -o $(AVC_JS)
