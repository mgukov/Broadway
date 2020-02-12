INPUT_DIR=./src
OUTPUT_DIR=./dist


TOTAL_MEMORY=$(shell echo "32 * 1024 * 1024" | bc)
EMCC_OPTS=-O3 -msimd128 --llvm-lto 3 --llvm-opts 3 --memory-init-file 1 -s NO_DYNAMIC_EXECUTION=1 \
	-s NO_FILESYSTEM=1 -s DISABLE_EXCEPTION_CATCHING=1 -s TOTAL_MEMORY=$(TOTAL_MEMORY) \
	-s INVOKE_RUN=0 -s DOUBLE_MODE=0
DEFAULT_EXPORTS:='_free','_malloc', 'ccall', 'cwrap'

AVC_DIR=avc
AVC_BUILD=$(AVC_DIR)/build
AVC_JS=$(OUTPUT_DIR)/avc.js
AVC_OBJ=$(AVC_BUILD)/libavc.a

AVC_EXPORTS:='_broadwayGetMajorVersion', '_broadwayGetMinorVersion', '_broadwayInit', '_broadwayExit', '_broadwayCreateStream', '_broadwayPlayStream', '_broadwayOnHeadersDecoded', '_broadwayOnPictureDecoded'
 

AVC_PUCK=avc.wasm.js

LIB_BROADCASTER_DIR=/home/mgukov/dev/vyu/lib_broadcaster

default: avc_puck

avc_puck: avc_build
	cat before.js $(AVC_JS) after.js > dist/$(AVC_PUCK)

clean:
	rm -rf $(OUTPUT_DIR) $(AVC_BUILD)
	mkdir $(OUTPUT_DIR)
	mkdir $(AVC_BUILD)

avc_obj: clean
	cd $(AVC_BUILD); emconfigure /usr/bin/cmake ..
	cd $(AVC_BUILD); emmake make


avc_build: avc_obj
	emcc $(EMCC_OPTS) -s EXPORT_NAME='"AvcModule"' -s EXPORTED_FUNCTIONS="[$(AVC_EXPORTS)]" -s USE_SDL=2 -s EXTRA_EXPORTED_RUNTIME_METHODS="[$(DEFAULT_EXPORTS)]" $(AVC_OBJ) -Iinc --js-library $(AVC_DIR)/lib.js -o $(AVC_JS)
