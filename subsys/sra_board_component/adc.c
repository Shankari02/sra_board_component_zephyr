#include "sra_board.h"

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

#define ADC_SAMPLING_RATE 10
// Define the ADC sampling rate in milliseconds
int err;
uint32_t count = 0;
uint16_t buf;
struct adc_sequence sequence = {
	.buffer = &buf,
	/* buffer size in bytes, not number of samples */
	.buffer_size = sizeof(buf),
};

int config_adc()
{
	/* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++)
	{
		if (!adc_is_ready_dt(&adc_channels[i]))
		{
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return 0;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0)
		{
			printk("Could not setup channel #%d (%d)\n", i, err);
			return 0;
		}
	}

	return 0;
}

int read_adc()
{
	int i = 0;
	int32_t val_mv;

	(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

	err = adc_read_dt(&adc_channels[i], &sequence);
	if (err < 0)
	{
		printk("Could not read (%d)\n", err);
		return 0;
	}

	/*
	 * If using differential mode, the 16 bit value
	 * in the ADC sample buffer should be a signed 2's
	 * complement value.
	 */
	if (adc_channels[i].channel_cfg.differential)
	{
		val_mv = (int32_t)((int16_t)buf);
	}
	else
	{
		val_mv = (int32_t)buf;
	}
	err = adc_raw_to_millivolts_dt(&adc_channels[i],
								   &val_mv);
	/* conversion to mV may not be supported, skip if not */
	if (err < 0)
	{
		printk(" (value in mV not available)\n");
	}
	k_sleep(K_MSEC(10));
	return val_mv;
}
