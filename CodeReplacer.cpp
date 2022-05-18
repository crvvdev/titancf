#include "Includes.hpp"

namespace CodeReplacer
{
	static void __cdecl ReplacerThread(PVOID)
	{
		for (auto &p : to_replace)
		{
			size_t FunctionSize = size_t(p.second - p.first);

			if (FunctionSize > 0)
			{
				for (size_t i = 0; i < FunctionSize; ++i)
					p.first[i] ^= BYTE(Tools::NtRand(GetTickCount()));

#ifdef _DEBUG
				printf("Replaced function at: %p ends at: %p\n", p.first, p.second);
#endif
			}
		}
	}

	void SetupReplacer(DWORD start, DWORD end)
	{
		MEMORY_BASIC_INFORMATION mbi{ };

		DWORD ptr = start;
		do
		{
			if (VirtualQuery(PVOID(ptr), &mbi, sizeof mbi))
			{
				if (  
					((mbi.Protect & PAGE_READWRITE) || (mbi.Protect & PAGE_EXECUTE_READWRITE) || (mbi.Protect & PAGE_EXECUTE_READ) || (mbi.Protect & PAGE_EXECUTE)) && 
					!(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) && 
					!(mbi.State & MEM_RELEASE))
				{
					auto FunctionStart = Tools::FindSignature(PBYTE(ptr), (PBYTE)"\xEB\x10\x57\x4C\x20\x20\x04\x00\x00\x00\x00\x00\x00\x00\x57\x4C\x20\x20", "xxxxxxx???????xxxx", mbi.RegionSize);
					if (FunctionStart != nullptr)
					{
						auto FunctionEnd = Tools::FindSignature(FunctionStart, (PBYTE)"\xEB\x10\x57\x4C\x20\x20\x05\x00\x00\x00\x00\x00\x00\x00\x57\x4C\x20\x20", "xxxxxxx???????xxxx", mbi.RegionSize);
						if (FunctionEnd != nullptr)
						{
#ifdef _DEBUG
							printf("Found a function at: %p ends at: %p\n", FunctionStart - Globals::own_module, FunctionEnd - Globals::own_module);
#endif
							to_replace.push_back({ FunctionStart, FunctionEnd });
						}
					}
				}
				ptr += mbi.RegionSize;
			}
		} while (ptr < start + end);


		if (!to_replace.empty())
			_beginthread(ReplacerThread, 0, 0);
	}
};