# Lepton Data Sender

This is a small program that runs on Intel Edison or BeagleBone Black and sends raw [FLIR Lepton](https://www.sparkfun.com/products/13233) infrared camera frames to the [Lepton Visualizer](https://github.com/jeradesign/Lepton_Visualizer) Processing sketch via TCP/IP.

## Lepton

This &nbsp;code is designed for the [FLIR Lepton breakout board](https://www.sparkfun.com/products/13233), available from SparkFun, among other places. The breakout board supports either 3.3v or 5v signals. The hookup tables below assume 3.3v.

## Lepton Visualizer

To use this code, you'll need to have the [Lepton Visualizer](https://github.com/jeradesign/Lepton_Visualizer) Processing script running on another computer to display the results. You must start the Lepton Visualizer each time before running Lepton Data Sender. See the Lepton Visualizer README.md file for setup instructions.

## Intel Edison

### Hardware Setup

This hardware setup is for the Intel Edison Arduino board. Two important notes:

1.  <span style="line-height: 28px;">The wiring assumes the Arduino board is set to 3.3v via the J9 jumper.
</span>
2.  <span style="line-height: 28px;">The Edison SPI bus appears to have problems keeping up with the Lepton. Expect lots of garbage frames. (This does not&nbsp;happen on BeagleBone Black.)</span>
<pre>Lepton   Edison with Arduino board
CS       ~10
MOSI     ~11   (Not currently used)
MISO      12
CLK       13
GND       GND
VIN       3.3V
SDA       SDA  (Not currently used)
SCL       SCL  (Not currently used)
</pre>

### Building and Running

(These instructions are based on&nbsp;edison-image-ww25.5-15.)

Open the project in the [Intel IoT version of Eclipse](https://software.intel.com/en-us/iot/downloads), build and run.&nbsp;<span style="font-size: 16pt; line-height: 21pt; text-indent: 2em;">The program will exit almost immediately. You'll need to execute it from the command line with the proper parameters to actually send data over.</span>

From a terminal window, enter:

<pre>./Lepton_Data_Sender &lt;VisualizerIP&gt;
</pre>

Where &lt;VisualizerIP&gt; is the IP address of the machine running the Lepton Visualizer Processing sketch.

## BeagleBone Black

### Hardware Setup

Note: A WiFi connection to the Lepton Visualizer machine will usually work fine, but for best results, use a hard-wired Ethernet connection via the BeagleBone's RJ-45 connector.

<pre>Lepton   BeagleBone Black P9 Header
CS       17  (SPI0_CS0)
MOSI     No Connection
MISO     21  (SPI0_D0)
CLK      22  (SPI0_CLK)
GND      1   (GND)
VIN      3   (DC_3.3V)
SDA      20 (I2C2_SDA) (Not currently used)
SCL      19 (I2C2_SCL) (Not currently used)
</pre>

### Building and Running

(These instructions were tested on the BeagleBoard.org Debian Image 2015-03-01)

You'll need to install [MRAA](https://github.com/intel-iot-devkit/mraa) to run (MRAA comes preinstalled on Edison).

Once installed, you'll need to add the MRAA library to your&nbsp;LD_LIBRARY_PATH:

<pre>export LD_LIBRARY_PATH=/usr/local/lib/arm-linux-gnueabihf/:</pre>

BeagleBone has the SPI bus turned off by default. To Turn it on, type:

<pre>echo ADAFRUIT-SPI0 &gt; /sys/devices/bone_capemgr.9/slots</pre>

Set up the environment variable "LEPTON_VISUALIZER_IP" to the IP of the machine running the Lepton Visualizer Processing sketch.

<pre>export LEPTON_VISUALIZER_IP=&lt;IP address of Lepton Visualizer Machine&gt;</pre>

At this point a simple "make" should build (as needed) and run the Lepton Data Sender program, and you should start seeing images in the Lepton Visualizer.

To make without running:

<pre>make Lepton_Data_Sender</pre>

To run manually:

<pre>./Lepton_Data_Sender &lt;VisualizerIP&gt;</pre>

Where &lt;VisualizerIP&gt; is the IP address of the machine running the Lepton Visualizer Processing sketch.
