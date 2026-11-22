#!/bin/bash
# Builds SDL3, SDL3_image, SDL3_ttf and SDL3_mixer for the web (Emscripten), installed in <prefix> (static libraries).
# The wasm exceptions and setjmp (FreeType), as the engine (cmake/SkaWeb.cmake : -fwasm-exceptions)
# Usage : tools/web/build_deps.sh <prefix>   (emcmake must be in the PATH : source <emsdk>/emsdk_env.sh)
set -euo pipefail
PREFIX=$(realpath -m "${1:?usage: build_deps.sh <prefix>}")
SOURCES="$PREFIX/src"
mkdir -p "$SOURCES"

fetch() { # <name> <repository> <tag>
	if [ ! -d "$SOURCES/$1" ]; then
		git clone -q --depth 1 --branch "$3" --recurse-submodules --shallow-submodules "$2" "$SOURCES/$1"
	fi
}
build() { # <name> <cmake options...>
	local name=$1; shift
	emcmake cmake -S "$SOURCES/$name" -B "$SOURCES/$name/build-web" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF \
		-DCMAKE_C_FLAGS="-sSUPPORT_LONGJMP=wasm" -DCMAKE_CXX_FLAGS="-fwasm-exceptions" \
		-DCMAKE_INSTALL_PREFIX="$PREFIX" -DCMAKE_PREFIX_PATH="$PREFIX" -DCMAKE_FIND_ROOT_PATH="$PREFIX" "$@" > "$SOURCES/$name.log"
	cmake --build "$SOURCES/$name/build-web" -j"$(nproc)" >> "$SOURCES/$name.log"
	cmake --install "$SOURCES/$name/build-web" >> "$SOURCES/$name.log"
	echo "$name : built"
}

fetch SDL https://github.com/libsdl-org/SDL.git release-3.4.2
fetch SDL_image https://github.com/libsdl-org/SDL_image.git release-3.4.0
fetch SDL_ttf https://github.com/libsdl-org/SDL_ttf.git release-3.2.2
fetch SDL_mixer https://github.com/libsdl-org/SDL_mixer.git release-3.2.4

build SDL -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF
# PNG and the other common formats with stb_image (no external library)
build SDL_image -DSDLIMAGE_BACKEND_STB=ON -DSDLIMAGE_VENDORED=OFF -DSDLIMAGE_AVIF=OFF -DSDLIMAGE_JXL=OFF -DSDLIMAGE_TIF=OFF \
	-DSDLIMAGE_WEBP=OFF -DSDLIMAGE_SAMPLES=OFF -DSDLIMAGE_TESTS=OFF
# FreeType vendored, without HarfBuzz
build SDL_ttf -DSDLTTF_VENDORED=ON -DSDLTTF_HARFBUZZ=OFF -DSDLTTF_PLUTOSVG=OFF -DSDLTTF_SAMPLES=OFF
# Its built-in decoders (wav, ogg vorbis, mp3, flac), as the desktop build (ska CMakeLists)
build SDL_mixer -DSDLMIXER_VENDORED=OFF -DSDLMIXER_FLAC_LIBFLAC=OFF -DSDLMIXER_GME=OFF -DSDLMIXER_MOD=OFF -DSDLMIXER_MIDI=OFF \
	-DSDLMIXER_OPUS=OFF -DSDLMIXER_MP3_MPG123=OFF -DSDLMIXER_VORBIS_VORBISFILE=OFF -DSDLMIXER_WAVPACK=OFF \
	-DSDLMIXER_TESTS=OFF -DSDLMIXER_EXAMPLES=OFF
