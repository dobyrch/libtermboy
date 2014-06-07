#include <pulse/simple.h>
#include <pulse/error.h>

int main(int argc, char *argv[]) {
	pa_simple *s;
	pa_sample_spec ss;
	ss.format = PA_SAMPLE_U8;
	ss.channels = 1;
	ss.rate = 32000;
	s = pa_simple_new(NULL,               // Use the default server.
			"Fooapp",           // Our application's name.
			PA_STREAM_PLAYBACK,
			NULL,               // Use the default device.
			"Music",            // Description of our stream.
			&ss,                // Our sample format.
			NULL,               // Use default channel map
			NULL,               // Use default buffering attributes.
			NULL                // Ignore error code.
			);

	int data = 0xFFFFFFFF;
	while (1) {
		pa_simple_write(s, &data, 4, NULL);
	}
}
