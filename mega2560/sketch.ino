#define NREADER	4
#define NTAGS	10
#define TAGLEN	14

int jackpot[] = {
  8,  /* on the first  reader, RX=15 */
  10, /* on the second reader, RX=17 */
  5,  /* on the third reader,  RX=19 */
  4,  /* on the fourth reader, RX=0 */
};

int relayPin = 12; /* used to trigger pin */

int jackpot_signal = LOW; /* signal sent to relayPin on success (LOW or HIGH) */

int max_report_millis = 1000; /* how many milliseconds we can wait before a report */
int begin_delay = 10; /* delay between SoftwareSerial.begin() and SoftwareSerial.available() */
int available_delay = 20;

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

/* accouting */
int stats[NREADER][NTAGS];
/* used for reporting timekeeping */
int last_report_millis;

void setup() {
  pinMode(relayPin, OUTPUT); // output signal trigger
  Serial.begin(9600);  // start main serial to PC
  Serial1.begin(9600); // start serial1 to PC
  Serial2.begin(9600); // start serial2 to PC
  Serial3.begin(9600); // start serial3 to PC
  last_report_millis = millis();
}

void RFID_read(HardwareSerial &refSer, int serial_index) {
	int data[TAGLEN]; // temp buffer where the rfid data is stored	
        delay(begin_delay);
	if (refSer.available() > 0) {
		delay(available_delay);
		for (int i = 0 ; i < TAGLEN; i++) {
			data[i] = refSer.read();
                }
		refSer.flush();
		for (int i = 0; i < NTAGS; i++) {
			if (memcmp(data, tags[i], sizeof(tags[i])) == 0) {
				stats[serial_index][i]++;                                
			}
		}

	}
        refSer.end();
        refSer.begin(9600);
       
}

void loop() {
	  
	  RFID_read(Serial1, 0);
	  RFID_read(Serial2, 1);
	  RFID_read(Serial3, 2);
          RFID_read(Serial, 3);
  
          int now = millis();
	
          if ((now - last_report_millis) > max_report_millis) {
		int matches = 0;
		last_report_millis = now;
		
                        // serial 1
			int the_one_1 = 0, nread_1 = 0;

			for (int x = 0; x < NTAGS; x++) {
				int n_1 = stats[0][x];
				if (n_1 > nread_1) {
					the_one_1 = x;
					nread_1 = n_1;
				}
			}
			Serial.print("===> RFID");
			Serial.print("1");
			Serial.print(": ");
			if (nread_1 == 0) {
				Serial.println(" nothing");
			} else {
				int expected = jackpot[0];
				Serial.print(the_one_1 + 1, DEC);
				Serial.print(" (read ");
				Serial.print(nread_1, DEC);
				Serial.print(" times");
				Serial.print(", expected=");
				Serial.print(expected, DEC);
				Serial.print(", the_one_1=");
				Serial.print(the_one_1, DEC);
				if (expected == (the_one_1 + 1)) {
					matches++;
					Serial.print(", matching");
				}
				Serial.println(")");
			}

                        // serial 2
                        int the_one_2 = 0, nread_2 = 0;
                        
			for (int x = 0; x < NTAGS; x++) {
				int n_2 = stats[1][x];
				if (n_2 > nread_2) {
					the_one_2 = x;
					nread_2 = n_2;
				}
			}
			Serial.print("===> RFID");
			Serial.print("2");
			Serial.print(": ");
			if (nread_2 == 0) {
				Serial.println(" nothing");
			} else {
				int expected = jackpot[1];
				Serial.print(the_one_2 + 1, DEC);
				Serial.print(" (read ");
				Serial.print(nread_2, DEC);
				Serial.print(" times");
				Serial.print(", expected=");
				Serial.print(expected, DEC);
				Serial.print(", the_one_2=");
				Serial.print(the_one_2, DEC);
				if (expected == (the_one_2 + 1)) {
					matches++;
					Serial.print(", matching");
				}
				Serial.println(")");
			}

                        // serial 3
                        int the_one_3 = 0, nread_3 = 0;
                        
			for (int x = 0; x < NTAGS; x++) {
				int n_3 = stats[2][x];
				if (n_3 > nread_3) {
					the_one_3 = x;
					nread_3 = n_3;
				}
			}
			Serial.print("===> RFID");
			Serial.print("3");
			Serial.print(": ");
			if (nread_3 == 0) {
				Serial.println(" nothing");
			} else {
				int expected = jackpot[2];
				Serial.print(the_one_3 + 1, DEC);
				Serial.print(" (read ");
				Serial.print(nread_3, DEC);
				Serial.print(" times");
				Serial.print(", expected=");
				Serial.print(expected, DEC);
				Serial.print(", the_one_1=");
				Serial.print(the_one_3, DEC);
				if (expected == (the_one_3 + 1)) {
					matches++;
					Serial.print(", matching");
				}
				Serial.println(")");
			}                       

                        // main serial
                        int the_one_4 = 0, nread_4 = 0;
                        
			for (int x = 0; x < NTAGS; x++) {
				int n_4 = stats[3][x];
				if (n_4 > nread_4) {
					the_one_4 = x;
					nread_4 = n_4;
				}
			}
			Serial.print("===> RFID");
			Serial.print(" MAIN");
			Serial.print(": ");
			if (nread_4 == 0) {
				Serial.println(" nothing");
			} else {
				int expected = jackpot[3];
				Serial.print(the_one_4 + 1, DEC);
				Serial.print(" (read ");
				Serial.print(nread_4, DEC);
				Serial.print(" times");
				Serial.print(", expected=");
				Serial.print(expected, DEC);
				Serial.print(", the_one_4=");
				Serial.print(the_one_4, DEC);
				if (expected == (the_one_4 + 1)) {
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
