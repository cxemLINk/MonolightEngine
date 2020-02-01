#pragma once

#include "osbasics.h"
#include "monolight.h"

namespace Monolight{

	namespace Resources
	{
		typedef uint16_t MountID;
		typedef uint32_t VFHandle;

		void init();
		void destroy();

		MountID mountMLR(ustring filepath);
		MountID mountDir(ustring dirpath);
		void unmountMLR(ustring filepath);
		void unmountDir(ustring dirpath);
		void unmount(MountID id);
		
		void enableMount(MountID id);
		void disableMount(MountID id);

		// VFS paths are UTF-8
		VFHandle openFile(std::string path);
		void closeFile(VFHandle handle);
		// TODO: Read/write funcs
		
		// TODO: Loaders
		template<class T>
		T loadAsset(std::string path);

		class Asset {

		};
	}

}
