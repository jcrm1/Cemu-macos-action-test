#pragma once
#include "util/helpers/fspinlock.h"

enum class DEBUG_SYMBOL_TYPE
{
	UNDEFINED,
	CODE,
	// big-endian types
	U64,
	U32,
	U16,
	U8,
	S64,
	S32,
	S16,
	S8,
	FLOAT,
	DOUBLE,
};


class DebugSymbolStorage
{
public:
	static void StoreDataType(MPTR address, DEBUG_SYMBOL_TYPE type)
	{
		s_lock.acquire();
		s_typeStorage[address] = type;
		s_lock.release();
	}

	static DEBUG_SYMBOL_TYPE GetDataType(MPTR address)
	{
		s_lock.acquire();
		auto itr = s_typeStorage.find(address);
		if (itr == s_typeStorage.end())
		{
			s_lock.release();
			return DEBUG_SYMBOL_TYPE::UNDEFINED;
		}
		DEBUG_SYMBOL_TYPE t = itr->second;
		s_lock.release();
		return t;
	}

	static void ClearRange(MPTR address, uint32 length)
	{
		s_lock.acquire();
		while (length > 0)
		{
			auto itr = s_typeStorage.find(address);
			if (itr != s_typeStorage.end())
				s_typeStorage.erase(itr);
			address += 4;
			length -= 4;
		}
		s_lock.release();
	}

private:
	static FSpinlock s_lock;
	static std::unordered_map<MPTR, DEBUG_SYMBOL_TYPE> s_typeStorage;
};