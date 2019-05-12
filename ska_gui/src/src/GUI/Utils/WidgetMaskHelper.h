#pragma once

namespace ska {
	class WStaticCounterGlobal {
	protected:
		static unsigned int getCounterAndInc();
	};

	class WidgetMaskHelper {
	private:
		WidgetMaskHelper() = default;

	public:
		template <class T>
		static unsigned int getMask() {
			return (WidgetMaskHelper::template getWidgetMaskCounter<T>());
		}

	private:
		template <class T>
		class WStaticCounterSpecialized : public WStaticCounterGlobal {
		public:
			WStaticCounterSpecialized() {
				m_mask = getCounterAndInc();
			}

			unsigned int getMask() const{
				return m_mask;
			}

		private:
			unsigned int m_mask;
		};

		template <class T>
		static unsigned int getWidgetMaskCounter() {
			static WStaticCounterSpecialized<T> wc;
			return wc.getMask();
		}
	};
}
