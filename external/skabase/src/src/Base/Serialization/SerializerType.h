#pragma once

#include "SerializerOutput.h"

namespace ska {
	
	template <class Type>
	struct SerializerTypeTraits {};

	template <class Type, class ... Args>
	struct SerializerType {
		template <class T, class ... Args_>
		friend struct SerializerType;
		
		static constexpr auto BytesRequired = SerializerTypeTraits<Type>::BytesRequired;
		static constexpr auto TypeName = SerializerTypeTraits<Type>::Name;
	public:
		SerializerType(SerializerOutput& output) :
			m_zone(output.template acquireMemory<BytesRequired>(TypeName)) {}

		template <std::size_t Bytes>
		SerializerType(SerializerSafeZone<Bytes>& zone) :
			m_zone(zone.template acquireMemory<BytesRequired>(TypeName)) {
		}

		void write(const std::remove_pointer_t<Type>& type, Args&& ... args) { 
			SerializerTypeTraits<Type>::Write(m_zone, type, std::forward<Args>(args)...);
		}

		void read(Type& type, Args&& ... args) {
			SerializerTypeTraits<Type>::Read(m_zone, type, std::forward<Args>(args)...);
		}

		void reset() { m_zone.reset(); }

	protected:
		SerializerSafeZone<BytesRequired> m_zone;
	};
}
