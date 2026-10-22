#include "feature.h"
#include "cpu/cu.h"
#include "cpu/int.h"
#include "hw/pit.h"

word hwft_get_available_features() {
	return FT_PIT_BIT;
}

word hwft_get_info(HardwareFeature feature) {
	switch(feature) {
		case FT_PIT: {
			return PIT_COUNT;
		}
		default: ;
	}

	cu_raise_int(INT_IOP, INT_IOP_0_UNKNOWN_FEATURE, 0);
	return 0;
}
