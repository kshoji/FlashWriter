#! /usr/bin/env perl

use Device::SerialPort;
use Time::HiRes;

if (scalar(@ARGV) != 2) {
  print "Usage: flashWrite.pl /dev/tty.usbserial-XXXX fileToWrite";
  exit;
}

my $port = Device::SerialPort->new($ARGV[0]);
$port->databits(8);
$port->baudrate(19200);
$port->parity('none');
$port->stopbits(1);
$port->datatype('raw');

# wait for connect
my $received = '';
while (1) {
  $received = $received . $port->read(1);
  if ($received =~ /checksum chip/) {
    last;
  }
}

print "initialized\n";

$port->write("DEL");
$received = '';
while (1) {
  $received = $received . $port->read(1);
  if ($received =~ /erase completed!/) {
    last;
  }
}

print "erased\n";

open(FILE, $ARGV[1]);
binmode(FILE);
my ($buf, $data, $n, $address);
$address = 0;
while (1) {
  $n = read(FILE, $data, 256);
  if ($n == 0 || $n < 256) {
    last;
  }

  while (1) {
    $port->write('WRT');
    $port->write(pack('C4', 
      (($address >> 24) & 0xff), 
      (($address >> 16) & 0xff), 
      (($address >> 8) & 0xff), 
      (($address >> 0) & 0xff)
      ));
    $port->write($data);

    $received = '';
    Time::HiRes::sleep(0.1);

    my $completed = 0;
    for ($n = 0; $n < 1000; $n++) {
      $received = $received . $port->read(1);
      if ($received =~ /write completed!/) {
        $completed = 1;
        last;
      } elsif ($received =~ /write failed\./) {
        last;
      }
      Time::HiRes::sleep(0.01);
    }
    print $received, "\n";

    if ($completed == 1) {
      print $address , "\n";
      $address += 256;
      last;
    }
  }
}
close(FILE);
