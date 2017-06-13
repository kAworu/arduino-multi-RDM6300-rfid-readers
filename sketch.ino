/*
 * Sketch to read RFID 125kHz chips through RDM6300 modules using
 * SoftwareSerial. When the correct combination is read (see `jackpot`) a signal
 * is sent to the `relayPin`.
 *
 * NOTE: to add a tag you need to read it using rfid_reader, add it to the
 * `tags` list and increase `NUMBER_OF_TAGS`.
 */
#include <SoftwareSerial.h>

#define NREADER	4
#define NTAGS	10
#define TAGLEN	14

int jackpot[] = {
	6, /* on the first  reader, RX=2 */
	9, /* on the second reader, RX=4 */
	2, /* on the third  reader, RX=6 */
	7, /* on the fourth reader, RX=8 */
};
SoftwareSerial readers[NREADER] = {
	SoftwareSerial(/* RX=*/2, /* TX=*/3),
	SoftwareSerial(/* RX=*/4, /* TX=*/5),
	SoftwareSerial(/* RX=*/6, /* TX=*/7),
	SoftwareSerial(/* RX=*/8, /* TX=*/9),
};

/* used to trigger pin */
int relayPin = 12;
 /* signal sent to relayPin on success (LOW or HIGH) */
int jackpot_signal = LOW;

/* how many milliseconds we can wait before a report */
int max_report_millis = 2000;

int tags[NTAGS][TAGLEN] = {
	{2,49,57,48,48,54,65,53,54,49,51,51,54,3}, /* ID=1 */
	{2,49,57,48,48,54,65,53,50,53,53,55,52,3}, /* ID=2 */
	{2,49,57,48,48,54,56,69,68,57,68,48,49,3}, /* ID=3 */
	{2,49,57,48,48,54,56,70,49,57,56,49,56,3}, /* ID=4 */
	{2,49,57,48,48,54,65,48,70,56,55,70,66,3}, /* ID=5 */
	{2,49,57,48,48,54,65,51,49,56,52,67,54,3}, /* ID=6 */
	{2,49,57,48,48,54,57,70,57,69,66,54,50,3}, /* ID=7 */
	{2,49,57,48,48,54,57,50,54,66,51,69,53,3}, /* ID=8 */
	{2,49,57,48,48,54,65,48,66,51,55,52,70,3}, /* ID=9 */
	{2,49,57,48,48,54,56,67,54,53,65,69,68,3}, /* ID=10 */
};

/* delay between SoftwareSerial.begin() and SoftwareSerial.available() */
int begin_delay = 60;
/* delay between SoftwareSerial.available() and SoftwareSerial.read() */
int available_delay = 30;

/* accouting */
int stats[NREADER][NTAGS];
/* used for reporting timekeeping */
int last_report_millis;

void setup() {
	pinMode(relayPin, OUTPUT); // output signal trigger
	Serial.begin(9600); // start serial to PC
	last_report_millis = millis();
}

/* FIXME: C++ code, doesn't work with the Arduino SDK although works fine with
   gcc */
class ReadResult
{
	public:

	ReadResult(int nread, int tag):
	    nread_(nread),
	    tag_(tag)
	{ }

	int nread()
	{
		return nread_;
	}

	int tag()
	{
		return tag_;
	}

	protected:

	int nread_;
	int tag_;

};


/*
 * return a {nread, tag} pair where tag is the tag index most read so far and
 * nread the read count.
 */
ReadResult *last_read(int reader_index) {
	SoftwareSerial RFID = readers[reader_index];
	int tag = -1, nread = 0;
	/* compute a max on stats to find the most read tag index so far */
	for (int i = 0; i < NTAGS; i++) {
		int n = stats[reader_index][i];
		if (n > nread) {
			nread = n;
			tag   = i;
		}
	}

	return (new ReadResult(nread, tag));
}

void RFID_read(int reader_index) {
	/* shortcut if we already read 3 times the same tag */
	ReadResult *prev = last_read(reader_index);
	int nread = prev->nread();
	delete prev;
	if (nread >= 3)
		return;

	int data[TAGLEN]; // temp buffer where the rfid data is stored
	SoftwareSerial RFID = readers[reader_index];

	RFID.begin(9600); // start serial to RFID reader
	delay(begin_delay);
	if (RFID.available() > 0) {
		delay(available_delay);
		for (int i = 0 ; i < TAGLEN; i++)
			data[i] = RFID.read();
		RFID.flush();
		for (int i = 0; i < NTAGS; i++) {
			if (memcmp(data, tags[i], sizeof(tags[i])) == 0) {
				stats[reader_index][i]++;
			}
		}
	}
	RFID.end();
}

void loop() {
	for (int r = 0; r < NREADER; r++)
		RFID_read(r);

	int now = millis();
	if ((now - last_report_millis) > max_report_millis) {
		int matches = 0;
		last_report_millis = now;
		for (int r = 0; r < NREADER; r++) {
			int the_one = 0, nread = 0;
			for (int x = 0; x < NTAGS; x++) {
				int n = stats[r][x];
				if (n > nread) {
					the_one = x;
					nread = n;
				}
			}
			Serial.print("===> RFID");
			Serial.print(r + 1, DEC);
			Serial.print(": ");
			if (nread == 0) {
				Serial.println(" nothing (");
			} else {
				Serial.print(the_one + 1, DEC);
				Serial.print(" (read ");
				Serial.print(nread, DEC);
				Serial.print(" times, ");
			}
			int expected = jackpot[r];
			Serial.print("expected=");
			Serial.print(expected, DEC);
			if (expected == (the_one + 1)) {
				matches++;
				Serial.print(", matching");
			}
			Serial.println(")");
		}
		Serial.println();
		int success = (matches == NREADER);
		if (success) {
			digitalWrite(relayPin, jackpot_signal);
			Serial.println("Super content <3");
		} else {
			digitalWrite(relayPin, !jackpot_signal);
			Serial.println("Pas encore super content :/");
		}
		memset(stats, 0, NREADER * NTAGS * sizeof(int));
	}
}
