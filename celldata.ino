#include "Particle.h"
#include "CellularHelper.h"

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

const unsigned long STARTUP_WAIT_TIME_MS = 4000;
const unsigned long MODEM_ON_WAIT_TIME_MS = 4000;
const unsigned long CLOUD_PRE_WAIT_TIME_MS = 5000;
const unsigned long DISCONNECT_WAIT_TIME_MS = 10000;
const int SYSOUT_SIZE = 120;
char sysout[SYSOUT_SIZE];

// Forward declarations
void reportData();
void runCellularTests();
bool reported = false;

void setup() {
}

void loop() {
  if (!reported) {
    reported = TRUE;
    reportData();
  }
}

void reportData() {
	prntflog("ICCID=%s", CellularHelper.getICCID().c_str());
	prntflog("manufacturer=%s", CellularHelper.getManufacturer().c_str());
	prntflog("model=%s", CellularHelper.getModel().c_str());
	prntflog("firmware version=%s", CellularHelper.getFirmwareVersion().c_str());
	prntflog("ordering code=%s", CellularHelper.getOrderingCode().c_str());
	prntflog("IMEI=%s", CellularHelper.getIMEI().c_str());
	prntflog("IMSI=%s", CellularHelper.getIMSI().c_str());

  runCellularTests();
}

// Various tests to find out information about the cellular network we connected to
void runCellularTests() {

	prntflog("operator name=%s", CellularHelper.getOperatorName().c_str());

	CellularHelperRSSIQualResponse rssiQual = CellularHelper.getRSSIQual();
	int bars = CellularHelperClass::rssiToBars(rssiQual.rssi);
	prntflog("rssi=%d, qual=%d, bars=%d", rssiQual.rssi, rssiQual.qual, bars);

	// First try to get info on neighboring cells. This doesn't work for me using the U260.
	// Get a maximum of 8 responses
	CellularHelperEnvironmentResponseStatic<8> envResp;
	CellularHelper.getEnvironment(CellularHelper.ENVIRONMENT_SERVING_CELL_AND_NEIGHBORS, envResp);
	if (envResp.resp != RESP_OK) {
		// We couldn't get neighboring cells, so try just the receiving cell
		CellularHelper.getEnvironment(CellularHelper.ENVIRONMENT_SERVING_CELL, envResp);
	}
	envResp.prntflogResponse();
	CellularHelperLocationResponse locResp = CellularHelper.getLocation();
	prntflog(locResp.toString());

	prntflog("ping 8.8.8.8=%d", CellularHelper.ping("8.8.8.8"));
	prntflog("dns device.spark.io=%s", CellularHelper.dnsLookup("device.spark.io").toString().c_str());
}

void prntflog(String format, String value) {
  sprintf(sysout, format, value);
  log(label, "");
}

void log(String label, String value) {
  Particle.publish(label, value);
}
