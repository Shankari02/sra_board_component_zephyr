#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

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
// static const struct adc_dt_spec an_mb1_dt = MY_ADC_DT_SPEC(3);
// static const struct adc_dt_spec an_mb2_dt = MY_ADC_DT_SPEC(4);


//static const int adc_io[1] = {0}; // Example GPIO pins
#define ADC_SAMPLING_RATE 10 // Define the ADC sampling rate in milliseconds

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
    // if (!adc_obj || !adc_obj->dev) {
    //     LOG_ERR("ADC device is not initialized");
    //     return -EINVAL;
    // }

    // for (int i = 0; i < ARRAY_SIZE(adc_io); i++) {
    //     adc_obj->channel_cfg.channel_id = adc_io[i];
    //     adc_obj->channel_cfg.sampling_delay = K_MSEC(ADC_SAMPLING_RATE);
    //     adc_obj->channel_cfg.gain = ADC_GAIN_1;
    //     adc_obj->channel_cfg.reference = ADC_REF_INTERNAL;
    //     adc_obj->channel_cfg.input_positive = adc_io[i];

    //     if (adc_channel_setup(adc_obj->dev, &adc_obj->channel_cfg) < 0) {
    //         LOG_ERR("Failed to configure ADC channel %d", adc_io[i]);
    //         return -EIO;
    //     }
    // }
    /* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return 0;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			printk("Could not setup channel #%d (%d)\n", i, err);
			return 0;
		}
	}

    return 0;
}


int read_adc()
{
//     if (!adc_obj || !adc_obj->dev) {
//         LOG_ERR("ADC device is not initialized");
//         return -EINVAL;
//     }

//     uint16_t sample_buffer;
//     struct adc_sequence sequence = {
//         .channels = BIT(channel),
//         .buffer = &sample_buffer,
//         .buffer_size = sizeof(sample_buffer),
//         .resolution = ADC_RESOLUTION,
//         .oversampling = 0, // No oversampling by default
//         .callback = NULL,   // No callback
//         .flags = 0         // No specific flags
//     };

//     int ret = adc_read(adc_obj->dev, &sequence);
//     if (ret < 0) {
//         LOG_ERR("Failed to read ADC value from channel %d", channel);
//         return ret;
//     }

//     return sample_buffer; // Return raw sample value or perform necessary calibration
#ifndef CONFIG_COVERAGE
	while (1) {
#else
	for (int k = 0; k < 10; k++) {
#endif
		printk("ADC reading[%u]:\n", count++);
		for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
			int32_t val_mv;

			printk("- %s, channel %d: ",
			       adc_channels[i].dev->name,
			       adc_channels[i].channel_id);

			(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

			err = adc_read_dt(&adc_channels[i], &sequence);
			if (err < 0) {
				printk("Could not read (%d)\n", err);
				continue;
			}

			/*
			 * If using differential mode, the 16 bit value
			 * in the ADC sample buffer should be a signed 2's
			 * complement value.
			 */
			if (adc_channels[i].channel_cfg.differential) {
				val_mv = (int32_t)((int16_t)buf);
			} else {
				val_mv = (int32_t)buf;
			}
			printk("%"PRId32, val_mv);
			err = adc_raw_to_millivolts_dt(&adc_channels[i],
						       &val_mv);
			/* conversion to mV may not be supported, skip if not */
			if (err < 0) {
				printk(" (value in mV not available)\n");
			} else {
				printk(" = %"PRId32" mV\n", val_mv);
			}
		}

		k_sleep(K_MSEC(1000));
	}

}
