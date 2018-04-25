#pragma once
#include "TilesetEventLoader.h"
#include "TilesetLoader.h"

namespace ska {
	template <class TilesetLoaderT, class TilesetEventLoaderT, 
		typename = std::enable_if_t<std::is_base_of_v<TilesetLoader, TilesetLoaderT> &&
		std::is_base_of_v<TilesetEventLoader, TilesetEventLoaderT>>>
	struct TilesetCompleteLoader {
		template <class ... Args>
		explicit TilesetCompleteLoader(Args&&...args) : 
			tilesetLoader(std::forward<Args>(args)...),
			tilesetEventLoader(std::forward<Args>(args)...) {
		}
		TilesetLoaderT tilesetLoader;
		TilesetEventLoaderT tilesetEventLoader;
	};
}
